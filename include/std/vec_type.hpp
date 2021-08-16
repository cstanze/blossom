

#ifndef VEC_TYPE_HPP
#define VEC_TYPE_HPP

#include "../VM/VM.hpp"

class VarVecIterable : public VarBase {
  VarVec *m_vec;
  size_t m_curr;

public:
  VarVecIterable(VarVec *vec, const size_t &src_id, const size_t &idx);
  ~VarVecIterable();

  VarBase *copy(const size_t &src_id, const size_t &idx);
  void set(VarBase *from);

  bool next(VarBase *&val);
};
#define VEC_ITERABLE(x) static_cast<VarVecIterable *>(x)

#endif // VEC_TYPE_HPP