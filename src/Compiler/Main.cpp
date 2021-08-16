

#include <cstdio>
#include <cstring>
#include <fstream>

#include "Common/Config.hpp"
#include "Common/Env.hpp"
#include "Common/FS.hpp"
#include "Common/String.hpp"
#include "Compiler/Args.hpp"
#include "Compiler/LoadFile.hpp"
#include "VM/VM.hpp"

int main(int argc, char** argv) {
  std::unordered_map<std::string, std::string> args;
  std::vector<std::string> code_args;
  size_t flags = args::parse(argc, (const char**)argv, args, code_args);

  if (flags & OPT_V) {
    fprintf(stdout, "Blossom %d.%d.%d\nBuilt with %s\nOn %s\n",
            BLOSSOM_VERSION_MAJOR, BLOSSOM_VERSION_MINOR, BLOSSOM_VERSION_PATCH,
            BUILD_CXX_COMPILER, BUILD_DATE);
    return E_OK;
  }

  Errors err = E_OK;

  // blossom binary absolute location
  std::string blossom_base, blossom_bin;
  blossom_bin = FS::absPath(Env::get_proc_path(), &blossom_base, true);

  std::string src_file = args["__main__"];
  if (src_file.empty())
    src_file = "repl";

  if ((src_file == "test" || src_file == "repl") && !FS::exists(src_file)) {
    src_file = blossom_base + "/include/blossom/" + src_file + bmod_ext();
  } else if (src_file == "install" || src_file == "build") {
    if (src_file == "install")
      code_args.insert(code_args.begin(), "install");
    src_file = std::string("build") + bmod_ext();
  }

  if (src_file.empty() || flags & OPT_H) {
    fprintf(stderr, "usage: %s [flags] <source file>\n", argv[0]);
    return E_FAIL;
  }

  if (!FS::exists(src_file)) {
    fprintf(stderr, "file load fail: file '%s' does not exist\n",
            src_file.c_str());
    err = E_FAIL;
    return err;
  }

  std::string src_dir;
  src_file = FS::absPath(src_file, &src_dir);

  SrcFile* main_src = bmod_load(src_file, src_dir, flags, true, err);
  if (err != E_OK) {
    return err;
  }

  int exec_err = 0;
  if (!(flags & OPT_D)) {
    VMState vm(blossom_bin, blossom_base, code_args, flags);
    vm.set_bmod_load_fn(bmod_load);
    vm.set_bmod_read_code_fn(bmod_read_code);
    vm.push_src(main_src, 0);
    if (!vm.load_core_mods()) {
      vm.pop_src();
      err = E_EXEC_FAIL;
      return err;
    }
    exec_err = vm::exec(vm);
    vm.pop_src();
  } else {
    if (main_src)
      delete main_src;
  }
  return exec_err;
}
