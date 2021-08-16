

#ifndef LIBRARY_CORE_TYPEID_HPP
#define LIBRARY_CORE_TYPEID_HPP

#include "VM/VM.hpp"

// logical functions

VarBase *typeid_eq(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarTypeId>()) {
    return vm.fals;
  }
  return TYPEID(fd.args[0])->get() == TYPEID(fd.args[1])->get() ? vm.tru
                                                                : vm.fals;
}

VarBase *typeid_ne(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarTypeId>()) {
    return vm.tru;
  }
  return TYPEID(fd.args[0])->get() != TYPEID(fd.args[1])->get() ? vm.tru
                                                                : vm.fals;
}

#endif // LIBRARY_CORE_TYPEID_HPP