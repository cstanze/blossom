#include "VM/VM.hpp"
#include "core/bool.hpp"
#include "core/float.hpp"
#include "core/int.hpp"
#include "core/nil.hpp"
#include "core/str.hpp"
#include "core/to_bool.hpp"
#include "core/to_float.hpp"
#include "core/to_int.hpp"
#include "core/to_str.hpp"
#include "core/typeid.hpp"

VarBase *all_get_type(VMState &vm, const FnData &fd) {
  return make<VarTypeId>(fd.args[0]->type());
}

VarBase *all_get_typefid(VMState &vm, const FnData &fd) {
  return make<VarTypeId>(fd.args[0]->typefn_id());
}

VarBase *all_get_typestr(VMState &vm, const FnData &fd) {
  return make<VarString>(vm.type_name(fd.args[0]));
}

VarBase *all_eq(VMState &vm, const FnData &fd) {
  return fd.args[0]->type() == fd.args[1]->type() ? vm.tru : vm.fals;
}

VarBase *all_ne(VMState &vm, const FnData &fd) {
  return fd.args[0]->type() != fd.args[1]->type() ? vm.tru : vm.fals;
}

VarBase *all_copy(VMState &vm, const FnData &fd) {
  VarBase *copy = fd.args[0]->copy(fd.src_id, fd.idx);
  // decreased because system internally will increment it again
  copy->dref();
  return copy;
}

VarBase *reference(VMState &vm, const FnData &fd) {
  fd.args[1]->set_load_as_ref();
  return fd.args[1];
}

VarBase *raise(VMState &vm, const FnData &fd) {
  std::string buf;
  if (fd.args[1]->to_str(vm, buf, fd.src_id, fd.idx)) {
    vm.fail(fd.src_id, fd.idx, buf.c_str());
  } else {
    vm.fail(fd.src_id, fd.idx, "raised error");
  }
  return nullptr;
}

VarBase *unimplemented(VMState &vm, const FnData &fd) {
  vm.fail(fd.src_id, fd.idx, "Unimplemented");
  return nullptr;
}

VarBase *todo(VMState &vm, const FnData &fd) {
  vm.fail(fd.src_id, fd.idx, "Todo");
  return nullptr;
}

VarBase *load_module(VMState &vm, const FnData &fd) {
  VarBase *mod_var = fd.args[1];
  if (!mod_var->istype<VarString>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected argument to be of type string, found: %s",
            vm.type_name(mod_var).c_str());
    return nullptr;
  }
  std::string mod = STR(mod_var)->get();
  if (!vm.nmod_load(STR(mod_var)->get(), fd.src_id, fd.idx)) {
    vm.fail(fd.src_id, fd.idx, "module load failed, look at error above");
    return nullptr;
  }
  return vm.nil;
}

VarBase *import_file(VMState &vm, const FnData &fd) {
  VarBase *file_var = fd.args[1];
  if (!file_var->istype<VarString>()) {
    vm.fail(file_var->src_id(), file_var->idx(),
            "expected argument to be of type string, found: %s",
            vm.type_name(file_var).c_str());
    return nullptr;
  }
  std::string file = STR(file_var)->get();
  // load_bmod() also adds the src to all_srcs map (push_src() function)
  int err = vm.bmod_load(file, fd.src_id, fd.idx);
  if (err != E_OK) {
    vm.fail(file_var->src_id(), file_var->idx(),
            "module import failed, look at error above (exit code: %d)", err);
    return nullptr;
  }
  return vm.all_srcs[file];
}

VarBase *is_main_src(VMState &vm, const FnData &fd) {
  SrcFile *src = vm.current_source_file();
  return src->is_main() ? vm.tru : vm.fals;
}

VarBase *print(VMState &vm, const FnData &fd) {
  for (size_t i = 1; i < fd.args.size(); ++i) {
    std::string str;
    if (!fd.args[i]->to_str(vm, str, fd.src_id, fd.idx)) {
      return nullptr;
    }
    fprintf(stdout, "%s", str.c_str());
  }
  return vm.nil;
}

VarBase *println(VMState &vm, const FnData &fd) {
  for (size_t i = 1; i < fd.args.size(); ++i) {
    std::string str;
    if (!fd.args[i]->to_str(vm, str, fd.src_id, fd.idx)) {
      return nullptr;
    }
    fprintf(stdout, "%s", str.c_str());
  }
  fprintf(stdout, "\n");
  return vm.nil;
}

VarBase *type_of(VMState &vm, const FnData &fd) {
  return make<VarString>(vm.type_name(fd.args[1]));
}

