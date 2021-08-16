

#include "Compiler/CodeGen/Internal.hpp"

bool StmtForEach::codegen(Bytecode& bc) const {
  bc.add(idx(), OP_PUSH_LOOP);

  // create __<loop_var> from expression
  m_expr->codegen(bc);
  bc.adds(m_expr->idx(), OP_LOAD, ODT_STRING, "__" + m_loop_var->data);
  bc.addb(m_expr->idx(), OP_CREATE, false);

  size_t continue_jmp_pos = bc.size();
  // let <loop_var> = __<loop_var>.next()
  bc.adds(m_expr->idx(), OP_LOAD, ODT_IDEN, "__" + m_loop_var->data);
  bc.adds(m_expr->idx(), OP_LOAD, ODT_STRING, "next");
  bc.adds(m_expr->idx(), OP_MEM_FNCL, ODT_STRING, "");
  // will be set later
  size_t jmp_loop_out_loc1 = bc.size();
  bc.addsz(m_loop_var->pos, OP_JMPN, 0);
  bc.adds(m_loop_var->pos, OP_LOAD, ODT_STRING, m_loop_var->data);
  bc.addb(m_loop_var->pos, OP_CREATE, false);

  // now comes the body of the loop
  size_t body_begin = bc.size();
  m_body->codegen(bc);
  size_t body_end = bc.size();

  bc.addsz(idx(), OP_JMP, continue_jmp_pos);

  // pos where break goes
  size_t break_jmp_loc = bc.size();
  bc.add(idx(), OP_POP_LOOP);

  bc.updatesz(jmp_loop_out_loc1, break_jmp_loc);

  // update all continue and break calls
  for (size_t i = body_begin; i < body_end; ++i) {
    if (bc.at(i) == OP_CONTINUE && bc.get()[i].data.sz == 0)
      bc.updatesz(i, continue_jmp_pos);
    if (bc.at(i) == OP_BREAK && bc.get()[i].data.sz == 0)
      bc.updatesz(i, break_jmp_loc);
  }
  return true;
}