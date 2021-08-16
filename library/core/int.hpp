

#ifndef LIBRARY_CORE_INT_HPP
#define LIBRARY_CORE_INT_HPP

#include "VM/VM.hpp"

#define ARITHI_FUNC(name)                                                      \
  VarBase *int_##name(VMState &vm, const FnData &fd) {                         \
    if (fd.args[1]->istype<VarInt>()) {                                        \
      VarInt *res = make<VarInt>(INT(fd.args[0])->get());                      \
      mpz_##name(res->get(), res->get(), INT(fd.args[1])->get());              \
      return res;                                                              \
    } else if (fd.args[1]->istype<VarFloat>()) {                               \
      VarInt *res = make<VarInt>(INT(fd.args[0])->get());                      \
      mpz_t tmp;                                                               \
      mpz_init(tmp);                                                           \
      mpfr_get_z(tmp, FLOAT(fd.args[1])->get(),                                \
                 mpfr_get_default_rounding_mode());                            \
      mpz_##name(res->get(), res->get(), tmp);                                 \
      mpz_clear(tmp);                                                          \
      return res;                                                              \
    }                                                                          \
    vm.fail(fd.src_id, fd.idx,                                                 \
            "expected int or float argument for int " STRINGIFY(               \
                name) ", found: %s",                                           \
            vm.type_name(fd.args[1]).c_str());                                 \
    return nullptr;                                                            \
  }

#define ARITHI_ASSN_FUNC(name)                                                 \
  VarBase *int_assn_##name(VMState &vm, const FnData &fd) {                    \
    if (fd.args[1]->istype<VarInt>()) {                                        \
      mpz_##name(INT(fd.args[0])->get(), INT(fd.args[0])->get(),               \
                 INT(fd.args[1])->get());                                      \
      return fd.args[0];                                                       \
    } else if (fd.args[1]->istype<VarFloat>()) {                               \
      mpz_t tmp;                                                               \
      mpz_init(tmp);                                                           \
      mpfr_get_z(tmp, FLOAT(fd.args[1])->get(),                                \
                 mpfr_get_default_rounding_mode());                            \
      mpz_##name(INT(fd.args[0])->get(), INT(fd.args[0])->get(), tmp);         \
      mpz_clear(tmp);                                                          \
      return fd.args[0];                                                       \
    }                                                                          \
    vm.fail(fd.src_id, fd.idx,                                                 \
            "expected int or float argument for int " STRINGIFY(               \
                name) "-assign, found: %s",                                    \
            vm.type_name(fd.args[1]).c_str());                                 \
    return nullptr;                                                            \
  }

#define LOGICI_FUNC(name, sym)                                                 \
  VarBase *int_##name(VMState &vm, const FnData &fd) {                         \
    if (fd.args[1]->istype<VarInt>()) {                                        \
      return mpz_cmp(INT(fd.args[0])->get(), INT(fd.args[1])->get()) sym 0     \
                 ? vm.tru                                                      \
                 : vm.fals;                                                    \
    }                                                                          \
    vm.fail(fd.src_id, fd.idx,                                                 \
            "expected int argument for int " STRINGIFY(name) ", found: %s",    \
            vm.type_name(fd.args[1]).c_str());                                 \
    return nullptr;                                                            \
  }

ARITHI_FUNC(add)
ARITHI_FUNC(sub)
ARITHI_FUNC(mul)
ARITHI_FUNC(mod)

ARITHI_ASSN_FUNC(add)
ARITHI_ASSN_FUNC(sub)
ARITHI_ASSN_FUNC(mul)
ARITHI_ASSN_FUNC(mod)

LOGICI_FUNC(lt, <)
LOGICI_FUNC(gt, >)
LOGICI_FUNC(le, <=)
LOGICI_FUNC(ge, >=)

