

#include "Compiler/CodeGen/Internal.hpp"

Errors parse_for(ParseHelper &ph, StmtBase *&loc) {
  StmtBase *init = nullptr, *cond = nullptr, *incr = nullptr;
  StmtBase *body = nullptr;

  size_t idx = ph.peak()->pos;
  ph.next();
init:
  if (ph.accept(TOK_COLS)) {
    ph.next();
    goto cond;
  }

  // both var_decl and expr_cols include semicolon
  if (ph.accept(TOK_LET)) {
    if (parse_var_decl(ph, init) != E_OK)
      goto fail;
  } else if (parse_expr_cols(ph, init) != E_OK) {
    goto fail;
  }
cond:
  if (ph.accept(TOK_COLS)) {
    ph.next();
    goto incr;
  }

  if (parse_expr_cols(ph, cond) != E_OK) {
    goto fail;
  }
incr:
  if (ph.accept(TOK_LBRACE)) {
    goto body;
  }

  if (parse_expr(ph, incr) != E_OK) {
    goto fail;
  }
body:
  if (!ph.accept(TOK_LBRACE)) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos,
             "expected left brace to begin body of loop, found: '%s'",
             TokStrs[ph.peakt()]);
    goto fail;
  }
  if (parse_block(ph, body) != E_OK) {
    goto fail;
  }

  loc = new StmtFor(init, cond, incr, body, idx);
  return E_OK;
fail:
  if (init)
    delete init;
  if (cond)
    delete cond;
  if (incr)
    delete incr;
  if (body)
    delete body;
  return E_PARSE_FAIL;
}
