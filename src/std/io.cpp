

#include <termios.h>
#include <unistd.h>

#include <iostream>

#include "VM/VM.hpp"
#include "std/fs_type.hpp"

const size_t MAX_C_STR_LEN = 1025;
extern std::unordered_map<std::string, const char *> COL;
int apply_colors(std::string &str);

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// Functions
///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

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

VarBase *fprint(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarFile>()) {
    vm.fail(fd.args[1]->src_id(), fd.args[1]->idx(),
            "expected a file argument for fprint, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  if (FILE(fd.args[1])->get() == nullptr) {
    vm.fail(fd.args[1]->src_id(), fd.args[1]->idx(),
            "file has probably been closed already",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  for (size_t i = 2; i < fd.args.size(); ++i) {
    std::string str;
    if (!fd.args[i]->to_str(vm, str, fd.src_id, fd.idx)) {
      return nullptr;
    }
    fprintf(FILE(fd.args[1])->get(), "%s", str.c_str());
  }
  return vm.nil;
}

VarBase *fprintln(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarFile>()) {
    vm.fail(fd.args[1]->src_id(), fd.args[1]->idx(),
            "expected a file argument for fprintln, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  if (FILE(fd.args[1])->get() == nullptr) {
    vm.fail(fd.args[1]->src_id(), fd.args[1]->idx(),
            "file has probably been closed already",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  for (size_t i = 2; i < fd.args.size(); ++i) {
    std::string str;
    if (!fd.args[i]->to_str(vm, str, fd.src_id, fd.idx)) {
      return nullptr;
    }
    fprintf(FILE(fd.args[1])->get(), "%s", str.c_str());
  }
  fprintf(FILE(fd.args[1])->get(), "\n");
  return vm.nil;
}

VarBase *col_print(VMState &vm, const FnData &fd) {
  for (size_t i = 1; i < fd.args.size(); ++i) {
    std::string str;
    if (!fd.args[i]->to_str(vm, str, fd.src_id, fd.idx)) {
      return nullptr;
    }
    apply_colors(str);
    fprintf(stdout, "%s", str.c_str());
  }
  return vm.nil;
}

VarBase *col_println(VMState &vm, const FnData &fd) {
  for (size_t i = 1; i < fd.args.size(); ++i) {
    std::string str;
    if (!fd.args[i]->to_str(vm, str, fd.src_id, fd.idx)) {
      return nullptr;
    }
    apply_colors(str);
    fprintf(stdout, "%s", str.c_str());
  }
  fprintf(stdout, "\n");
  return vm.nil;
}

VarBase *col_dprint(VMState &vm, const FnData &fd) {
  for (size_t i = 1; i < fd.args.size(); ++i) {
    std::string str;
    if (!fd.args[i]->to_str(vm, str, fd.src_id, fd.idx)) {
      return nullptr;
    }
    apply_colors(str);
    fprintf(stderr, "%s", str.c_str());
  }
  return vm.nil;
}

VarBase *col_dprintln(VMState &vm, const FnData &fd) {
  for (size_t i = 1; i < fd.args.size(); ++i) {
    std::string str;
    if (!fd.args[i]->to_str(vm, str, fd.src_id, fd.idx)) {
      return nullptr;
    }
    apply_colors(str);
    fprintf(stderr, "%s", str.c_str());
  }
  fprintf(stderr, "\n");
  return vm.nil;
}

VarBase *scan(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarString>()) {
    vm.fail(fd.args[1]->src_id(), fd.args[1]->idx(),
            "expected string data for input prompt, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  fprintf(stdout, "%s", STR(fd.args[1])->get().c_str());

  char str[MAX_C_STR_LEN];
  fgets(str, MAX_C_STR_LEN, stdin);
  std::string res(str);

  if (res.back() == '\r')
    res.pop_back();
  if (res.back() == '\n')
    res.pop_back();

  return make<VarString>(res);
}

VarBase *scaneof(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarString>()) {
    vm.fail(fd.args[1]->src_id(), fd.args[1]->idx(),
            "expected string data for input prompt, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  fprintf(stdout, "%s", STR(fd.args[1])->get().c_str());

  std::string line, res;

  while (std::getline(std::cin, line))
    res += line;

  if (res.back() == '\r')
    res.pop_back();
  if (res.back() == '\n')
    res.pop_back();

  return make<VarString>(res);
}

VarBase *fflush(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarFile>()) {
    vm.fail(fd.args[1]->src_id(), fd.args[1]->idx(),
            "expected a file argument for fflush, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  if (FILE(fd.args[1])->get() == nullptr) {
    vm.fail(fd.args[1]->src_id(), fd.args[1]->idx(),
            "file has probably been closed already",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  fflush(FILE(fd.args[1])->get());
  return vm.nil;
}

VarBase *readchar(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(fd.args[1]->src_id(), fd.args[1]->idx(),
            "expected an integer argument for file descriptor, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  if (!fd.args[2]->istype<VarString>()) {
    vm.fail(fd.args[2]->src_id(), fd.args[2]->idx(),
            "expected a string argument for storage buffer, found: %s",
            vm.type_name(fd.args[2]).c_str());
    return nullptr;
  }

  int fdescr = mpz_get_si(INT(fd.args[1])->get());
  char c = 0;
  int res = read(fdescr, &c, 1);
  if (res > 0) {
    STR(fd.args[2])->get() = std::string(1, c);
  }
  return make<VarInt>(res);
}

INIT_MODULE(io) {
  VarSrc *src = vm.current_source();

  src->add_native_fn("print", print, 1, true);
  src->add_native_fn("println", println, 0, true);
  src->add_native_fn("fprint", fprint, 2, true);
  src->add_native_fn("fprintln", fprintln, 1, true);
  src->add_native_fn("cprint", col_print, 1, true);
  src->add_native_fn("cprintln", col_println, 0, true);
  src->add_native_fn("cdprint", col_dprint, 1, true);
  src->add_native_fn("cdprintln", col_dprintln, 0, true);
  src->add_native_fn("scan_native", scan, 1);
  src->add_native_fn("scaneof_native", scaneof, 1);
  src->add_native_fn("fflush", fflush, 1);
  src->add_native_fn("readch", readchar, 2);

  // stdout and stderr cannot be owned by a VarFile
  src->add_native_var("stdout",
                      make_all<VarFile>(stdout, "w", src_id, idx, false));
  src->add_native_var("stderr",
                      make_all<VarFile>(stderr, "w", src_id, idx, false));
  return true;
}

std::unordered_map<std::string, const char *> COL = {
    {"0", "\033[0m"},

    {"r", "\033[0;31m"},  {"g", "\033[0;32m"},  {"y", "\033[0;33m"},
    {"b", "\033[0;34m"},  {"m", "\033[0;35m"},  {"c", "\033[0;36m"},
    {"w", "\033[0;37m"},

    {"br", "\033[1;31m"}, {"bg", "\033[1;32m"}, {"by", "\033[1;33m"},
    {"bb", "\033[1;34m"}, {"bm", "\033[1;35m"}, {"bc", "\033[1;36m"},
    {"bw", "\033[1;37m"},
};

int apply_colors(std::string &str) {
  int chars = 0;
  for (size_t i = 0; i < str.size();) {
    if (str[i] == '{' &&
        (i == 0 || (str[i - 1] != '$' && str[i - 1] != '%' &&
                    str[i - 1] != '#' && str[i - 1] != '\\'))) {
      str.erase(str.begin() + i);
      if (i < str.size() && str[i] == '{') {
        ++i;
        ++chars;
        continue;
      }

      std::string var;

      while (i < str.size() && str[i] != '}') {
        var += str[i];
        str.erase(str.begin() + i);
      }

      // Remove the ending brace
      if (i < str.size())
        str.erase(str.begin() + i);

      if (var.empty())
        continue;

      if (COL.find(var) != COL.end()) {
        std::string val = COL[var];
        str.insert(str.begin() + i, val.begin(), val.end());
        i += val.size();
      }
    } else {
      ++i;
      ++chars;
    }
  }
  return chars;
}