VarBase *int_div(VMState &vm, const FnData &fd) {
  if (fd.args[1]->istype<VarInt>()) {
    // rhs == 0
    if (mpz_get_ui(INT(fd.args[1])->get()) == 0) {
      vm.fail(fd.src_id, fd.idx, "division by zero");
      return nullptr;
    }
    VarInt *res = make<VarInt>(INT(fd.args[0])->get());
    mpz_div(res->get(), res->get(), INT(fd.args[1])->get());
    return res;
  } else if (fd.args[1]->istype<VarFloat>()) {
    VarInt *res = make<VarInt>(INT(fd.args[0])->get());
    mpz_t tmp;
    mpz_init(tmp);
    mpfr_get_z(tmp, FLOAT(fd.args[1])->get(), mpfr_get_default_rounding_mode());
    // rhs == 0
    if (mpz_get_ui(tmp) == 0) {
      vm.fail(fd.src_id, fd.idx, "division by zero");
      return nullptr;
    }
    mpz_div(res->get(), res->get(), tmp);
    mpz_clear(tmp);
    return res;
  }
  vm.fail(
      fd.src_id, fd.idx,
      "expected int or float argument for int " STRINGIFY(name) ", found: %s",
      vm.type_name(fd.args[1]).c_str());
  return nullptr;
}

VarBase *int_assn_div(VMState &vm, const FnData &fd) {
  if (fd.args[1]->istype<VarInt>()) {
    // rhs == 0
    if (mpz_get_ui(INT(fd.args[1])->get()) == 0) {
      vm.fail(fd.src_id, fd.idx, "division by zero");
      return nullptr;
    }
    mpz_div(INT(fd.args[0])->get(), INT(fd.args[0])->get(),
            INT(fd.args[1])->get());
    return fd.args[0];
  } else if (fd.args[1]->istype<VarFloat>()) {
    mpz_t tmp;
    mpz_init(tmp);
    mpfr_get_z(tmp, FLOAT(fd.args[1])->get(), mpfr_get_default_rounding_mode());
    // rhs == 0
    if (mpz_get_ui(tmp) == 0) {
      vm.fail(fd.src_id, fd.idx, "division by zero");
      return nullptr;
    }
    mpz_div(INT(fd.args[0])->get(), INT(fd.args[0])->get(), tmp);
    mpz_clear(tmp);
    return fd.args[0];
  }
  vm.fail(fd.src_id, fd.idx,
          "expected int or float argument for int " STRINGIFY(
              name) "-assign, found: %s",
          vm.type_name(fd.args[1]).c_str());
  return nullptr;
}

VarBase *int_band(VMState &vm, const FnData &fd) {
  if (fd.args[1]->istype<VarInt>()) {
    VarInt *res = make<VarInt>(INT(fd.args[0])->get());
    mpz_and(res->get(), INT(fd.args[0])->get(), INT(fd.args[1])->get());
    return res;
  }
  vm.fail(fd.src_id, fd.idx,
          "expected int argument for int bitwise and, found: %s",
          vm.type_name(fd.args[1]).c_str());
  return nullptr;
}

VarBase *int_bor(VMState &vm, const FnData &fd) {
  if (fd.args[1]->istype<VarInt>()) {
    VarInt *res = make<VarInt>(INT(fd.args[0])->get());
    mpz_ior(res->get(), INT(fd.args[0])->get(), INT(fd.args[1])->get());
    return res;
  }
  vm.fail(fd.src_id, fd.idx,
          "expected int argument for int bitwise or, found: %s",
          vm.type_name(fd.args[1]).c_str());
  return nullptr;
}

VarBase *int_bxor(VMState &vm, const FnData &fd) {
  if (fd.args[1]->istype<VarInt>()) {
    VarInt *res = make<VarInt>(INT(fd.args[0])->get());
    mpz_xor(res->get(), INT(fd.args[0])->get(), INT(fd.args[1])->get());
    return res;
  }
  vm.fail(fd.src_id, fd.idx,
          "expected int argument for int bitwise xor, found: %s",
          vm.type_name(fd.args[1]).c_str());
  return nullptr;
}

VarBase *int_bnot(VMState &vm, const FnData &fd) {
  VarInt *res = make<VarInt>(INT(fd.args[0])->get());
  mpz_com(res->get(), INT(fd.args[0])->get());
  return res;
}

VarBase *int_bandassn(VMState &vm, const FnData &fd) {
  if (fd.args[1]->istype<VarInt>()) {
    mpz_and(INT(fd.args[0])->get(), INT(fd.args[0])->get(),
            INT(fd.args[1])->get());
    return fd.args[0];
  }
  vm.fail(fd.src_id, fd.idx,
          "expected int argument for int bitwise and-assn, found: %s",
          vm.type_name(fd.args[1]).c_str());
  return nullptr;
}

