

#include "VM/VM.hpp"
#include "std/struct_type.hpp"

static uintptr_t gen_struct_enum_id() {
  static uintptr_t id = 0;
  return id++;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// Functions
///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VarBase* create_struct(VMState& vm, const FnData& fd) {
  const size_t src_id = vm.current_source()->src_id();
  std::vector<std::string> attr_order;
  std::unordered_map<std::string, VarBase*> attrs;
  for (size_t i = 0; i < fd.assn_args.size(); ++i) {
    auto& arg = fd.assn_args[i];
    attr_order.push_back(arg.name);
    var_iref(arg.val);
    attrs[arg.name] = arg.val;
  }
  return make<VarStructDef>(gen_struct_enum_id(), attr_order, attrs);
}

VarBase* create_enum(VMState& vm, const FnData& fd) {
  std::unordered_map<std::string, VarBase*> attrs;

  for (size_t i = 1; i < fd.args.size(); ++i) {
    auto& arg = fd.args[i];
    if (!arg->istype<VarString>()) {
      vm.fail(arg->src_id(), arg->idx(),
              "expected const strings for enums (use strings or atoms)");
      goto fail;
    }
    attrs[STR(arg)->get()] = new VarInt(i - 1, fd.src_id, fd.idx);
  }

  for (auto& arg : fd.assn_args) {
    if (!arg.val->istype<VarInt>()) {
      vm.fail(arg.src_id, arg.idx,
              "expected argument value to be of integer for enums, found: %s",
              vm.type_name(arg.val).c_str());
      goto fail;
    }
    if (attrs.find(arg.name) != attrs.end()) {
      var_dref(attrs[arg.name]);
    }
    attrs[arg.name] = arg.val->copy(fd.src_id, fd.idx);
  }

  return make<VarStruct>(gen_struct_enum_id(), attrs, nullptr);
fail:
  for (auto& attr : attrs) {
    var_dref(attr.second);
  }
  return nullptr;
}

VarBase* struct_to_str(VMState& vm, const FnData& fd) {
  VarStruct* data = STRUCT(fd.args[0]);
  std::string res = vm.type_name(data->typefn_id()) + "{";
  for (auto& e : data->attrs()) {
    std::string str;
    if (!e.second->to_str(vm, str, fd.src_id, fd.idx)) {
      return nullptr;
    }
    res += e.first + ": " + str + ", ";
  }
  if (data->attrs().size() > 0) {
    res.pop_back();
    res.pop_back();
  }
  res += "}";
  return make<VarString>(res);
}

VarBase* struct_def_set_typename(VMState& vm, const FnData& fd) {
  if (!fd.args[1]->istype<VarString>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected argument to be of type string, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  vm.set_typename(STRUCT_DEF(fd.args[0])->typefn_id(), STR(fd.args[1])->get());
  return fd.args[0];
}

VarBase* struct_def_get_fields(VMState& vm, const FnData& fd) {
  std::vector<VarBase*> vec;
  const std::unordered_map<std::string, VarBase*>& attrs =
      STRUCT_DEF(fd.args[0])->attrs();
  for (auto& attr : attrs) {
    vec.push_back(new VarString(attr.first, fd.src_id, fd.idx));
  }
  return make<VarVec>(vec, false);
}

VarBase* struct_def_get_field_value(VMState& vm, const FnData& fd) {
  if (!fd.args[1]->istype<VarString>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected field name to be of type string, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  const std::string& attr = STR(fd.args[1])->get();
  const std::unordered_map<std::string, VarBase*>& attrs =
      STRUCT_DEF(fd.args[0])->attrs();

  if (attrs.find(attr) == attrs.end())
    return vm.nil;

  return attrs.at(attr);
}

VarBase* struct_get_fields(VMState& vm, const FnData& fd) {
  std::vector<VarBase*> vec;
  const std::unordered_map<std::string, VarBase*>& attrs =
      STRUCT(fd.args[0])->attrs();
  for (auto& attr : attrs) {
    vec.push_back(new VarString(attr.first, fd.src_id, fd.idx));
  }
  return make<VarVec>(vec, false);
}

VarBase* struct_set_field_value(VMState& vm, const FnData& fd) {
  if (!fd.args[1]->istype<VarString>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected field name to be of type string, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  VarBase* val = fd.args[2];
  const std::string& attr = STR(fd.args[1])->get();
  const std::unordered_map<std::string, VarBase*>& attrs =
      STRUCT(fd.args[0])->attrs();

  const auto& res = attrs.find(attr);
  if (res == attrs.end()) {
    vm.fail(fd.src_id, fd.idx, "field name '%s' not found", attr.c_str());
    return nullptr;
  }

  if (res->second->type() == val->type()) {
    res->second->set(val);
  } else {
    vm.fail(fd.src_id, fd.idx,
            "attribute value type mismatch, provided '%s', existing '%s'",
            vm.type_name(val).c_str(), vm.type_name(res->second).c_str());
    return nullptr;
  }
  return vm.nil;
}

INIT_MODULE(lang) {
  VarSrc* src = vm.current_source();
  src->add_native_fn("enum", create_enum, 0, true);
  src->add_native_fn("struct", create_struct);

  vm.add_native_typefn<VarStruct>("str", struct_to_str, 0, src_id, idx);

  vm.add_native_typefn<VarStructDef>("set_typename", struct_def_set_typename, 1,
                                     src_id, idx);
  vm.add_native_typefn<VarStructDef>("get_fields", struct_def_get_fields, 0,
                                     src_id, idx);
  vm.add_native_typefn<VarStructDef>("[]", struct_def_get_field_value, 1,
                                     src_id, idx);

  vm.add_native_typefn<VarStruct>("get_fields", struct_get_fields, 0, src_id,
                                  idx);
  vm.add_native_typefn<VarStruct>("set_field", struct_set_field_value, 2,
                                  src_id, idx);

  return true;
}
