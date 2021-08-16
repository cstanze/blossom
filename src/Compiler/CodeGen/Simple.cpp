

#include "Compiler/CodeGen/Internal.hpp"

bool StmtSimple::codegen(Bytecode &bc) const {
  if (!m_val)
    return true;

  switch (m_val->type) {
  case TOK_INT:
    bc.adds(m_val->pos, OP_LOAD, ODT_INT, m_val->data);
    break;
  case TOK_FLOAT:
    bc.adds(m_val->pos, OP_LOAD, ODT_FLOAT, m_val->data);
    break;
  case TOK_STRING:
    bc.adds(m_val->pos, OP_LOAD, ODT_STRING, m_val->data);
    break;
  case TOK_IDENT:
    bc.adds(m_val->pos, OP_LOAD, ODT_IDEN, m_val->data);
    break;
  case TOK_TRUE: // fallthrough
  case TOK_FALSE:
    bc.addb(m_val->pos, OP_LOAD, m_val->type == TOK_TRUE);
    break;
  case TOK_NIL:
    bc.add(m_val->pos, OP_LOAD);
    break;
  default:
    return false;
  }

  return true;
}
