

#ifndef LIBRARY_CORE_TO_float_HPP
#define LIBRARY_CORE_TO_float_HPP

#include "VM/VM.hpp"

VarBase *nil_to_float(VMState &vm, const FnData &fd) {
  return make<VarFloat>(0.0);
}

VarBase *bool_to_float(VMState &vm, const FnData &fd) {
  return make<VarFloat>(BOOL(fd.args[0])->get() ? 1.0 : 0.0);
}

VarBase *int_to_float(VMState &vm, const FnData &fd) {
  return make<VarFloat>(INT(fd.args[0])->get());
}

VarBase *float_to_float(VMState &vm, const FnData &fd) { return fd.args[0]; }

VarBase *str_to_float(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(fd.src_id, fd.idx, "base must be an integer, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  VarFloat *res = make<VarFloat>(0.0);
  int tmp = mpfr_set_str(res->get(), STR(fd.args[0])->get().c_str(),
                         mpz_get_ui(INT(fd.args[1])->get()),
                         mpfr_get_default_rounding_mode());
  if (tmp == 0)
    return res;
  return vm.nil;
}

#endif // LIBRARY_CORE_TO_float_HPP