INIT_MODULE(core) {
  const std::string &src_name = vm.current_source_file()->path();

  // fundamental functions for builtin types
  vm.add_native_typefn<VarAll>("_type_", all_get_type, 0, src_id, idx);
  vm.add_native_typefn<VarAll>("_typefid_", all_get_typefid, 0, src_id, idx);
  vm.add_native_typefn<VarAll>("_typestr_", all_get_typestr, 0, src_id, idx);
  vm.add_native_typefn<VarAll>("==", all_eq, 1, src_id, idx);
  vm.add_native_typefn<VarAll>("!=", all_ne, 1, src_id, idx);
  vm.add_native_typefn<VarAll>("copy", all_copy, 0, src_id, idx);

  // to string
  vm.add_native_typefn<VarAll>("str", all_to_str, 0, src_id, idx);
  vm.add_native_typefn<VarNil>("str", nil_to_str, 0, src_id, idx);
  vm.add_native_typefn<VarTypeId>("str", typeid_to_str, 0, src_id, idx);
  vm.add_native_typefn<VarBool>("str", bool_to_str, 0, src_id, idx);
  vm.add_native_typefn<VarInt>("str", int_to_str, 0, src_id, idx);
  vm.add_native_typefn<VarFloat>("str", float_to_str, 0, src_id, idx, true);
  vm.add_native_typefn<VarString>("str", str_to_str, 0, src_id, idx);
  vm.add_native_typefn<VarVec>("str", vec_to_str, 0, src_id, idx);
  vm.add_native_typefn<VarMap>("str", map_to_str, 0, src_id, idx);

  // to bool
  vm.add_native_typefn<VarAll>("bool", all_to_bool, 0, src_id, idx);
  vm.add_native_typefn<VarNil>("bool", nil_to_bool, 0, src_id, idx);
  vm.add_native_typefn<VarTypeId>("bool", typeid_to_bool, 0, src_id, idx);
  vm.add_native_typefn<VarBool>("bool", bool_to_bool, 0, src_id, idx);
  vm.add_native_typefn<VarInt>("bool", int_to_bool, 0, src_id, idx);
  vm.add_native_typefn<VarFloat>("bool", float_to_bool, 0, src_id, idx);
  vm.add_native_typefn<VarString>("bool", str_to_bool, 0, src_id, idx);
  vm.add_native_typefn<VarVec>("bool", vec_to_bool, 0, src_id, idx);
  vm.add_native_typefn<VarMap>("bool", map_to_bool, 0, src_id, idx);

  // to int
  vm.add_native_typefn<VarNil>("int", nil_to_int, 0, src_id, idx);
  vm.add_native_typefn<VarTypeId>("int", typeid_to_int, 0, src_id, idx);
  vm.add_native_typefn<VarBool>("int", bool_to_int, 0, src_id, idx);
  vm.add_native_typefn<VarInt>("int", int_to_int, 0, src_id, idx);
  vm.add_native_typefn<VarFloat>("int", float_to_int, 0, src_id, idx);
  vm.add_native_typefn<VarString>("int", str_to_int, 0, src_id, idx);

  // to float
  vm.add_native_typefn<VarNil>("float", nil_to_float, 0, src_id, idx);
  vm.add_native_typefn<VarBool>("float", bool_to_float, 0, src_id, idx);
  vm.add_native_typefn<VarInt>("float", int_to_float, 0, src_id, idx);
  vm.add_native_typefn<VarFloat>("float", float_to_float, 0, src_id, idx);
  vm.add_native_typefn<VarString>("float", str_to_float, 1, src_id, idx);

  // global required
  vm.gadd("ref",
          new VarFn(src_name, {""}, {}, {.native = reference}, src_id, idx),
          false);
  vm.gadd("raise",
          new VarFn(src_name, {""}, {}, {.native = raise}, src_id, idx), false);
  vm.gadd("unimplemented!",
          new VarFn(src_name, {}, {}, {.native = unimplemented}, src_id, idx), false);
  vm.gadd("todo!",
          new VarFn(src_name, {}, {}, {.native = todo}, src_id, idx), false);
  vm.gadd("mload",
          new VarFn(src_name, {""}, {}, {.native = load_module}, src_id, idx),
          false);
  vm.gadd("__import__",
          new VarFn(src_name, {""}, {}, {.native = import_file}, src_id, idx),
          false);
  vm.gadd("__ismainsrc__",
          new VarFn(src_name, {}, {}, {.native = is_main_src}, src_id, idx),
          false);
  vm.gadd("__type__",
          new VarFn(src_name, {""}, {}, {.native = type_of}, src_id, idx),
          false);
  vm.gadd("print",
          new VarFn(src_name, "", ".", {""}, {}, {.native = print}, true,
                    "print", src_id, idx),
          false);
  vm.gadd("println",
          new VarFn(src_name, "", ".", {}, {}, {.native = println}, true,
                    "println", src_id, idx),
          false);

  // core type functions

  // nil
  vm.add_native_typefn<VarNil>("==", nil_eq, 1, src_id, idx);
  vm.add_native_typefn<VarNil>("!=", nil_ne, 1, src_id, idx);

  // typeid
  vm.add_native_typefn<VarTypeId>("==", typeid_eq, 1, src_id, idx);
  vm.add_native_typefn<VarTypeId>("!=", typeid_ne, 1, src_id, idx);

  // bool
  vm.add_native_typefn<VarBool>("<", bool_lt, 1, src_id, idx);
  vm.add_native_typefn<VarBool>(">", bool_gt, 1, src_id, idx);
  vm.add_native_typefn<VarBool>("<=", bool_le, 1, src_id, idx);
  vm.add_native_typefn<VarBool>(">=", bool_ge, 1, src_id, idx);
  vm.add_native_typefn<VarBool>("==", bool_eq, 1, src_id, idx);
  vm.add_native_typefn<VarBool>("!=", bool_ne, 1, src_id, idx);

  vm.add_native_typefn<VarBool>("!", bool_not, 0, src_id, idx);

  // int
  vm.add_native_typefn<VarInt>("+", int_add, 1, src_id, idx);
  vm.add_native_typefn<VarInt>("-", int_sub, 1, src_id, idx);
  vm.add_native_typefn<VarInt>("*", int_mul, 1, src_id, idx);
  vm.add_native_typefn<VarInt>("/", int_div, 1, src_id, idx);
  vm.add_native_typefn<VarInt>("%", int_mod, 1, src_id, idx);
  vm.add_native_typefn<VarInt>("<<", int_lshift, 1, src_id, idx);
  vm.add_native_typefn<VarInt>(">>", int_rshift, 1, src_id, idx);

  vm.add_native_typefn<VarInt>("+=", int_assn_add, 1, src_id, idx);
  vm.add_native_typefn<VarInt>("-=", int_assn_sub, 1, src_id, idx);
  vm.add_native_typefn<VarInt>("*=", int_assn_mul, 1, src_id, idx);
  vm.add_native_typefn<VarInt>("/=", int_assn_div, 1, src_id, idx);
  vm.add_native_typefn<VarInt>("%=", int_assn_mod, 1, src_id, idx);
  vm.add_native_typefn<VarInt>("<<=", int_lshiftassn, 1, src_id, idx);
  vm.add_native_typefn<VarInt>(">>=", int_rshiftassn, 1, src_id, idx);

  vm.add_native_typefn<VarInt>("**", int_pow, 1, src_id, idx);
  vm.add_native_typefn<VarInt>("/#", int_root, 1, src_id, idx);
  vm.add_native_typefn<VarInt>("++x", int_preinc, 0, src_id, idx);
  vm.add_native_typefn<VarInt>("x++", int_postinc, 0, src_id, idx);
  vm.add_native_typefn<VarInt>("--x", int_predec, 0, src_id, idx);
  vm.add_native_typefn<VarInt>("x--", int_postdec, 0, src_id, idx);

  vm.add_native_typefn<VarInt>("u-", int_usub, 0, src_id, idx);

  vm.add_native_typefn<VarInt>("<", int_lt, 1, src_id, idx);
  vm.add_native_typefn<VarInt>(">", int_gt, 1, src_id, idx);
  vm.add_native_typefn<VarInt>("<=", int_le, 1, src_id, idx);
  vm.add_native_typefn<VarInt>(">=", int_ge, 1, src_id, idx);
  vm.add_native_typefn<VarInt>("==", int_eq, 1, src_id, idx);
  vm.add_native_typefn<VarInt>("!=", int_ne, 1, src_id, idx);

  vm.add_native_typefn<VarInt>("&", int_band, 1, src_id, idx);
  vm.add_native_typefn<VarInt>("|", int_bor, 1, src_id, idx);
  vm.add_native_typefn<VarInt>("^", int_bxor, 1, src_id, idx);
  vm.add_native_typefn<VarInt>("~", int_bnot, 0, src_id, idx);

  vm.add_native_typefn<VarInt>("&=", int_bandassn, 1, src_id, idx);
  vm.add_native_typefn<VarInt>("|=", int_borassn, 1, src_id, idx);
  vm.add_native_typefn<VarInt>("^=", int_bxorassn, 1, src_id, idx);

  vm.add_native_typefn<VarInt>("popcnt", int_popcnt, 0, src_id, idx);

  // float
  vm.add_native_typefn<VarFloat>("+", float_add, 1, src_id, idx);
  vm.add_native_typefn<VarFloat>("-", float_sub, 1, src_id, idx);
  vm.add_native_typefn<VarFloat>("*", float_mul, 1, src_id, idx);
  vm.add_native_typefn<VarFloat>("/", float_div, 1, src_id, idx);

  vm.add_native_typefn<VarFloat>("+=", float_assn_add, 1, src_id, idx);
  vm.add_native_typefn<VarFloat>("-=", float_assn_sub, 1, src_id, idx);
  vm.add_native_typefn<VarFloat>("*=", float_assn_mul, 1, src_id, idx);
  vm.add_native_typefn<VarFloat>("/=", float_assn_div, 1, src_id, idx);

  vm.add_native_typefn<VarFloat>("++x", float_preinc, 0, src_id, idx);
  vm.add_native_typefn<VarFloat>("x++", float_postinc, 0, src_id, idx);
  vm.add_native_typefn<VarFloat>("--x", float_predec, 0, src_id, idx);
  vm.add_native_typefn<VarFloat>("x--", float_postdec, 0, src_id, idx);

  vm.add_native_typefn<VarFloat>("u-", float_usub, 0, src_id, idx);

  vm.add_native_typefn<VarFloat>("round", float_round, 0, src_id, idx);

  vm.add_native_typefn<VarFloat>("**", float_pow, 1, src_id, idx);
  vm.add_native_typefn<VarFloat>("/#", float_root, 1, src_id, idx);

  vm.add_native_typefn<VarFloat>("<", float_lt, 1, src_id, idx);
  vm.add_native_typefn<VarFloat>(">", float_gt, 1, src_id, idx);
  vm.add_native_typefn<VarFloat>("<=", float_le, 1, src_id, idx);
  vm.add_native_typefn<VarFloat>(">=", float_ge, 1, src_id, idx);
  vm.add_native_typefn<VarFloat>("==", float_eq, 1, src_id, idx);
  vm.add_native_typefn<VarFloat>("!=", float_ne, 1, src_id, idx);

  // string
  vm.add_native_typefn<VarString>("+", str_add, 1, src_id, idx);
  vm.add_native_typefn<VarString>("*", str_mul, 1, src_id, idx);
  vm.add_native_typefn<VarString>("*=", str_mulassn, 1, src_id, idx);

  vm.add_native_typefn<VarString>("+=", str_addassn, 1, src_id, idx);

  vm.add_native_typefn<VarString>("<", str_lt, 1, src_id, idx);
  vm.add_native_typefn<VarString>(">", str_gt, 1, src_id, idx);
  vm.add_native_typefn<VarString>("<=", str_le, 1, src_id, idx);
  vm.add_native_typefn<VarString>(">=", str_ge, 1, src_id, idx);
  vm.add_native_typefn<VarString>("==", str_eq, 1, src_id, idx);
  vm.add_native_typefn<VarString>("!=", str_ne, 1, src_id, idx);

  vm.add_native_typefn<VarString>("at", str_at, 1, src_id, idx);
  vm.add_native_typefn<VarString>("[]", str_at, 1, src_id, idx);

  // std string functions

  vm.add_native_typefn<VarString>("len", str_size, 0, src_id, idx);
  vm.add_native_typefn<VarString>("clear", str_clear, 0, src_id, idx);
  vm.add_native_typefn<VarString>("empty", str_empty, 0, src_id, idx);
  vm.add_native_typefn<VarString>("front", str_front, 0, src_id, idx);
  vm.add_native_typefn<VarString>("back", str_back, 0, src_id, idx);
  vm.add_native_typefn<VarString>("push", str_push, 1, src_id, idx);
  vm.add_native_typefn<VarString>("pop", str_pop, 0, src_id, idx);
  vm.add_native_typefn<VarString>("has_char_at", str_has_char_at, 2, src_id,
                                  idx);
  vm.add_native_typefn<VarString>("set", str_setat, 2, src_id, idx);
  vm.add_native_typefn<VarString>("insert", str_insert, 2, src_id, idx);
  vm.add_native_typefn<VarString>("erase", str_erase, 1, src_id, idx);
  vm.add_native_typefn<VarString>("find", str_find, 1, src_id, idx);
  vm.add_native_typefn<VarString>("substr_native", str_substr, 2, src_id, idx);
  vm.add_native_typefn<VarString>("real_length", str_zlength, 0, src_id, idx);
  vm.add_native_typefn<VarString>("trim", str_trim, 0, src_id, idx);
  vm.add_native_typefn<VarString>("upper", str_upper, 0, src_id, idx);
  vm.add_native_typefn<VarString>("lower", str_lower, 0, src_id, idx);
  vm.add_native_typefn<VarString>("split", str_split, 0, src_id, idx, true);
  vm.add_native_typefn<VarString>("repeat", str_mul, 1, src_id, idx);

  vm.add_native_typefn<VarString>("byte", byt, 0, src_id, idx);
  vm.add_native_typefn<VarInt>("char", chr, 0, src_id, idx);

  return true;
}