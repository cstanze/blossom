

#include "Compiler/CodeGen/Internal.hpp"

bool StmtImport::codegen(Bytecode &bc) const {
  for (auto &id : m_decls) {
    if (!id->codegen(bc))
      return false;
  }
  return true;
}

bool StmtImportBase::codegen(Bytecode &bc) const {
  auto impFnTok = new lex::tok_t(0, TOK_IDENT, "__import__");
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

  if (m_name)
    bc.addb(m_name->val()->pos, OP_CREATE, false);

  if (m_with.size()) {
    for (auto &with : m_with) {
      if (m_name) {
        bc.adds(m_name->val()->pos, OP_LOAD, ODT_IDEN, m_name->val()->data);
      } else {
        // load function name (import)
        if (!impFn->codegen(bc))
          return false;

        // load module name, as a string
        if (!m_mod->codegen(bc))
          return false;

        // call import again
        bc.adds(m_mod->val()->pos, OP_FNCL, ODT_STRING, "00");
      }

      // but with the new name (or alias)
      bc.adds(with->name()->pos, OP_ATTR, ODT_STRING, with->name()->data);

      // optionally, alias the imported module
      if (with->alias()) {
        bc.adds(with->alias()->pos, OP_LOAD, ODT_STRING, with->alias()->data);
        bc.addb(with->alias()->pos, OP_CREATE, false);
      } else {
        // set the variable to the imported module name
        bc.adds(with->name()->pos, OP_LOAD, ODT_STRING, with->name()->data);
        bc.addb(with->name()->pos, OP_CREATE, false);
      }
    }
  }

  return true;
}

bool ImportWith::codegen(Bytecode &bc) const {
  return true; // dummy for now
}
