

#include "Compiler/CodeGen/Internal.hpp"

Errors parse_while(ParseHelper &ph, StmtBase *&loc) {
  StmtBase *expr = nullptr;
  StmtBase *body = nullptr;

  size_t idx = ph.peak()->pos;
  ph.next();

  if (parse_expr_15(ph, expr) != E_OK) {
    goto fail;
  }
  if (parse_block(ph, body) != E_OK) {
    goto fail;
  }

  loc = new StmtWhile(expr, body, idx);
  return E_OK;
fail:
  if (expr)
    delete expr;
  if (body)
    delete body;
  return E_PARSE_FAIL;
}
