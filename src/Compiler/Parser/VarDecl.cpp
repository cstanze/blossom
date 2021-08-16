

#include "Compiler/CodeGen/Internal.hpp"

Errors parse_var_decl(ParseHelper &ph, StmtBase *&loc) {
  std::vector<const StmtVarDeclBase *> decls;
  StmtBase *decl = nullptr;

  size_t idx = ph.peak()->pos;
  if (!ph.accept(TOK_LET)) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos,
             "expected keyword 'let' here, but found: '%s'",
             TokStrs[ph.peakt()]);
    goto fail;
  }
  ph.next();

begin:
  if (parse_var_decl_base(ph, decl) != E_OK) {
    goto fail;
  }
  decls.push_back((StmtVarDeclBase *)decl);
  decl = nullptr;

  if (ph.accept(TOK_COMMA)) {
    ph.next();
    goto begin;
  }

  if (ph.accept(TOK_COLS)) {
    // Err::set(E_PARSE_FAIL, ph.peak()->pos,
    //          "expected semicolon after variable declaration, found: '%s'",
    //          TokStrs[ph.peakt()]);
    // goto fail;
    ph.next();
  }
  // ph.next();
  loc = new StmtVarDecl(decls, idx);
  return E_OK;
fail:
  for (auto &decl : decls)
    delete decl;
  return E_PARSE_FAIL;
}

Errors parse_var_decl_base(ParseHelper &ph, StmtBase *&loc) {
  const lex::tok_t *lhs = nullptr;
  StmtBase *in = nullptr, *rhs = nullptr;

  // iden index
  size_t idx = ph.peak()->pos;

  // the variable in which data is to be stored, must be a const str
  ph.sett(TOK_STRING);
  lhs = ph.peak();
  ph.next();

  if (ph.peakt() == TOK_IN) {
    ph.next();
    // 01 = parenthesized expression, func call, subscript, dot
    if (parse_expr_01(ph, in) != E_OK) {
      goto fail;
    }
  }

  if (!ph.accept(TOK_ASSN)) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos,
             "expected assignment operator here for var decl, but found: '%s'",
             TokStrs[ph.peakt()]);
    goto fail;
  }
  ph.next();

  // 15 = everything excluding comma
  if (parse_expr_15(ph, rhs) != E_OK) {
    goto fail;
  }

  loc = new StmtVarDeclBase(new StmtSimple(lhs), in, rhs);
  return E_OK;
fail:
  if (in)
    delete in;
  if (rhs)
    delete rhs;
  return E_PARSE_FAIL;
}