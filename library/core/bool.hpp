

#ifndef LIBRARY_CORE_BOOL_HPP
#define LIBRARY_CORE_BOOL_HPP

#include "VM/VM.hpp"

// logical functions

VarBase *bool_lt(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarBool>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected boolean argument for logical less than, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  return BOOL(fd.args[0])->get() < BOOL(fd.args[1])->get() ? vm.tru : vm.fals;
}

VarBase *bool_gt(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarBool>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected boolean argument for logical greater than, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  return BOOL(fd.args[0])->get() > BOOL(fd.args[1])->get() ? vm.tru : vm.fals;
}

VarBase *bool_le(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarBool>()) {
    vm.fail(
        fd.src_id, fd.idx,
        "expected boolean argument for logical less than or equal, found: %s",
        vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  return BOOL(fd.args[0])->get() <= BOOL(fd.args[1])->get() ? vm.tru : vm.fals;
}

VarBase *bool_ge(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarBool>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected boolean argument for logical greater than or equal, "
            "found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  return BOOL(fd.args[0])->get() >= BOOL(fd.args[1])->get() ? vm.tru : vm.fals;
}

VarBase *bool_eq(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarBool>()) {
    return vm.fals;
  }
  return BOOL(fd.args[0])->get() == BOOL(fd.args[1])->get() ? vm.tru : vm.fals;
}

VarBase *bool_ne(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarBool>()) {
    return vm.tru;
  }
  return BOOL(fd.args[0])->get() != BOOL(fd.args[1])->get() ? vm.tru : vm.fals;
}

VarBase *bool_not(VMState &vm, const FnData &fd) {
  return BOOL(fd.args[0])->get() ? vm.fals : vm.tru;
}

#endif // LIBRARY_CORE_BOOL_HPP