VarBase *int_borassn(VMState &vm, const FnData &fd) {
  if (fd.args[1]->istype<VarInt>()) {
    mpz_ior(INT(fd.args[0])->get(), INT(fd.args[0])->get(),
            INT(fd.args[1])->get());
    return fd.args[0];
  }
  vm.fail(fd.src_id, fd.idx,
          "expected int argument for int bitwise or-assn, found: %s",
          vm.type_name(fd.args[1]).c_str());
  return nullptr;
}

VarBase *int_bxorassn(VMState &vm, const FnData &fd) {
  if (fd.args[1]->istype<VarInt>()) {
    mpz_xor(INT(fd.args[0])->get(), INT(fd.args[0])->get(),
            INT(fd.args[1])->get());
    return fd.args[0];
  }
  vm.fail(fd.src_id, fd.idx,
          "expected int argument for int bitwise xor, found: %s",
          vm.type_name(fd.args[1]).c_str());
  return nullptr;
}

VarBase *int_bnotassn(VMState &vm, const FnData &fd) {
  mpz_com(INT(fd.args[0])->get(), INT(fd.args[0])->get());
  return fd.args[0];
}

VarBase *int_popcnt(VMState &vm, const FnData &fd) {
  return make<VarInt>(mpz_popcount(INT(fd.args[0])->get()));
}

VarBase *int_eq(VMState &vm, const FnData &fd) {
  if (fd.args[1]->istype<VarInt>()) {
    return mpz_cmp(INT(fd.args[0])->get(), INT(fd.args[1])->get()) == 0
               ? vm.tru
               : vm.fals;
  }
  return vm.fals;
}

VarBase *int_ne(VMState &vm, const FnData &fd) {
  if (fd.args[1]->istype<VarInt>()) {
    return mpz_cmp(INT(fd.args[0])->get(), INT(fd.args[1])->get()) != 0
               ? vm.tru
               : vm.fals;
  }
  return vm.tru;
}

VarBase *int_lshift(VMState &vm, const FnData &fd) {
  if (fd.args[1]->istype<VarInt>()) {
    VarInt *res = make<VarInt>(INT(fd.args[0])->get());
    mpz_mul_2exp(res->get(), INT(fd.args[0])->get(),
                 mpz_get_si(INT(fd.args[1])->get()));
    return res;
  } else if (fd.args[1]->istype<VarFloat>()) {
    VarInt *res = make<VarInt>(INT(fd.args[0])->get());
    mpz_t tmp;
    mpz_init(tmp);
    mpfr_get_z(tmp, FLOAT(fd.args[1])->get(), mpfr_get_default_rounding_mode());
    mpz_mul_2exp(res->get(), INT(fd.args[0])->get(), mpz_get_si(tmp));
    mpz_clear(tmp);
    return res;
  }
  vm.fail(fd.src_id, fd.idx,
          "expected int or float argument for int leftshift, found: %s",
          vm.type_name(fd.args[1]).c_str());
  return nullptr;
}

VarBase *int_rshift(VMState &vm, const FnData &fd) {
  if (fd.args[1]->istype<VarInt>()) {
    VarInt *res = make<VarInt>(INT(fd.args[0])->get());
    mpz_div_2exp(res->get(), INT(fd.args[0])->get(),
                 mpz_get_si(INT(fd.args[1])->get()));
    return res;
  } else if (fd.args[1]->istype<VarFloat>()) {
    VarInt *res = make<VarInt>(INT(fd.args[0])->get());
    mpz_t tmp;
    mpz_init(tmp);
    mpfr_get_z(tmp, FLOAT(fd.args[1])->get(), mpfr_get_default_rounding_mode());
    mpz_div_2exp(res->get(), INT(fd.args[0])->get(), mpz_get_si(tmp));
    mpz_clear(tmp);
    return res;
  }
  vm.fail(fd.src_id, fd.idx,
          "expected int or float argument for int rightshift, found: %s",
          vm.type_name(fd.args[1]).c_str());
  return nullptr;
}

VarBase *int_lshiftassn(VMState &vm, const FnData &fd) {
  if (fd.args[1]->istype<VarInt>()) {
    mpz_mul_2exp(INT(fd.args[0])->get(), INT(fd.args[0])->get(),
                 mpz_get_si(INT(fd.args[1])->get()));
    return fd.args[0];
  } else if (fd.args[1]->istype<VarFloat>()) {
    mpz_t tmp;
    mpz_init(tmp);
    mpfr_get_z(tmp, FLOAT(fd.args[1])->get(), mpfr_get_default_rounding_mode());
    mpz_mul_2exp(INT(fd.args[0])->get(), INT(fd.args[0])->get(),
                 mpz_get_si(tmp));
    mpz_clear(tmp);
    return fd.args[0];
  }
  vm.fail(fd.src_id, fd.idx,
          "expected int or float argument for int leftshift-assign, found: %s",
          vm.type_name(fd.args[1]).c_str());
  return nullptr;
}

