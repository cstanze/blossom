

#include "VM/OpCodes.hpp"

#include <cstring>

#include "VM/Memory.hpp"

const char *OpCodeStrs[_OP_LAST] = {
    "CREATE", // create a new variable
    "STORE",  // store in a name: value from stack
    "LOAD",   // load from operand, onto stack
    "UNLOAD", // unload (pop) from stack

    "JMP",          // unconditional jump to index
    "JMP_TRUE",     // jump to index if top element on stack is true - will not
                    // unload if true
    "JMP_FALSE",    // jump to index if top element on stack is false - will not
                    // unload if false
    "JMP_TRUEPOP",  // jump to index if top element on stack is true - will pop
                    // unconditionally
    "JMP_FALSEPOP", // jump to index if top element on stack is false - will
                    // pop unconditionally
    "JMP_NIL",      // jump to index if top element on stack is nil (won't pop
                    // otherwise)

    "BODY_TILL", // jump to index which is where the body (of a function) ends
                 // +
                 // 1
    "MK_FUNC",   // create a function object

    "BLK_ADD", // add count scopes
    "BLK_REM", // rem count scopes

    "FUNC_CALL",   // call a function
    "M_FUNC_CALL", // call a member function (string arg - argument format)
    "ATTRIBUTE", // get attribute from an object (string arg - argument format)

    "RETURN",   // return - bool - false pushes nil on top of stack
    "CONTINUE", // size_t operand - jump to
    "BREAK",    // size_t operand - jump to

    // for loops
    "PUSH_LOOP", // marks a loop's beginning for variable stack
    "POP_LOOP",  // marks a loop's ending for variable stack

    // for 'or' keyword as of right now
    "OP_PUSH_JMP",  // marks the position to jump to if 'or' exists in an
                    // expression
    "OP_PUSH_JMPN", // sets the variable name for last jump instruction (must
                    // occur after OP_PUSH_JMP)
    "OP_POP_JMP",   // unmarks the position to jump to if 'or' exists in an
                    // expression
};

const char *OpDataTypeStrs[_ODT_LAST] = {
    "INT",  "float", "STR", "IDEN",

    "SZ",

    "BOOL",

    "NIL",
};

inline char *scpy(const std::string &str) {
  char *res = (char *)mem::alloc(str.size() + 1);
  return strcpy(res, str.c_str());
}

Bytecode::~Bytecode() {
  for (auto &op : m_bcode) {
    if (op.dtype != ODT_SZ && op.dtype != ODT_BOOL && op.dtype != ODT_NIL) {
      mem::free(op.data.s, mem::mult8_roundup(strlen(op.data.s) + 1));
    }
  }
}

void Bytecode::add(const size_t &idx, const OpCodes op) {
  m_bcode.push_back(Op{0, idx, op, ODT_NIL, {.s = nullptr}});
}
void Bytecode::adds(const size_t &idx, const OpCodes op, const OpDataType dtype,
                    const std::string &data) {
  m_bcode.push_back(Op{0, idx, op, dtype, {.s = scpy(data)}});
}
void Bytecode::addb(const size_t &idx, const OpCodes op, const bool &data) {
  m_bcode.push_back(Op{0, idx, op, ODT_BOOL, {.b = data}});
}
void Bytecode::addsz(const size_t &idx, const OpCodes op,
                     const std::string &data) {
  m_bcode.push_back(Op{0, idx, op, ODT_SZ, {.sz = std::stoull(data)}});
}
void Bytecode::addsz(const size_t &idx, const OpCodes op, const size_t &data) {
  m_bcode.push_back(Op{0, idx, op, ODT_SZ, {.sz = data}});
}

OpCodes Bytecode::at(const size_t &pos) const {
  if (pos >= m_bcode.size())
    return _OP_LAST;
  return m_bcode[pos].op;
}

void Bytecode::updatesz(const size_t &pos, const size_t &value) {
  if (pos >= m_bcode.size())
    return;
  m_bcode[pos].data.sz = value;
}
