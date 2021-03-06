

#include <dirent.h>
#include <sys/wait.h>
#include <unistd.h>

#include <regex>

#include "Compiler/Parser.hpp"
#include "VM/VM.hpp"

bool eval(VMState &vm, const std::string &data, std::string &res,
          const size_t &src_id, const size_t &idx);

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// Functions
///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VarBase *templ(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarString>()) {
    vm.fail(fd.args[1]->src_id(), fd.args[1]->idx(),
            "expected a string for format string parameter, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }

  std::string fmt_str = STR(fd.args[1])->get();
  std::string tmp;
  bool prev_back_slash = false;
  int brace_count = 0;

  for (size_t i = 0; i < fmt_str.size(); ++i) {
    if (fmt_str[i] == '#' && fmt_str.length() > (i + 1) && fmt_str[i + 1] == '{') {
      ++brace_count;
      if (brace_count > 1) {
        goto capture;
      }
      fmt_str.erase(fmt_str.begin() + i);
      fmt_str.erase(fmt_str.begin() + i--);
      continue;
    }
    if (fmt_str[i] == '}' && brace_count > 0) {
      --brace_count;
      if (brace_count > 0) {
        goto capture;
      }
      std::string res;
      if (!eval(vm, tmp, res, fd.args[1]->src_id(), fd.args[1]->idx())) {
        return nullptr;
      }
      tmp.clear();
      fmt_str.erase(fmt_str.begin() + i);
      fmt_str.insert(fmt_str.begin() + i, res.begin(), res.end());
      i += res.size();
      --i;
      continue;
    }
  capture:
    if (brace_count > 0) {
      tmp += fmt_str[i];
      fmt_str.erase(fmt_str.begin() + i--);
      continue;
    }
    prev_back_slash = fmt_str[i] == '\\';
  }
  if (brace_count > 0) {
    vm.fail(fd.args[1]->src_id(), fd.args[1]->idx(),
            "invalid template: mismatched braces found (brace count: %d)",
            brace_count);
    return nullptr;
  }

  return make<VarString>(fmt_str);
}

INIT_MODULE(fmt) {
  vm.current_source()->add_native_fn("template", templ, 1);
  return true;
}

bool eval(VMState &vm, const std::string &data, std::string &res,
          const size_t &src_id, const size_t &idx) {
  SrcFile *src = vm.current_source_file();
  size_t begin_stack_sz = vm.vm_stack->size();
  static size_t i = 0;
  Bytecode bc;
  Errors err = vm.bmod_read_code_fn()(data, src->dir(), src->path(), bc, false, true, 0, -1);
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