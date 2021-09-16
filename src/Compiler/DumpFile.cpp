#include "Compiler/DumpFile.hpp"

Errors bmod_dump_file(Bytecode &bc, const char* filename) {
  FILE* fp = fopen(filename, "wb");
  if (fp == NULL) {
    Err::set(E_FILE_IO, 0,
             "Failed to open file '%s' for writing", 
             filename);
    return E_FILE_IO;
  }

  // Write magic header
  fwrite("BLSM", 1, 4, fp);

  // Write version
  size_t version_size = snprintf(NULL, 0, "%d.%d.%d", BLOSSOM_VERSION_MAJOR, BLOSSOM_VERSION_MINOR, BLOSSOM_VERSION_PATCH) + 1;
  char *version = (char*)malloc(version_size);
  sprintf(version, "%d.%d.%d", BLOSSOM_VERSION_MAJOR, BLOSSOM_VERSION_MINOR, BLOSSOM_VERSION_PATCH);
  
  // Write version size
  uint32_t version_size_le = htonl(version_size);
  fwrite(&version_size_le, 1, 4, fp);

  // Write version
  fwrite(version, 1, version_size, fp);

  // Write bytecode size
  uint32_t bc_size_le = htonl(bc.size());
  fwrite(&bc_size_le, 1, 4, fp);

  // Write bytecode
  for (size_t i = 0; i < bc.size(); i++) {
    const Op &op = bc.get()[i];

    // Write opcode op
    uint32_t op_le = htonl(op.op);
    fwrite(&op_le, 1, 4, fp);

    // Write opcode dtype
    uint32_t dtype_le = htonl(op.dtype);
    fwrite(&dtype_le, 1, 4, fp);

    // Write opcode rtype
    uint32_t rtype_le = htonl(op.rtype);
    fwrite(&rtype_le, 1, 4, fp);

    // Write opcode data
    if(op.rtype == ODT_IDEN || op.rtype == ODT_STRING) {
      uint64_t data_size_le = htonll(strlen(op.data.s) + 1);
      fwrite(&data_size_le, 1, sizeof(uint64_t), fp);
      fwrite(op.data.s, 1, strlen(op.data.s) + 1, fp);
    } else if (op.rtype == ODT_INT || op.rtype == ODT_FLOAT || op.rtype == ODT_SZ || op.rtype == ODT_BOOL) {
      uint64_t data_le = htonll(op.rtype == ODT_BOOL ? (size_t)op.data.b : op.data.sz);
      fwrite(&data_le, 1, sizeof(uint64_t), fp);
    } // nothing to write for ODT_NIL

    // Write src_id and idx
    uint32_t src_id_le = htonl(op.src_id);
    fwrite(&src_id_le, 1, 4, fp);
    uint32_t idx_le = htonl(op.idx);
    fwrite(&idx_le, 1, 4, fp);
  }

  return E_OK;
}
