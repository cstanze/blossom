

#include "VM/VM.hpp"
#include "std/ptr_type.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// Functions
///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VarBase* ptr_new_native(VMState& vm, const FnData& fd) {
  return make<VarPtr>(fd.args[1]);
}

VarBase* ptr_set(VMState& vm, const FnData& fd) {
  VarPtr* self = PTR(fd.args[0]);
  self->update(fd.args[1]);
  return fd.args[0];
}

VarBase* ptr_get(VMState& vm, const FnData& fd) {
  return PTR(fd.args[0])->get();
}

INIT_MODULE(ptr) {
  VarSrc* src = vm.current_source();
  src->add_native_fn("new_native", ptr_new_native, 1);

  vm.add_native_typefn<VarPtr>("set", ptr_set, 1, src_id, idx);
  vm.add_native_typefn<VarPtr>("get", ptr_get, 0, src_id, idx);
  return true;
}