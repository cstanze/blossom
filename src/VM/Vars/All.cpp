#include "VM/VM.hpp"
#include "VM/Vars/Base.hpp"

VarAll::VarAll(const size_t &src_id, const size_t &idx)
    : VarBase(type_id<VarAll>(), src_id, idx, false, false) {}

VarBase *VarAll::copy(const size_t &src_id, const size_t &idx) {
  return new VarAll(src_id, idx);
}

void VarAll::set(VarBase *from) {}
