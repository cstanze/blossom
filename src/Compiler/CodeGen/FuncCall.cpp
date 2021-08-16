

#include "Compiler/CodeGen/Internal.hpp"

std::vector<std::string> fn_call_args;

bool StmtFnCallArgs::codegen(Bytecode& bc) const {
  fn_call_args.emplace_back();

  for (auto assn_arg = m_assn_args.rbegin(); assn_arg != m_assn_args.rend();
       ++assn_arg) {
    (*assn_arg)->codegen(bc);
  }
  for (auto arg = m_args.rbegin(); arg != m_args.rend(); ++arg) {
    (*arg)->codegen(bc);
  }

  fn_call_args.back() += m_va_unpack ? '1' : '0';
  fn_call_args.back() += std::string(m_args.size(), '0');
  fn_call_args.back() += std::string(m_assn_args.size(), '1');
  return true;
}

bool StmtFnAssnArg::codegen(Bytecode& bc) const {
  m_rhs->codegen(bc);
  m_lhs->codegen(bc);
  return true;
}
