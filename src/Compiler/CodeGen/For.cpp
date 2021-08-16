

#include "Compiler/CodeGen/Internal.hpp"

bool StmtFor::codegen(Bytecode &bc) const {
  bc.add(idx(), OP_PUSH_LOOP);

  if (m_init)
    m_init->codegen(bc);

  size_t iter_jmp_loc = bc.size();

  if (m_cond)
    m_cond->codegen(bc);
  size_t cond_fail_jmp_from = bc.size();
  if (m_cond) {
    // placeholder location
    bc.addsz(m_cond->idx(), OP_JMPFPOP, 0);
  }

  size_t body_begin = bc.size();
  m_body->codegen(bc);
  size_t body_end = bc.size();

  size_t continue_jmp_loc = bc.size();

  if (m_incr) {
    m_incr->codegen(bc);
    bc.add(m_incr->idx(), OP_ULOAD);
  }

  bc.addsz(idx(), OP_JMP, iter_jmp_loc);

  if (m_cond) {
    bc.updatesz(cond_fail_jmp_from, bc.size());
  }

  // pos where break goes
  size_t break_jmp_loc = bc.size();
  bc.add(idx(), OP_POP_LOOP);

  // update all continue and break calls
  for (size_t i = body_begin; i < body_end; ++i) {
    if (bc.at(i) == OP_CONTINUE && bc.get()[i].data.sz == 0)
      bc.updatesz(i, continue_jmp_loc);
    if (bc.at(i) == OP_BREAK && bc.get()[i].data.sz == 0)
      bc.updatesz(i, break_jmp_loc);
  }
  return true;
}
