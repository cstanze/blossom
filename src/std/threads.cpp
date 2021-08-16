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

#include "VM/VM.hpp"
#include "std/thread_type.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// Functions
///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

ThreadRes thread_exec(VMState *vm, VarFn *&fn, std::vector<VarBase *> args,
                      const size_t src_id, const size_t idx) {
  ThreadRes res = {nullptr, nullptr};
  if (!fn->call(*vm, args, {}, {}, src_id, idx)) {
    vm->fail(src_id, idx, "function call for thread failed");
    // go to the next part - !vm->fails.empty()
  }
  if (!vm->fails.empty()) {
    res.err = vm->fails.pop(false);
    if (vm->vm_stack->size() > 0)
      var_dref(vm->vm_stack->back());
    return res;
  }
  if (vm->vm_stack->size() > 0) {
    res.res = vm->vm_stack->pop(false);
  } else {
    var_iref(vm->nil);
    res.res = vm->nil;
  }
  delete vm;
  for (auto &arg : args)
    var_dref(arg);
  return res;
}

VarBase *threads_max(VMState &vm, const FnData &fd) {
  return make<VarInt>(std::thread::hardware_concurrency());
}

VarBase *threads_new(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarFn>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected function to be executed as first parameter, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  VarFn *fn = FN(fd.args[1]);
  return make<VarThread>(nullptr, fn, nullptr, true);
}

VarBase *thread_start(VMState &vm, const FnData &fd) {
  std::vector<VarBase *> args;
  args.push_back(nullptr); // for 'self'
  for (size_t i = 1; i < fd.args.size(); ++i) {
    var_iref(fd.args[i]);
    args.push_back(fd.args[i]);
  }
  VarThread *t = THREAD(fd.args[0]);
  if (t->get_id() == -1)
    t->init_id();
  VarFn *&fn = t->get_fn();
  std::packaged_task<ThreadRes(VMState *, VarFn *&, std::vector<VarBase *>,
                               const size_t, const size_t)>
      task(thread_exec);
  VMState *threadvm = vm.thread_copy(fd.src_id, fd.idx);
  t->get_future() = new std::shared_future<ThreadRes>(task.get_future());
  t->get_thread() = new std::thread(std::move(task), threadvm, std::ref(fn),
                                    args, fd.src_id, fd.idx);
  return vm.nil;
}

VarBase *thread_get_id(VMState &vm, const FnData &fd) {
  if (THREAD(fd.args[0])->get_id() == -1)
    THREAD(fd.args[0])->init_id();
  return make<VarInt>(THREAD(fd.args[0])->get_id());
}

VarBase *thread_is_done(VMState &vm, const FnData &fd) {
  std::shared_future<ThreadRes> *&fut = THREAD(fd.args[0])->get_future();
  if (!fut->valid())
    return vm.fals;
  return fut->wait_for(std::chrono::seconds(0)) == std::future_status::ready
             ? vm.tru
             : vm.fals;
}

VarBase *thread_join(VMState &vm, const FnData &fd) {
  std::shared_future<ThreadRes> *&fut = THREAD(fd.args[0])->get_future();
  if (!fut->valid())
    fut->wait();
  if (fut->get().err) {
    vm.fail(fd.src_id, fd.idx, fut->get().err, "thread function failed", false);
    return nullptr;
  }
  return fut->get().res;
}

INIT_MODULE(threads) {
  VarSrc *src = vm.current_source();

  src->add_native_fn("max", threads_max, 0);
  src->add_native_fn("new", threads_new, 1);

  vm.add_native_typefn<VarThread>("start", thread_start, 0, src_id, idx, true);
  vm.add_native_typefn<VarThread>("id", thread_get_id, 0, src_id, idx);
  vm.add_native_typefn<VarThread>("done", thread_is_done, 0, src_id, idx);
  vm.add_native_typefn<VarThread>("join", thread_join, 0, src_id, idx);

  return true;
}