

#include "Compiler/CodeGen/Internal.hpp"

Errors parse_fn_call_args(ParseHelper& ph, StmtBase*& loc) {
  // assn_args = kw args
  std::vector<const StmtBase*> args;
  std::vector<const StmtFnAssnArg*> assn_args;
  bool va_unpack = false;
  size_t va_unpack_pos = 0;
  StmtBase* expr = nullptr;

  size_t idx = ph.peak()->pos;
begin:
  if (va_unpack) {
    Err::set(E_PARSE_FAIL, va_unpack_pos,
             "variadic unpack '...' can be on last argument only");
    goto fail;
  }
  if (ph.acceptd() && ph.peakt(1) == TOK_ASSN) {
    const lex::tok_t* lhs = ph.peak();
    if (ph.peakt() == TOK_IDENT)
      ph.sett(TOK_STRING);
    StmtBase* rhs = nullptr;
    ph.next();
    ph.next();
    if (parse_expr_15(ph, rhs) != E_OK) {
      goto fail;
    }
    assn_args.push_back(new StmtFnAssnArg(new StmtSimple(lhs), rhs));
  } else if (parse_expr_15(ph, expr) == E_OK) {
    if (ph.accept(TOK_TDOT)) {
      va_unpack = true;
      va_unpack_pos = ph.peak()->pos;
      ph.next();
    }
    args.push_back(expr);
  } else {
    Err::set(E_PARSE_FAIL, ph.peak()->pos,
             "failed to parse function call args");
    goto fail;
  }

  if (ph.accept(TOK_COMMA)) {
    ph.next();
    goto begin;
  }

  loc = new StmtFnCallArgs(args, assn_args, va_unpack, idx);
  return E_OK;
fail:
  for (auto& arg : args)
    delete arg;
  return E_PARSE_FAIL;
}
