

#include "Compiler/CodeGen/Internal.hpp"

bool StmtConditional::codegen(Bytecode &bc) const {
  std::vector<size_t> body_jmps;
  for (size_t i = 0; i < m_conds.size(); ++i) {
    size_t false_jmp_pos = 0;
    if (m_conds[i].condition) {
      m_conds[i].condition->codegen(bc);
      false_jmp_pos = bc.size();
      bc.addsz(m_conds[i].idx, OP_JMPFPOP, 0);
    }

    m_conds[i].body->codegen(bc);
    if (i < m_conds.size() - 1) {
      body_jmps.push_back(bc.size());
      bc.addsz(m_conds[i].idx, OP_JMP, 0);
    }
    if (m_conds[i].condition) {
      bc.updatesz(false_jmp_pos, bc.size());
    }
  }

  size_t jmp_to = bc.size();
  for (auto &jmp : body_jmps) {
    bc.updatesz(jmp, jmp_to);
  }

  return true;
}
