

#include "Compiler/CodeGen/Internal.hpp"

Errors parse_single_operand_stmt(ParseHelper &ph, StmtBase *&loc) {
  const lex::tok_t *sost = ph.peak();
  StmtBase *operand = nullptr;

  ph.next();

  if (ph.accept(TOK_COLS)) {
    ph.next();
    goto done;
  }

  if (sost->type == TOK_CONTINUE || sost->type == TOK_BREAK) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos,
             "statement of type '%s' expects semicolon after the keyword, "
             "found: '%s'",
             TokStrs[sost->type], TokStrs[ph.peakt()]);
    goto fail;
  }

  if (sost->type == TOK_RETURN && ph.accept(TOK_LBRACE)) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos,
             "'return' statement expects a semicolon or expression after the "
             "keyword, "
             "not a block");
    goto fail;
  }

  if (ph.accept(TOK_LBRACE)) {
    if (parse_block(ph, operand) != E_OK)
      goto fail;
    goto done;
  }

  if (parse_expr_15(ph, operand) != E_OK) {
    goto fail;
  }

  if (!ph.accept(TOK_COLS)) {
    Err::set(
        E_PARSE_FAIL, ph.peak()->pos,
        "expected semicolon to denote end of statement '%s',"
        " but found: '%s'", TokStrs[sost->type], TokStrs[ph.peakt()]);
    goto fail;
  }
  ph.next();

done:
  loc = new StmtSingleOpStmt(sost, operand);
  return E_OK;
fail:
  if (operand)
    delete operand;
  return E_PARSE_FAIL;
}
