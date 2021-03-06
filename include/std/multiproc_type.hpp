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

#ifndef MULTIPROC_TYPE_HPP
#define MULTIPROC_TYPE_HPP

#include <future>
#include <thread>

#include "../VM/VM.hpp"

class VarMultiproc : public VarBase {
  std::thread *m_thread;
  std::shared_future<int> *m_res;
  size_t m_id;
  bool m_owner;

public:
  VarMultiproc(std::thread *thread, std::shared_future<int> *res,
               const size_t &src_id, const size_t &idx,
               const bool &owner = true);
  VarMultiproc(std::thread *thread, std::shared_future<int> *res,
               const size_t &id, const size_t &src_id, const size_t &idx,
               const bool &owner = true);
  ~VarMultiproc();

  VarBase *copy(const size_t &src_id, const size_t &idx);
  void set(VarBase *from);

  inline std::thread *&get_thread() { return m_thread; }
  inline std::shared_future<int> *&get_future() { return m_res; }
  inline size_t get_id() { return m_id; }
};
#define MULTIPROC(x) static_cast<VarMultiproc *>(x)

#endif // MULTIPROC_TYPE_HPP