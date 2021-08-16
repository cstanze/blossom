

#include "Compiler/CodeGen/Internal.hpp"

Errors parse_foreach(ParseHelper &ph, StmtBase *&loc) {
  const lex::tok_t *loop_var = nullptr;
  StmtBase *expr = nullptr;
  StmtBase *body = nullptr;

  size_t idx = ph.peak()->pos;
  ph.next();
  if (!ph.accept(TOK_IDENT)) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos,
             "expected identifier for loop variable");
    goto fail;
  }
  loop_var = ph.peak();
  ph.next();

  if (!ph.accept(TOK_IN)) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos,
             "expected token 'in' for foreach loop after identifier");
    goto fail;
  }
  ph.next();

  if (parse_expr_01(ph, expr) != E_OK) {
    goto fail;
  }

  if (!ph.accept(TOK_LBRACE)) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos,
             "expected left brace to begin body of loop, found: '%s'",
             TokStrs[ph.peakt()]);
    goto fail;
  }
  if (parse_block(ph, body) != E_OK) {
    goto fail;
  }
done:
  loc = new StmtForEach(loop_var, expr, body, idx);
  return E_OK;
fail:
  if (expr)
    delete expr;
  if (body)
    delete body;
  return E_PARSE_FAIL;
}
