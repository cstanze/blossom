#ifndef VM_OPCODES_COMPAT_HPP
#define VM_OPCODES_COMPAT_HPP

enum OpCodes {
  OP_CREATE,

  OP_STORE,
  OP_LOAD,
  OP_ULOAD,

  OP_JMP,
  OP_JMPT,

  OP_JMPF,

  OP_JMPTPOP,

  OP_JMPFPOP,

  OP_JMPN,

  OP_BODY_TILL,

  OP_MKFN,

  OP_BLKA,
  OP_BLKR,

  OP_FNCL,
  OP_MEM_FNCL,
  OP_ATTR,

  OP_RET,
  OP_CONTINUE,
  OP_BREAK,

  OP_PUSH_LOOP,
  OP_POP_LOOP,

  OP_PUSH_JMP,
  OP_PUSH_JMPN,

  OP_POP_JMP,

  _OP_LAST
};

enum OpDataType {
  ODT_INT,
  ODT_FLOAT,
  ODT_STRING,
  ODT_IDEN,

  ODT_SZ,

  ODT_BOOL,

  ODT_NIL,

  _ODT_LAST
};

static const char *OpCodeNames[_OP_LAST] = {
  "CREATE",
  "STORE",
  "LOAD",
  "ULOAD",
  "JMP",
  "JMPT",
  "JMPF",
  "JMPTPOP",
  "JMPFPOP",
  "JMPN",
  "BODY_TILL",
  "MKFN",
  "BLKA",
  "BLKR",
  "FNCL",
  "MEM_FNCL",
  "ATTR",
  "RET",
  "CONTINUE",
  "BREAK",
  "PUSH_LOOP",
  "POP_LOOP",
  "PUSH_JMP",
  "PUSH_JMPN",
  "POP_JMP",
};

static const char *OpDataTypeNames[] = {
  "INT",
  "FLOAT",
  "STRING",
  "IDEN",
  "SIZE",
  "BOOL",
  "NIL",
};

// union OpData {
//   size_t sz;
//   char *s;
//   bool b;
// };

// struct Op {
//   size_t src_id;
//   size_t idx;
//   OpCodes op;
//   OpDataType dtype;
//   OpData data;
// };

#endif