VarBase *int_rshiftassn(VMState &vm, const FnData &fd) {
  if (fd.args[1]->istype<VarInt>()) {
    mpz_div_2exp(INT(fd.args[0])->get(), INT(fd.args[0])->get(),
                 mpz_get_si(INT(fd.args[1])->get()));
    return fd.args[0];
  } else if (fd.args[1]->istype<VarFloat>()) {
    mpz_t tmp;
    mpz_init(tmp);
    mpfr_get_z(tmp, FLOAT(fd.args[1])->get(), mpfr_get_default_rounding_mode());
    mpz_div_2exp(INT(fd.args[0])->get(), INT(fd.args[0])->get(),
                 mpz_get_si(tmp));
    mpz_clear(tmp);
    return fd.args[0];
  }
  vm.fail(fd.src_id, fd.idx,
          "expected int or float argument for int rightshift-assign, found: %s",
          vm.type_name(fd.args[1]).c_str());
  return nullptr;
}

VarBase *int_pow(VMState &vm, const FnData &fd) {
  if (fd.args[1]->istype<VarInt>()) {
    VarInt *res = make<VarInt>(INT(fd.args[0])->get());
    mpz_pow_ui(res->get(), res->get(), mpz_get_ui(INT(fd.args[1])->get()));
    return res;
  } else if (fd.args[1]->istype<VarFloat>()) {
    VarInt *res = make<VarInt>(INT(fd.args[0])->get());
    mpz_t tmp;
    mpz_init(tmp);
    mpfr_get_z(tmp, FLOAT(fd.args[1])->get(), mpfr_get_default_rounding_mode());
    mpz_pow_ui(res->get(), res->get(), mpz_get_ui(tmp));
    mpz_clear(tmp);
    return res;
  }
  vm.fail(fd.src_id, fd.idx,
          "expected int or float argument for int power, found: %s",
          vm.type_name(fd.args[1]).c_str());
  return nullptr;
}

VarBase *int_root(VMState &vm, const FnData &fd) {
  if (fd.args[1]->istype<VarInt>()) {
    VarInt *res = make<VarInt>(INT(fd.args[0])->get());
    mpz_root(res->get(), res->get(), mpz_get_ui(INT(fd.args[1])->get()));
    return res;
  } else if (fd.args[1]->istype<VarFloat>()) {
    VarInt *res = make<VarInt>(INT(fd.args[0])->get());
    mpz_t tmp;
    mpz_init(tmp);
    mpfr_get_z(tmp, FLOAT(fd.args[1])->get(), mpfr_get_default_rounding_mode());
    mpz_root(res->get(), res->get(), mpz_get_ui(tmp));
    mpz_clear(tmp);
    return res;
  }
  vm.fail(fd.src_id, fd.idx,
          "expected int or float argument for int root, found: %s",
          vm.type_name(fd.args[1]).c_str());
  return nullptr;
}

VarBase *int_preinc(VMState &vm, const FnData &fd) {
  mpz_add_ui(INT(fd.args[0])->get(), INT(fd.args[0])->get(), 1);
  return fd.args[0];
}

VarBase *int_postinc(VMState &vm, const FnData &fd) {
  VarInt *res = make<VarInt>(INT(fd.args[0])->get());
  mpz_add_ui(INT(fd.args[0])->get(), INT(fd.args[0])->get(), 1);
  return res;
}

VarBase *int_predec(VMState &vm, const FnData &fd) {
  mpz_sub_ui(INT(fd.args[0])->get(), INT(fd.args[0])->get(), 1);
  return fd.args[0];
}

VarBase *int_postdec(VMState &vm, const FnData &fd) {
  VarInt *res = make<VarInt>(INT(fd.args[0])->get());
  mpz_sub_ui(INT(fd.args[0])->get(), INT(fd.args[0])->get(), 1);
  return res;
}

VarBase *int_usub(VMState &vm, const FnData &fd) {
  VarInt *res = make<VarInt>(INT(fd.args[0])->get());
  mpz_neg(res->get(), res->get());
  return res;
}

// logical functions

#endif // LIBRARY_CORE_INT_HPP