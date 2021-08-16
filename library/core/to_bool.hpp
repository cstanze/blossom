

#ifndef LIBRARY_CORE_TO_BOOL_HPP
#define LIBRARY_CORE_TO_BOOL_HPP

#include "VM/VM.hpp"

VarBase *all_to_bool(VMState &vm, const FnData &fd) {
  return make<VarBool>(false);
}

VarBase *nil_to_bool(VMState &vm, const FnData &fd) {
  return make<VarBool>(false);
}

VarBase *bool_to_bool(VMState &vm, const FnData &fd) { return fd.args[0]; }

VarBase *typeid_to_bool(VMState &vm, const FnData &fd) {
  return make<VarBool>(true);
}

VarBase *int_to_bool(VMState &vm, const FnData &fd) {
  return make<VarBool>(mpz_cmp_si(INT(fd.args[0])->get(), 0));
}

VarBase *float_to_bool(VMState &vm, const FnData &fd) {
  return make<VarBool>(mpfr_cmp_si(FLOAT(fd.args[0])->get(), 0));
}

VarBase *str_to_bool(VMState &vm, const FnData &fd) {
  return make<VarBool>(!STR(fd.args[0])->get().empty());
}

VarBase *vec_to_bool(VMState &vm, const FnData &fd) {
  return make<VarBool>(!VEC(fd.args[0])->get().empty());
}

VarBase *map_to_bool(VMState &vm, const FnData &fd) {
  return make<VarBool>(!MAP(fd.args[0])->get().empty());
}

#endif // LIBRARY_CORE_TO_BOOL_HPP