

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
  char *_res;
  gmp_asprintf(&_res, "%Zd", INT(fd.args[0])->get());
  VarBase *res = make<VarString>(_res);

  return res;
}

VarBase *float_to_str(VMState &vm, const FnData &fd) {
  char *_res;
  std::string format = "%.2Rf";

  if (fd.args.size() == 2 && fd.args[1]->istype<VarString>()) {
    format = STR(fd.args[1])->get();
  }

  // mpfr_asprintf to &_res with any precision
  mpfr_asprintf(&_res, format.c_str(), FLOAT(fd.args[0])->get());
  VarString *res = make<VarString>(_res);
  
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