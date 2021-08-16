#include <unordered_map>

#include "Compiler/CodeGen/Internal.hpp"

Errors parseFail(StmtBase *args, StmtBase *body, StmtBase *name) {
  if (args)
    delete args;
  if (body)
    delete body;
  if (name)
    delete name;

  return E_PARSE_FAIL;
}

Errors parse_func_decl(ParseHelper &ph, StmtBase *&loc) {
  bool argsDone = false;
  bool isAnon = true;
  StmtBase *args = nullptr, *body = nullptr, *name = nullptr;
  size_t idx = ph.peak()->pos;

  if (!ph.accept(TOK_FUNC)) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos, "Expected 'func' keyword");
    return parseFail(args, body, name);
  }
  ph.next();

  if (!ph.accept(TOK_LPAREN) && !ph.accept(TOK_IDENT)) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos,
             "Expected '(' or identifier (function name)");
    return parseFail(args, body, name);
  }

  if (ph.accept(TOK_IDENT)) {
    isAnon = false;
    ph.sett(TOK_STRING);
    name = new StmtSimple(ph.peak());
    ph.next();
  }

  if (!ph.accept(TOK_LPAREN)) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos, "Expected '('");
    return parseFail(args, body, name);
  }

  ph.next();

  while (true) {
    if (ph.accept(TOK_RPAREN)) {
      ph.next();
      break;
    }

    if (argsDone) {
      Err::set(E_PARSE_FAIL, ph.peak()->pos,
               "Expected ')' to close function arguments");
      return parseFail(args, body, name);
    }

    if (parse_fn_decl_args(ph, args) != E_OK) {
      return parseFail(args, body, name);
    }
    argsDone = true;
  }

  if (!ph.accept(TOK_LBRACE)) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos,
             "Expected '{' to start function body");
    return parseFail(args, body, name);
  }

  if (parse_block(ph, body) != E_OK) {
    return parseFail(args, body, name);
  }

  loc = new StmtFnDef((StmtFnDefArgs *)args, (StmtBlock *)body,
                      (StmtSimple *)name, isAnon, idx);
  return E_OK;
}

Errors parse_fn_decl_args(ParseHelper &ph, StmtBase *&loc) {
  std::vector<const StmtBase *> args;
  const StmtSimple *kw_arg = nullptr, *va_arg = nullptr;
  StmtBase *expr = nullptr;
  std::unordered_map<std::string, size_t> done_assn_args;

  size_t idx = ph.peak()->pos;
begin:
  if (va_arg) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos,
             "cannot have any argument after variadic arg declaration");
    goto fail;
  }
  if (ph.accept(TOK_STRING)) { // check kw arg
    if (kw_arg) {
      Err::set(E_PARSE_FAIL, ph.peak()->pos,
               "function can't have multiple keyword args (previous: %s)",
               kw_arg->val()->data.c_str());
      goto fail;
    }
    kw_arg = new StmtSimple(ph.peak());
    ph.next();
  } else if (ph.accept(TOK_IDENT) && ph.peakt(1) != TOK_ASSN &&
             ph.peakt(1) != TOK_TDOT) {
    if (done_assn_args.size() > 0) {
      Err::set(E_PARSE_FAIL, ph.peak()->pos,
               "cannot have a simple parameter after default argument");
      goto fail;
    }
    ph.sett(TOK_STRING);
    args.push_back(new StmtSimple(ph.peak()));
    ph.next();
  } else if (ph.acceptd() &&
             (ph.peakt(1) == TOK_ASSN ||
              ph.peakt(1) == TOK_TDOT)) { // since STR is checked above, won't
                                          // be bothered with it anymore
    // but we still have to make IDEN data type to STR
    if (ph.accept(TOK_IDENT))
      ph.sett(TOK_STRING);
    if (ph.peakt(1) == TOK_ASSN) {
      const lex::tok_t *lhs = ph.peak();
      StmtBase *rhs = nullptr;
      ph.next();
      ph.next();
      if (parse_expr_15(ph, rhs) != E_OK) {
        goto fail;
      }
      if (done_assn_args.find(lhs->data) != done_assn_args.end()) {
        Err::set(E_PARSE_FAIL, lhs->pos,
                 "cannot have more than one assigned argument of same name");
        delete rhs;
        goto fail;
      }
      args.push_back(new StmtFnAssnArg(new StmtSimple(lhs), rhs));
      done_assn_args[lhs->data] = lhs->pos;
    } else if (ph.peakt(1) == TOK_TDOT) { // perhaps a variadic
      va_arg = new StmtSimple(ph.peak());
      ph.next();
      ph.next();
    }
  } else {
    Err::set(E_PARSE_FAIL, ph.peak()->pos, "failed to parse function def args");
    goto fail;
  }

  if (ph.accept(TOK_COMMA)) {
    ph.next();
    goto begin;
  }

  loc = new StmtFnDefArgs(args, kw_arg, va_arg, idx);
  return E_OK;
fail:
  for (auto &arg : args)
    delete arg;
  return E_PARSE_FAIL;
}
