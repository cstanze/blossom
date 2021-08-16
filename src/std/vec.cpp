

#include "VM/VM.hpp"
#include "std/vec_type.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// Functions
///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VarBase *vec_new(VMState &vm, const FnData &fd) {
  size_t reserve_cap = fd.args.size() - 1;
  bool refs = false;
  if (fd.assn_args_loc.find("refs") != fd.assn_args_loc.end()) {
    VarBase *refs_var = fd.assn_args[fd.assn_args_loc.at("refs")].val;
    if (!refs_var->istype<VarBool>()) {
      vm.fail(
          fd.src_id, fd.idx,
          "expected 'refs' named argument to be of type bool for vec.new(), "
          "found: %s",
          vm.type_name(refs_var).c_str());
      return nullptr;
    }
    refs = BOOL(refs_var)->get();
  }
  if (fd.assn_args_loc.find("cap") != fd.assn_args_loc.end()) {
    VarBase *cap_var = fd.assn_args[fd.assn_args_loc.at("cap")].val;
    if (!cap_var->istype<VarInt>()) {
      vm.fail(fd.src_id, fd.idx,
              "expected 'cap' named argument to be of type int for vec.new(), "
              "found: %s",
              vm.type_name(cap_var).c_str());
      return nullptr;
    }
    reserve_cap = mpz_get_ui(INT(cap_var)->get());
  }
  VarVec *res = make<VarVec>(std::vector<VarBase *>{}, refs);
  std::vector<VarBase *> &vec_val = res->get();
  vec_val.reserve(reserve_cap);
  if (refs) {
    for (size_t i = 1; i < fd.args.size(); ++i) {
      var_iref(fd.args[i]);
      vec_val.push_back(fd.args[i]);
    }
  } else {
    for (size_t i = 1; i < fd.args.size(); ++i) {
      vec_val.push_back(fd.args[i]->copy(fd.src_id, fd.idx));
    }
  }
  return res;
}

VarBase *vec_size(VMState &vm, const FnData &fd) {
  return make<VarInt>(VEC(fd.args[0])->get().size());
}

VarBase *vec_cap(VMState &vm, const FnData &fd) {
  return make<VarInt>(VEC(fd.args[0])->get().capacity());
}

VarBase *vec_is_ref(VMState &vm, const FnData &fd) {
  return VEC(fd.args[0])->is_ref_vec() ? vm.tru : vm.fals;
}

VarBase *vec_empty(VMState &vm, const FnData &fd) {
  return VEC(fd.args[0])->get().size() == 0 ? vm.tru : vm.fals;
}

VarBase *vec_front(VMState &vm, const FnData &fd) {
  std::vector<VarBase *> &vec = VEC(fd.args[0])->get();
  return vec.size() == 0 ? vm.nil : vec.front();
}

VarBase *vec_back(VMState &vm, const FnData &fd) {
  std::vector<VarBase *> &vec = VEC(fd.args[0])->get();
  return vec.size() == 0 ? vm.nil : vec.back();
}

VarBase *vec_push(VMState &vm, const FnData &fd) {
  std::vector<VarBase *> &vec = VEC(fd.args[0])->get();
  if (VEC(fd.args[0])->is_ref_vec()) {
    var_iref(fd.args[1]);
    vec.push_back(fd.args[1]);
  } else {
    vec.push_back(fd.args[1]->copy(fd.src_id, fd.idx));
  }
  return fd.args[0];
}

VarBase *vec_pop(VMState &vm, const FnData &fd) {
  std::vector<VarBase *> &vec = VEC(fd.args[0])->get();
  if (vec.empty()) {
    vm.fail(fd.src_id, fd.idx, "performed pop() on an empty vector");
    return nullptr;
  }
  var_dref(vec.back());
  vec.pop_back();
  return fd.args[0];
}

VarBase *vec_setat(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected first argument to be of type integer for vec.set(), "
            "found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  size_t pos = mpz_get_ui(INT(fd.args[1])->get());
  std::vector<VarBase *> &vec = VEC(fd.args[0])->get();
  if (pos >= vec.size()) {
    vm.fail(fd.src_id, fd.idx,
            "position %zu is not within string of length %zu", pos, vec.size());
    return nullptr;
  }
  var_dref(vec[pos]);
  if (VEC(fd.args[0])->is_ref_vec()) {
    var_iref(fd.args[2]);
    vec[pos] = fd.args[2];
  } else {
    vec[pos] = fd.args[2]->copy(fd.src_id, fd.idx);
  }
  return fd.args[0];
}

VarBase *vec_insert(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected first argument to be of type integer for "
            "string.insert(), found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  size_t pos = mpz_get_ui(INT(fd.args[1])->get());
  std::vector<VarBase *> &vec = VEC(fd.args[0])->get();
  if (pos > vec.size()) {
    vm.fail(fd.src_id, fd.idx, "position %zu is greater than vector length %zu",
            pos, vec.size());
    return nullptr;
  }
  if (VEC(fd.args[0])->is_ref_vec()) {
    var_iref(fd.args[2]);
    vec.insert(vec.begin() + pos, fd.args[2]);
  } else {
    vec.insert(vec.begin() + pos, fd.args[2]->copy(fd.src_id, fd.idx));
  }
  return fd.args[0];
}

