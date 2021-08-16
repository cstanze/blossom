/*
        MIT License

        Copyright (c) 2021 Blossom Language repositories

        Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
        furnished to do so.
*/

#include <sys/wait.h>

#include "VM/VM.hpp"
#include "std/multiproc_type.hpp"

int exec_command(const std::string &cmd);

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// Functions
///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VarBase *multiproc_nproc(VMState &vm, const FnData &fd) {
  return make<VarInt>(std::thread::hardware_concurrency());
}

VarBase *multiproc_new(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarString>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected string argument for multiproc execution, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  std::packaged_task<int(std::string)> task(exec_command);
  std::shared_future<int> *fut = new std::shared_future<int>(task.get_future());
  return make<VarMultiproc>(
      new std::thread(std::move(task), STR(fd.args[1])->get()), fut);
}

VarBase *multiproc_get_id(VMState &vm, const FnData &fd) {
  return make<VarInt>(MULTIPROC(fd.args[0])->get_id());
}

VarBase *multiproc_is_done(VMState &vm, const FnData &fd) {
  std::shared_future<int> *&fut = MULTIPROC(fd.args[0])->get_future();
  if (!fut->valid())
    return vm.fals;
  return fut->wait_for(std::chrono::seconds(0)) == std::future_status::ready
             ? vm.tru
             : vm.fals;
}

VarBase *multiproc_get_res(VMState &vm, const FnData &fd) {
  std::shared_future<int> *&fut = MULTIPROC(fd.args[0])->get_future();
  if (!fut->valid() ||
      fut->wait_for(std::chrono::seconds(0)) != std::future_status::ready)
    return vm.nil;
  return make<VarInt>(fut->get());
}

INIT_MODULE(multiproc) {
  VarSrc *src = vm.current_source();

  src->add_native_fn("nproc", multiproc_nproc, 0);
  src->add_native_fn("new", multiproc_new, 1);

  vm.add_native_typefn<VarMultiproc>("id", multiproc_get_id, 0, src_id, idx);
  vm.add_native_typefn<VarMultiproc>("done", multiproc_is_done, 0, src_id, idx);
  vm.add_native_typefn<VarMultiproc>("res", multiproc_get_res, 0, src_id, idx);
  return true;
}

// Required because popen() and pclose() are seemingly not threadsafe
static std::mutex pipe_mtx;

int exec_command(const std::string &cmd) {
  FILE *pipe = NULL;
  {
    std::lock_guard<std::mutex> lock(pipe_mtx);
    pipe = popen(cmd.c_str(), "r");
  }
  if (!pipe)
    return 1;
  char *line = NULL;
  size_t len = 0;
  ssize_t nread;

  while ((nread = getline(&line, &len, pipe)) != -1) {
    fprintf(stdout, "%s", line);
  }

  free(line);
  int res = 0;
  {
    std::lock_guard<std::mutex> lock(pipe_mtx);
    res = pclose(pipe);
  }
  int exit_code = WEXITSTATUS(res);
  return WEXITSTATUS(res);
}