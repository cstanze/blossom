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

#include "std/mutex_type.hpp"

VarMutex::VarMutex(const size_t &src_id, const size_t &idx)
    : VarBase(type_id<VarMutex>(), src_id, idx, false, false) {}

VarBase *VarMutex::copy(const size_t &src_id, const size_t &idx) {
  return new VarMutex(src_id, idx);
}
void VarMutex::set(VarBase *from) {
  // nothing to reassign in a mutex
}