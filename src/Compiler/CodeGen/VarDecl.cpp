

#include "Compiler/CodeGen/Internal.hpp"

bool StmtVarDecl::codegen(Bytecode &bc) const {
  for (auto &vd : m_decls) {
    if (!vd->codegen(bc))
      return false;
  }
  return true;
}

bool StmtVarDeclBase::codegen(Bytecode &bc) const {
  if (!m_rhs->codegen(bc))
    return false;
  if (m_in && !m_in->codegen(bc))
    return false;
  if (!m_lhs->codegen(bc))
    return false;

  bc.addb(idx(), OP_CREATE, m_in);
  return true;
}
