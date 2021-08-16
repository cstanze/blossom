#include "Compiler/Config.hpp"
#include "VM/VM.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// Functions
///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VarBase *_exit(VMState &vm, const FnData &fd) {
  vm.exit_called = true;
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected integer for exit function parameter - exit code");
    return nullptr;
  }
  vm.exit_code = mpz_get_si(INT(fd.args[1])->get());
  return vm.nil;
}

VarBase *var_exists(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarString>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected string argument for variable name, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  return vm.current_source()->vars()->get(STR(fd.args[1])->get()) != nullptr
             ? vm.tru
             : vm.fals;
}

VarBase *set_call_stack_max(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(fd.src_id, fd.idx, "expected int argument for max count, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  vm.exec_stack_max = mpz_get_ui(INT(fd.args[1])->get());
  return vm.nil;
}

VarBase *get_call_stack_max(VMState &vm, const FnData &fd) {
  return make<VarInt>(vm.exec_stack_max);
}

bool _eval(VMState &vm, const std::string &data, std::string &res,
           const size_t &src_id, const size_t &idx) {
  SrcFile *src = vm.current_source_file();
  size_t begin_stack_sz = vm.vm_stack->size();
  static size_t i = 0;
  Bytecode bc;
  Errors err = vm.bmod_read_code_fn()(data, src->dir(), src->path(), bc,
                                      vm.exec_flags, false, true, 0, -1);
  if (err != E_OK) {
    vm.fail(src_id, idx, "failed while parsing expression '%s' in string at",
            data.c_str());
    return false;
  }
  for (auto &b : bc.getmut()) {
    b.src_id = src_id;
    b.idx = idx;
  }
  int res_int = vm::exec(vm, &bc);
  if (res_int != E_OK) {
    vm.fail(src_id, idx, "failed while evaluating expression '%s' in string at",
            data.c_str());
    return false;
  }
  size_t end_stack_sz = vm.vm_stack->size();
  if (end_stack_sz == 0) {
    vm.fail(src_id, idx,
            "received empty vm stack\nwhile evaluating '%s' in string at",
            data.c_str());
  } else if (end_stack_sz <= begin_stack_sz) {
    vm.fail(src_id, idx,
            "expected vm stack to have one more element,"
            " found less or equal\nwhile evaluating '%s' in string at",
            data.c_str());
  } else {
    if (!vm.vm_stack->back()->to_str(vm, res, src_id, idx)) {
      return false;
    }
    vm.vm_stack->pop();
    return true;
  }
  return false;
}

VarBase *eval(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarString>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected string argument for expression, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  std::string expr = STR(fd.args[1])->get();
  std::string res;
  if (!_eval(vm, expr, res, fd.args[1]->src_id(), fd.args[1]->idx())) {
    return nullptr;
  }
  return make<VarString>(res);
}

INIT_MODULE(sys) {
  VarSrc *src = vm.current_source();

  src->add_native_fn("eval", eval, 1);
  src->add_native_fn("exit_native", _exit, 1);
  src->add_native_fn("var_exists", var_exists, 1);
  src->add_native_fn("set_call_stack_max_native", set_call_stack_max, 1);
  src->add_native_fn("get_call_stack_max", get_call_stack_max, 0);

  src->add_native_var("args", vm.src_args);

  src->add_native_var(
      "install_prefix",
      make_all<VarString>(STRINGIFY(INSTALL_PREFIX), src_id, idx));

  src->add_native_var("self_bin",
                      make_all<VarString>(vm.self_bin(), src_id, idx));
  src->add_native_var("self_base",
                      make_all<VarString>(vm.self_base(), src_id, idx));

  src->add_native_var("version_major",
                      make_all<VarInt>(BLOSSOM_VERSION_MAJOR, src_id, idx));
  src->add_native_var("version_minor",
                      make_all<VarInt>(BLOSSOM_VERSION_MINOR, src_id, idx));
  src->add_native_var("version_patch",
                      make_all<VarInt>(BLOSSOM_VERSION_PATCH, src_id, idx));

  src->add_native_var("build_date",
                      make_all<VarString>(BUILD_DATE, src_id, idx));
  src->add_native_var("build_compiler",
                      make_all<VarString>(BUILD_CXX_COMPILER, src_id, idx));

  src->add_native_var("CALL_STACK_MAX_DEFAULT",
                      make_all<VarInt>(EXEC_STACK_MAX_DEFAULT, src_id, idx));
  return true;
}
