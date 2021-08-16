

#ifndef LIBRARY_CORE_NIL_HPP
#define LIBRARY_CORE_NIL_HPP

#include "VM/VM.hpp"

VarBase *nil_eq(VMState &vm, const FnData &fd) {
  return fd.args[1]->istype<VarNil>() ? vm.tru : vm.fals;
}

VarBase *nil_ne(VMState &vm, const FnData &fd) {
  return !fd.args[1]->istype<VarNil>() ? vm.tru : vm.fals;
}

#endif // LIBRARY_CORE_NIL_HPP