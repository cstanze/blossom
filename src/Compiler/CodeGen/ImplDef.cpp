#include "Compiler/CodeGen/Internal.hpp"

bool StmtImplDef::codegen(Bytecode &bc) const {
  size_t btp = bc.size();
  bc.addsz(idx(), OP_BODY_TILL, 0);
  if (!m_body->codegen(bc))
    return false;
  if (bc.get().back().op != OP_RET)
    bc.addb(idx(), OP_RET, false);
  bc.updatesz(btp, bc.size());

  if (m_args) {
    if (!m_args->codegen(bc))
      return false;
  }

  bc.adds(idx(), OP_MKFN, ODT_STRING, m_args ? fn_args.back() : "00");
  if (m_args)
    fn_args.pop_back();
  if (!m_in->codegen(bc))
    return false;
  if (!m_name->codegen(bc))
    return false;
  bc.addb(idx(), OP_CREATE, true);

  return true;
}
