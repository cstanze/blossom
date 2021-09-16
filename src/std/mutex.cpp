#include "VM/VM.hpp"
#include "std/mutex_type.hpp"

int exec_command(const std::string &cmd);

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// Functions
///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VarBase *mutex_new(VMState &vm, const FnData &fd) { return make<VarMutex>(); }

VarBase *mutex_lock(VMState &vm, const FnData &fd) {
  MUTEX(fd.args[0])->get().lock();
  return vm.nil;
}

VarBase *mutex_unlock(VMState &vm, const FnData &fd) {
  MUTEX(fd.args[0])->get().unlock();
  return vm.nil;
}

INIT_MODULE(mutex) {
  VarSrc *src = vm.current_source();

  src->add_native_fn("new", mutex_new, 0);

  vm.add_native_typefn<VarMutex>("lock", mutex_lock, 0, src_id, idx);
  vm.add_native_typefn<VarMutex>("unlock", mutex_unlock, 0, src_id, idx);
  return true;
}
