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

#ifndef MUTEX_TYPE_HPP
#define MUTEX_TYPE_HPP

#include <mutex>

#include "../VM/VM.hpp"

class VarMutex : public VarBase {
  std::mutex m_mtx;

public:
  VarMutex(const size_t &src_id, const size_t &idx);

  VarBase *copy(const size_t &src_id, const size_t &idx);
  void set(VarBase *from);

  inline std::mutex &get() { return m_mtx; }
};
#define MUTEX(x) static_cast<VarMutex *>(x)

#endif // MUTEX_TYPE_HPP