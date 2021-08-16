

#ifndef LIBRARY_CORE_float_HPP
#define LIBRARY_CORE_float_HPP

#include "VM/VM.hpp"

#define ARITHF_FUNC(name)                                                      \
  VarBase *float_##name(VMState &vm, const FnData &fd) {                       \
    if (fd.args[1]->istype<VarInt>()) {                                        \
      VarFloat *res = make<VarFloat>(FLOAT(fd.args[0])->get());                \
      mpfr_t tmp;                                                              \
      mpfr_init_set_z(tmp, INT(fd.args[1])->get(),                             \
                      mpfr_get_default_rounding_mode());                       \
      mpfr_##name(res->get(), res->get(), tmp,                                 \
                  mpfr_get_default_rounding_mode());                           \
      mpfr_clear(tmp);                                                         \
      return res;                                                              \
    } else if (fd.args[1]->istype<VarFloat>()) {                               \
      VarFloat *res = make<VarFloat>(FLOAT(fd.args[0])->get());                \
      mpfr_##name(res->get(), res->get(), FLOAT(fd.args[1])->get(),            \
                  mpfr_get_default_rounding_mode());                           \
      return res;                                                              \
    }                                                                          \
    vm.fail(fd.src_id, fd.idx,                                                 \
            "expected int or float argument for float " STRINGIFY(             \
                name) ", found: %s",                                           \
            vm.type_name(fd.args[1]).c_str());                                 \
    return nullptr;                                                            \
  }

#define ARITHF_ASSN_FUNC(name)                                                 \
  VarBase *float_assn_##name(VMState &vm, const FnData &fd) {                  \
    if (fd.args[1]->istype<VarInt>()) {                                        \
      mpfr_t tmp;                                                              \
      mpfr_init_set_z(tmp, INT(fd.args[1])->get(),                             \
                      mpfr_get_default_rounding_mode());                       \
      mpfr_##name(FLOAT(fd.args[0])->get(), FLOAT(fd.args[0])->get(), tmp,     \
                  mpfr_get_default_rounding_mode());                           \
      mpfr_clear(tmp);                                                         \
      return fd.args[0];                                                       \
    } else if (fd.args[1]->istype<VarFloat>()) {                               \
      mpfr_##name(FLOAT(fd.args[0])->get(), FLOAT(fd.args[0])->get(),          \
                  FLOAT(fd.args[1])->get(), mpfr_get_default_rounding_mode()); \
      return fd.args[0];                                                       \
    }                                                                          \
    vm.fail(fd.src_id, fd.idx,                                                 \
            "expected int or float argument for float " STRINGIFY(             \
                name) "-assign, found: %s",                                    \
            vm.type_name(fd.args[1]).c_str());                                 \
    return nullptr;                                                            \
  }

#define LOGICF_FUNC(name, sym)                                                 \
  VarBase *float_##name(VMState &vm, const FnData &fd) {                       \
    if (fd.args[1]->istype<VarFloat>()) {                                      \
      return mpfr_cmp(FLOAT(fd.args[0])->get(), FLOAT(fd.args[1])->get())      \
                     sym 0                                                     \
                 ? vm.tru                                                      \
                 : vm.fals;                                                    \
    }                                                                          \
    vm.fail(                                                                   \
        fd.src_id, fd.idx,                                                     \
        "expected float argument for float " STRINGIFY(name) ", found: %s",    \
        vm.type_name(fd.args[1]).c_str());                                     \
    return nullptr;                                                            \
  }

ARITHF_FUNC(add)
ARITHF_FUNC(sub)
ARITHF_FUNC(mul)
ARITHF_FUNC(div)

ARITHF_ASSN_FUNC(add)
ARITHF_ASSN_FUNC(sub)
ARITHF_ASSN_FUNC(mul)
ARITHF_ASSN_FUNC(div)

LOGICF_FUNC(lt, <)
LOGICF_FUNC(gt, >)
LOGICF_FUNC(le, <=)
LOGICF_FUNC(ge, >=)

VarBase *float_eq(VMState &vm, const FnData &fd) {
  if (fd.args[1]->istype<VarFloat>()) {
    return mpfr_cmp(FLOAT(fd.args[0])->get(), FLOAT(fd.args[1])->get()) == 0
               ? vm.tru
               : vm.fals;
  }
  return vm.fals;
}

VarBase *float_ne(VMState &vm, const FnData &fd) {
  if (fd.args[1]->istype<VarFloat>()) {
    return mpfr_cmp(FLOAT(fd.args[0])->get(), FLOAT(fd.args[1])->get()) != 0
               ? vm.tru
               : vm.fals;
  }
  return vm.tru;
}

VarBase *float_preinc(VMState &vm, const FnData &fd) {
  mpfr_add_ui(FLOAT(fd.args[0])->get(), FLOAT(fd.args[0])->get(), 1,
              mpfr_get_default_rounding_mode());
  return fd.args[0];
}

VarBase *float_postinc(VMState &vm, const FnData &fd) {
  VarFloat *res = make<VarFloat>(FLOAT(fd.args[0])->get());
  mpfr_add_ui(FLOAT(fd.args[0])->get(), FLOAT(fd.args[0])->get(), 1,
              mpfr_get_default_rounding_mode());
  return res;
}

VarBase *float_predec(VMState &vm, const FnData &fd) {
  mpfr_sub_ui(FLOAT(fd.args[0])->get(), FLOAT(fd.args[0])->get(), 1,
              mpfr_get_default_rounding_mode());
  return fd.args[0];
}

VarBase *float_postdec(VMState &vm, const FnData &fd) {
  VarFloat *res = make<VarFloat>(FLOAT(fd.args[0])->get());
  mpfr_sub_ui(FLOAT(fd.args[0])->get(), FLOAT(fd.args[0])->get(), 1,
              mpfr_get_default_rounding_mode());
  return res;
}

VarBase *float_usub(VMState &vm, const FnData &fd) {
  VarFloat *res = make<VarFloat>(FLOAT(fd.args[0])->get());
  mpfr_neg(res->get(), FLOAT(fd.args[0])->get(),
           mpfr_get_default_rounding_mode());
  return res;
}

VarBase *float_round(VMState &vm, const FnData &fd) {
  VarInt *res = make<VarInt>(0);
  mpfr_get_z(res->get(), FLOAT(fd.args[0])->get(), MPFR_RNDN);
  return res;
}

VarBase *float_pow(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(fd.src_id, fd.idx, "power must be an integer, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  VarFloat *res = make<VarFloat>(FLOAT(fd.args[0])->get());
  mpfr_pow_si(res->get(), FLOAT(fd.args[0])->get(),
              mpz_get_si(INT(fd.args[1])->get()), MPFR_RNDN);
  return res;
}

VarBase *float_root(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(fd.src_id, fd.idx, "root must be an integer, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  VarFloat *res = make<VarFloat>(FLOAT(fd.args[0])->get());
#if MPFR_VERSION_MAJOR >= 4
  mpfr_rootn_ui(res->get(), FLOAT(fd.args[0])->get(),
                mpz_get_ui(INT(fd.args[1])->get()), MPFR_RNDN);
#else
  mpfr_root(res->get(), FLOAT(fd.args[0])->get(),
            mpz_get_ui(INT(fd.args[1])->get()), MPFR_RNDN);
#endif // MPFR_VERSION_MAJOR
  return res;
}

#endif // LIBRARY_CORE_float_HPP