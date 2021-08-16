

#include "VM/VM.hpp"
#include "std/map_type.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// Functions
///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VarBase* map_new(VMState& vm, const FnData& fd) {
  if ((fd.args.size() - 1) % 2 != 0) {
    vm.fail(fd.src_id, fd.idx, "argument count must be even to create a map");
    return nullptr;
  }
  bool refs = false;
  if (fd.assn_args_loc.find("refs") != fd.assn_args_loc.end()) {
    VarBase* refs_var = fd.assn_args[fd.assn_args_loc.at("refs")].val;
    if (!refs_var->istype<VarBool>()) {
      vm.fail(
          fd.src_id, fd.idx,
          "expected 'refs' named argument to be of type bool for map.new(), "
          "found: %s",
          vm.type_name(refs_var).c_str());
      return nullptr;
    }
    refs = BOOL(refs_var)->get();
  }
  std::map<std::string, VarBase*> map_val;
  for (size_t i = 1; i < fd.args.size(); ++i) {
    std::string key;
    if (!fd.args[i]->to_str(vm, key, fd.src_id, fd.idx)) {
      return nullptr;
    }
    if (map_val.find(key) != map_val.end())
      var_dref(map_val[key]);
    if (refs) {
      var_iref(fd.args[++i]);
      map_val[key] = fd.args[i];
    } else {
      map_val[key] = fd.args[++i]->copy(fd.src_id, fd.idx);
    }
  }
  return make<VarMap>(map_val, refs);
}

VarBase* map_len(VMState& vm, const FnData& fd) {
  return make<VarInt>(MAP(fd.args[0])->get().size());
}

VarBase* map_is_ref(VMState& vm, const FnData& fd) {
  return MAP(fd.args[0])->is_ref_map() ? vm.tru : vm.fals;
}

VarBase* map_empty(VMState& vm, const FnData& fd) {
  return MAP(fd.args[0])->get().empty() ? vm.tru : vm.fals;
}

VarBase* map_insert(VMState& vm, const FnData& fd) {
  std::map<std::string, VarBase*>& map = MAP(fd.args[0])->get();
  std::string key;
  if (!fd.args[1]->to_str(vm, key, fd.src_id, fd.idx)) {
    return nullptr;
  }
  if (map.find(key) != map.end()) {
    var_dref(map[key]);
  }
  if (MAP(fd.args[0])->is_ref_map()) {
    var_iref(fd.args[2]);
    map[key] = fd.args[2];
  } else {
    map[key] = fd.args[2]->copy(fd.src_id, fd.idx);
  }
  return fd.args[0];
}

VarBase* map_erase(VMState& vm, const FnData& fd) {
  std::map<std::string, VarBase*>& map = MAP(fd.args[0])->get();
  std::string key;
  if (!fd.args[1]->to_str(vm, key, fd.src_id, fd.idx)) {
    return nullptr;
  }
  auto key_it = map.find(key);
  if (key_it != map.end()) {
    var_dref(key_it->second);
    map.erase(key_it);
  }
  return fd.args[0];
}

VarBase* map_get(VMState& vm, const FnData& fd) {
  std::map<std::string, VarBase*>& map = MAP(fd.args[0])->get();
  std::string key;
  if (!fd.args[1]->to_str(vm, key, fd.src_id, fd.idx)) {
    return nullptr;
  }
  if (map.find(key) == map.end()) {
    return vm.nil;
  }
  return map[key];
}

VarBase* map_find(VMState& vm, const FnData& fd) {
  std::map<std::string, VarBase*>& map = MAP(fd.args[0])->get();
  std::string key;
  if (!fd.args[1]->to_str(vm, key, fd.src_id, fd.idx)) {
    return nullptr;
  }
  return map.find(key) != map.end() ? vm.tru : vm.fals;
}

VarBase* map_each(VMState& vm, const FnData& fd) {
  return make<VarMapIterable>(MAP(fd.args[0]));
}

VarBase* map_iterable_next(VMState& vm, const FnData& fd) {
  VarMapIterable* it = MAP_ITERABLE(fd.args[0]);
  VarBase* res = nullptr;
  if (!it->next(res, fd.src_id, fd.idx))
    return vm.nil;
  res->set_load_as_ref();
  return res;
}

INIT_MODULE(map) {
  VarSrc* src = vm.current_source();

  src->add_native_fn("new", map_new, 0, true);

  vm.add_native_typefn<VarMap>("len", map_len, 0, src_id, idx);
  vm.add_native_typefn<VarMap>("is_ref", map_is_ref, 0, src_id, idx);
  vm.add_native_typefn<VarMap>("empty", map_empty, 0, src_id, idx);
  vm.add_native_typefn<VarMap>("insert", map_insert, 2, src_id, idx);
  vm.add_native_typefn<VarMap>("erase", map_erase, 1, src_id, idx);
  vm.add_native_typefn<VarMap>("get", map_get, 1, src_id, idx);
  vm.add_native_typefn<VarMap>("[]", map_get, 1, src_id, idx);
  vm.add_native_typefn<VarMap>("find", map_find, 1, src_id, idx);
  vm.add_native_typefn<VarMap>("each", map_each, 0, src_id, idx);

  vm.add_native_typefn<VarMapIterable>("next", map_iterable_next, 0, src_id,
                                       idx);

  return true;
}
