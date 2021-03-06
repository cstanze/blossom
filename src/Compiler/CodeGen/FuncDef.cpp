#include "Compiler/CodeGen/Internal.hpp"

std::vector<std::string> fn_args;

bool StmtFnDef::codegen(Bytecode &bc) const {
  size_t body_till_pos = bc.size();
  bc.addsz(idx(), OP_BODY_TILL, 0);
  if (!m_body->codegen(bc))
    return false;
  if (bc.get().back().op != OP_RET)
    bc.addb(idx(), OP_RET, false);
  bc.updatesz(body_till_pos, bc.size());

  if (m_args) {
    if (!m_args->codegen(bc))
      return false;
  }

  bc.adds(idx(), OP_MKFN, ODT_STRING, m_args ? fn_args.back() : "00");
  if (m_args)
    fn_args.pop_back();

  if (!m_isAnon) {
    if (m_name) {
      if (!m_name->codegen(bc))
        return false;
      bc.addb(idx(), OP_CREATE, false);
    } else {
      fprintf(stderr, "Error: function name is nullptr\n");
      return false;
    }
  }

  return true;
}

bool StmtFnDefArgs::codegen(Bytecode &bc) const {
  std::string arg_info;
  arg_info += m_kwarg ? "1" : "0";
  arg_info += m_vaarg ? "1" : "0";

  for (auto arg = m_args.rbegin(); arg != m_args.rend(); ++arg) {
    if (!(*arg)->codegen(bc))
      return false;
  }

  for (auto &arg : m_args) {
    arg_info += arg->type() == GT_FN_ASSN_ARG ? "1" : "0";
  }

  if (m_vaarg) {
    if (!m_vaarg->codegen(bc))
      return false;
  }
  if (m_kwarg) {
    if (!m_kwarg->codegen(bc))
      return false;
  }

  fn_args.push_back(arg_info);
  return true;
}
