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

#include "std/thread_type.hpp"

static size_t thread_id = 0;

VarThread::VarThread(std::thread* thread, VarFn* fn,
                     std::shared_future<ThreadRes>* res, const bool& owner,
                     const size_t& src_id, const size_t& idx)
    : VarBase(type_id<VarThread>(), src_id, idx, false, false),
      m_thread(thread), m_fn(fn), m_res(res), m_id(-1), m_owner(owner) {
  var_iref(m_fn);
}
VarThread::VarThread(std::thread* thread, VarFn* fn,
                     std::shared_future<ThreadRes>* res, const bool& owner,
                     const size_t& id, const size_t& src_id, const size_t& idx)
    : VarBase(type_id<VarThread>(), src_id, idx, false, false),
      m_thread(thread), m_fn(fn), m_res(res), m_id(id), m_owner(owner) {
  var_iref(m_fn);
}
VarThread::~VarThread() {
  if (m_owner) {
    if (m_thread) {
      m_thread->join();
      delete m_thread;
    }
    if (m_res) {
      if (!m_res->valid())
        m_res->wait();
      var_dref_const(m_res->get().res);
      var_dref_const(m_res->get().err);
      delete m_res;
    }
  }
  var_dref(m_fn);
}

VarBase* VarThread::copy(const size_t& src_id, const size_t& idx) {
  return new VarThread(nullptr, m_fn, nullptr, true, src_id, idx);
}
void VarThread::set(VarBase* from) {
  VarThread* t = THREAD(from);
  if (m_owner) {
    if (m_thread) {
      m_thread->join();
      delete m_thread;
    }
    if (m_res) {
      if (!m_res->valid())
        m_res->wait();
      var_dref_const(m_res->get().res);
      var_dref_const(m_res->get().err);
      delete m_res;
    }
  }
  m_owner = false;
  m_fn = t->m_fn;
  var_dref(m_fn);
  var_iref(t->m_fn);
  m_id = t->m_id;
  m_thread = t->m_thread;
  m_res = t->m_res;
}

void VarThread::init_id() { m_id = thread_id++; }