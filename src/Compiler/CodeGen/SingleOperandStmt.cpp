

#include "Compiler/CodeGen/Internal.hpp"

bool StmtSingleOpStmt::codegen(Bytecode &bc) const {
  size_t blk_till_pos = bc.size();

  if (m_operand)
    m_operand->codegen(bc);

  if (m_sost->type == TOK_RETURN) {
    bc.addb(idx(), OP_RET, m_operand);
  } else if (m_sost->type == TOK_CONTINUE) {
    // placeholder (updated in For, Foreach, While)
    bc.addsz(idx(), OP_CONTINUE, 0);
  } else if (m_sost->type == TOK_BREAK) {
    // placeholder (updated in For, Foreach, While)
    bc.addsz(idx(), OP_BREAK, 0);
  }
  return true;
}
