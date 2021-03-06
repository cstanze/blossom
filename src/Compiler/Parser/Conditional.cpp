

#include "Compiler/CodeGen/Internal.hpp"

Errors parse_conditional(ParseHelper &ph, StmtBase *&loc) {
  std::vector<conditional_t> conds;
  conditional_t cond;
  bool got_else = false;

  const lex::tok_t *tok = nullptr;

  cond.idx = ph.peak()->pos;

_if_elif:
  if (got_else) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos,
             "cannot have an else if block after else block for a condtion");
    goto fail;
  }
  tok = ph.peak();
  ph.next();
_cond:
  if (parse_expr_15(ph, cond.condition) != E_OK) {
    goto fail;
  }
  goto block;
_else:
  if (got_else && ph.peak(1)->type != TOK_IF) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos,
             "cannot have more than one else block for a condtion");
    goto fail;
  } else if (got_else && ph.peak(1)->type == TOK_IF) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos,
             "cannot have an else if block after else block for a condtion");
    goto fail;
  }
  if (ph.peak(1)->type == TOK_IF) {
    ph.next();
    tok = ph.peak();
    ph.next();
    goto _cond;
  }
  tok = ph.peak();
  ph.next();
  got_else = true;
block:
  if (!ph.accept(TOK_LBRACE)) {
    Err::set(E_PARSE_FAIL, ph.peak()->pos, "expected block for statement '%s'",
             TokStrs[tok->type]);
    goto fail;
  }
  if (parse_block(ph, cond.body) != E_OK) {
    goto fail;
  }

  conds.push_back(cond);
  cond.condition = nullptr;
  cond.body = nullptr;

  cond.idx = ph.peak()->pos;
  if (ph.accept(TOK_ELIF)) {
    goto _if_elif;
  } else if (ph.accept(TOK_ELSE)) {
    goto _else;
  }

done:
  loc = new StmtConditional(conds, conds[0].idx);
  return E_OK;
fail:
  for (auto &c : conds) {
    if (c.condition)
      delete c.condition;
    delete c.body;
  }
  return E_PARSE_FAIL;
}
