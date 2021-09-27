#include "VM/Consts.hpp"

namespace blossom {
namespace consts {
VarBase *get(VMState &vm, const OpDataType type, const OpData &opd,
             const size_t &src_id, const size_t &idx) {
  switch (type) {
  case ODT_BOOL:
    return opd.b ? vm.tru : vm.fals;
  case ODT_NIL:
    return vm.nil;
  case ODT_INT:
    return make_all<VarInt>(opd.s, src_id, idx);
  case ODT_FLOAT:
    return make_all<VarFloat>(opd.s, src_id, idx);
  case ODT_STRING:
    return make_all<VarString>(opd.s, src_id, idx);
  default:
    break;
  }
  return nullptr;
}

} // namespace consts
}