VarBase *vec_erase(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(
        fd.src_id, fd.idx,
        "expected argument to be of type integer for string.erase(), found: %s",
        vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  size_t pos = mpz_get_ui(INT(fd.args[1])->get());
  std::vector<VarBase *> &vec = VEC(fd.args[0])->get();
  if (pos >= vec.size()) {
    vm.fail(fd.src_id, fd.idx, "attempted erase on pos: %zu, vector size: %zu",
            pos, vec.size());
    return nullptr;
  }
  var_dref(vec[pos]);
  vec.erase(vec.begin() + pos);
  return fd.args[0];
}

VarBase *vec_last(VMState &vm, const FnData &fd) {
  return make<VarInt>(VEC(fd.args[0])->get().size() - 1);
}

VarBase *vec_at(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(
        fd.src_id, fd.idx,
        "expected argument to be of type integer for string.erase(), found: %s",
        vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  std::vector<VarBase *> &vec = VEC(fd.args[0])->get();
  size_t pos = mpz_get_ui(INT(fd.args[1])->get());
  if (pos >= vec.size())
    return vm.nil;
  return vec[pos];
}

VarBase *vec_each(VMState &vm, const FnData &fd) {
  return make<VarVecIterable>(VEC(fd.args[0]));
}

VarBase *vec_iterable_next(VMState &vm, const FnData &fd) {
  VarVecIterable *it = VEC_ITERABLE(fd.args[0]);
  VarBase *res = nullptr;
  if (!it->next(res))
    return vm.nil;
  res->set_load_as_ref();
  return res;
}

VarBase *vec_sub(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(
        fd.src_id, fd.idx,
        "expected starting index to be of type 'int' for vec.sub(), found: %s",
        vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  if (!fd.args[2]->istype<VarInt>()) {
    vm.fail(
        fd.src_id, fd.idx,
        "expected ending index to be of type 'int' for vec.sub(), found: %s",
        vm.type_name(fd.args[2]).c_str());
    return nullptr;
  }

  std::vector<VarBase *> &vec = VEC(fd.args[0])->get();
  size_t begin = mpz_get_ui(INT(fd.args[1])->get());
  size_t end = mpz_get_ui(INT(fd.args[2])->get());

  std::vector<VarBase *> newvec;
  if (end > begin)
    newvec.reserve(end - begin);
  for (size_t i = begin; i < end; ++i) {
    newvec.push_back(vec[i]->copy(fd.src_id, fd.idx));
  }
  return make<VarVec>(newvec, true);
}

VarBase *vec_slice(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected starting index to be of type 'int' for vec.slice(), "
            "found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  if (!fd.args[2]->istype<VarInt>()) {
    vm.fail(
        fd.src_id, fd.idx,
        "expected ending index to be of type 'int' for vec.slice(), found: %s",
        vm.type_name(fd.args[2]).c_str());
    return nullptr;
  }

  std::vector<VarBase *> &vec = VEC(fd.args[0])->get();
  size_t begin = mpz_get_ui(INT(fd.args[1])->get());
  size_t end = mpz_get_ui(INT(fd.args[2])->get());

  std::vector<VarBase *> newvec;
  if (end > begin)
    newvec.reserve(end - begin);
  for (size_t i = begin; i < end; ++i) {
    var_iref(vec[i]);
    newvec.push_back(vec[i]);
  }
  return make<VarVec>(newvec, true);
}

INIT_MODULE(vec) {
  VarSrc *src = vm.current_source();

  src->add_native_fn("new", vec_new, 0, true);

  vm.add_native_typefn<VarVec>("len", vec_size, 0, src_id, idx);
  vm.add_native_typefn<VarVec>("cap", vec_cap, 0, src_id, idx);
  vm.add_native_typefn<VarVec>("is_ref", vec_is_ref, 0, src_id, idx);
  vm.add_native_typefn<VarVec>("empty", vec_empty, 0, src_id, idx);
  vm.add_native_typefn<VarVec>("front", vec_front, 0, src_id, idx);
  vm.add_native_typefn<VarVec>("back", vec_back, 0, src_id, idx);
  vm.add_native_typefn<VarVec>("push", vec_push, 1, src_id, idx);
  vm.add_native_typefn<VarVec>("pop", vec_pop, 0, src_id, idx);
  vm.add_native_typefn<VarVec>("insert", vec_insert, 2, src_id, idx);
  vm.add_native_typefn<VarVec>("erase", vec_erase, 1, src_id, idx);
  vm.add_native_typefn<VarVec>("lastidx", vec_last, 0, src_id, idx);
  vm.add_native_typefn<VarVec>("set", vec_setat, 2, src_id, idx);
  vm.add_native_typefn<VarVec>("at", vec_at, 1, src_id, idx);
  vm.add_native_typefn<VarVec>("[]", vec_at, 1, src_id, idx);
  vm.add_native_typefn<VarVec>("each", vec_each, 0, src_id, idx);

  vm.add_native_typefn<VarVec>("sub_native", vec_sub, 2, src_id, idx);
  vm.add_native_typefn<VarVec>("slice_native", vec_slice, 2, src_id, idx);

  vm.add_native_typefn<VarVecIterable>("next", vec_iterable_next, 0, src_id,
                                       idx);

  return true;
}
