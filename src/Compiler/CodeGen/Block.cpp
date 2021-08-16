

#include "Compiler/CodeGen/Internal.hpp"

bool StmtBlock::codegen(Bytecode& bc) const {
  if (!m_no_brace)
    bc.addsz(idx(), OP_BLKA, 1);

  for (auto& stmt : m_stmts) {
    if (!stmt->codegen(bc))
      return false;
  }

  if (!m_no_brace)
    bc.addsz(idx(), OP_BLKR, 1);
  return true;
}
