

#ifndef LIBRARY_CORE_TO_INT_HPP
#define LIBRARY_CORE_TO_INT_HPP

#include "VM/VM.hpp"

VarBase *nil_to_int(VMState &vm, const FnData &fd) { return make<VarInt>(0); }

VarBase *bool_to_int(VMState &vm, const FnData &fd) {
  return make<VarInt>(BOOL(fd.args[0])->get() ? 1 : 0);
}

VarBase *typeid_to_int(VMState &vm, const FnData &fd) {
  return make<VarInt>(TYPEID(fd.args[0])->get());
}

VarBase *int_to_int(VMState &vm, const FnData &fd) { return fd.args[0]; }

VarBase *float_to_int(VMState &vm, const FnData &fd) {
  VarInt *res = make<VarInt>(0);
  mpfr_get_z(res->get(), FLOAT(fd.args[0])->get(),
             mpfr_get_default_rounding_mode());
  return res;
}

VarBase *str_to_int(VMState &vm, const FnData &fd) {
  VarInt *res = make<VarInt>(0);
  int tmp = mpz_set_str(res->get(), STR(fd.args[0])->get().c_str(), 0);
  if (tmp == 0)
    return res;
  return vm.nil;
}

#endif // LIBRARY_CORE_TO_INT_HPP