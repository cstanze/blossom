

#include "Compiler/CodeGen/Internal.hpp"

bool StmtImport::codegen(Bytecode &bc) const {
  for (auto &id : m_decls) {
    if (!id->codegen(bc))
      return false;
  }
  return true;
}

bool StmtImportBase::codegen(Bytecode &bc) const {
  auto impFnTok = new lex::tok_t(0, TOK_IDENT, "import");
  auto impFn = new StmtSimple(impFnTok);

  // load function name (import)
  if (!impFn->codegen(bc))
    return false;

  // load module name, as a string
  if (!m_mod->codegen(bc))
    return false;
  
  // call import function with module name
  bc.adds(m_mod->val()->pos, OP_FNCL, ODT_STRING, "00");

  // optionally, set the imported module to a variable
  if (m_name && !m_name->codegen(bc))
    return false;

  if(m_name)
    bc.addb(m_name->val()->pos, OP_CREATE, false);

  return true;
}
