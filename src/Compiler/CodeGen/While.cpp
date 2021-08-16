

#include "Compiler/CodeGen/Internal.hpp"

bool StmtWhile::codegen(Bytecode &bc) const {
  bc.add(idx(), OP_PUSH_LOOP);

  // loop expression
  size_t begin_loop = bc.size();
  m_expr->codegen(bc);

  size_t jmp_loop_out_loc = bc.size();
  bc.addsz(idx(), OP_JMPFPOP, 0);

  size_t body_begin = bc.size();
  m_body->codegen(bc);
  size_t body_end = bc.size();

  bc.addsz(idx(), OP_JMP, begin_loop);

  bc.updatesz(jmp_loop_out_loc, bc.size());

  size_t break_jmp_loc = bc.size();
  bc.add(idx(), OP_POP_LOOP);

  // update all continue and break calls
  for (size_t i = body_begin; i < body_end; ++i) {
    if (bc.at(i) == OP_CONTINUE && bc.get()[i].data.sz == 0)
      bc.updatesz(i, begin_loop);
    if (bc.at(i) == OP_BREAK && bc.get()[i].data.sz == 0)
      bc.updatesz(i, break_jmp_loc);
  }
  return true;
}
