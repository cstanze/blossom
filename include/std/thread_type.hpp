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

#ifndef THREAD_TYPE_HPP
#define THREAD_TYPE_HPP

#include <future>
#include <thread>

#include "../VM/VM.hpp"

struct ThreadRes {
  VarBase *err;
  VarBase *res;
};

class VarThread : public VarBase {
  std::thread *m_thread;
  VarFn *m_fn;
  std::shared_future<ThreadRes> *m_res;
  size_t m_id;
  bool m_owner;

public:
  VarThread(std::thread *thread, VarFn *fn, std::shared_future<ThreadRes> *res,
            const bool &owner, const size_t &src_id, const size_t &idx);
  VarThread(std::thread *thread, VarFn *fn, std::shared_future<ThreadRes> *res,
            const bool &owner, const size_t &id, const size_t &src_id,
            const size_t &idx);
  ~VarThread();

  VarBase *copy(const size_t &src_id, const size_t &idx);
  void set(VarBase *from);

  void init_id();

  inline std::thread *&get_thread() { return m_thread; }
  inline VarFn *&get_fn() { return m_fn; }
  inline std::shared_future<ThreadRes> *&get_future() { return m_res; }
  inline size_t &get_id() { return m_id; }
};
#define THREAD(x) static_cast<VarThread *>(x)

#endif // THREAD_TYPE_HPP