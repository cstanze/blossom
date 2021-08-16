

#ifndef LIBRARY_CORE_TO_STR_HPP
#define LIBRARY_CORE_TO_STR_HPP

#include <cstring>

#include "VM/VM.hpp"

VarBase *all_to_str(VMState &vm, const FnData &fd) {
  VarBase *_data = fd.args[0];
  char res[1024];
  sprintf(res, "type: %s at %p", vm.type_name(_data).c_str(), _data);
  return make<VarString>(res);
}

VarBase *nil_to_str(VMState &vm, const FnData &fd) {
  return make<VarString>("(nil)");
}

VarBase *bool_to_str(VMState &vm, const FnData &fd) {
  return make<VarString>(BOOL(fd.args[0])->get() ? "true" : "false");
}

VarBase *typeid_to_str(VMState &vm, const FnData &fd) {
  return make<VarString>("typeid<" + std::to_string(TYPEID(fd.args[0])->get()) +
                         ">");
}

VarBase *int_to_str(VMState &vm, const FnData &fd) {
  typedef void (*gmp_freefunc_t)(void *, size_t);

  char *_res = mpz_get_str(NULL, 10, INT(fd.args[0])->get());
  VarString *res = make<VarString>("");
  res->get() = _res;

  gmp_freefunc_t freefunc;
  mp_get_memory_functions(NULL, NULL, &freefunc);
  freefunc(_res, strlen(_res) + 1);

  return res;
}

VarBase *float_to_str(VMState &vm, const FnData &fd) {
  mpfr_exp_t expo;
  char *_res = mpfr_get_str(NULL, &expo, 10, 0, FLOAT(fd.args[0])->get(),
                            mpfr_get_default_rounding_mode());
  VarString *res = make<VarString>(_res);
  mpfr_free_str(_res);
  if (res->get().empty() || expo == 0 || expo > 25)
    return res;
  auto last_zero_from_end = res->get().find_last_of("123456789");
  if (last_zero_from_end != std::string::npos)
    res->get() = res->get().erase(last_zero_from_end + 1);
  if (expo > 0) {
    std::string &str_res = res->get();
    size_t sz = str_res.size();
    while (expo > sz) {
      str_res += '0';
    }
    if (res->get()[0] == '-')
      ++expo;
    res->get().insert(expo, ".");
  } else {
    std::string pre_zero(-expo, '0');
    res->get() = "0." + pre_zero + res->get();
  }
  return res;
}

VarBase *str_to_str(VMState &vm, const FnData &fd) { return fd.args[0]; }

VarBase *vec_to_str(VMState &vm, const FnData &fd) {
  VarVec *vec = VEC(fd.args[0]);
  std::string res = "[";
  for (auto &e : vec->get()) {
    std::string str;
    if (!e->to_str(vm, str, fd.src_id, fd.idx)) {
      return nullptr;
    }
    res += str + ", ";
  }
  if (vec->get().size() > 0) {
    res.pop_back();
    res.pop_back();
  }
  res += "]";
  return make<VarString>(res);
}

VarBase *map_to_str(VMState &vm, const FnData &fd) {
  VarMap *map = MAP(fd.args[0]);
  std::string res = "{";
  for (auto &e : map->get()) {
    std::string str;
    if (!e.second->to_str(vm, str, fd.src_id, fd.idx)) {
      return nullptr;
    }
    res += e.first + ": " + str + ", ";
  }
  if (map->get().size() > 0) {
    res.pop_back();
    res.pop_back();
  }
  res += "}";
  return make<VarString>(res);
}

#endif // LIBRARY_CORE_TO_STR_HPP