

#include "std/vec_type.hpp"

VarVecIterable::VarVecIterable(VarVec *vec, const size_t &src_id,
                               const size_t &idx)
    : VarBase(type_id<VarVecIterable>(), src_id, idx, false, false), m_vec(vec),
      m_curr(0) {
  var_iref(m_vec);
}
VarVecIterable::~VarVecIterable() { var_dref(m_vec); }

VarBase *VarVecIterable::copy(const size_t &src_id, const size_t &idx) {
  return new VarVecIterable(m_vec, src_id, idx);
}
void VarVecIterable::set(VarBase *from) {
  var_dref(m_vec);
  m_vec = VEC_ITERABLE(from)->m_vec;
  var_iref(m_vec);
  m_curr = VEC_ITERABLE(from)->m_curr;
}

bool VarVecIterable::next(VarBase *&val) {
  if (m_curr >= m_vec->get().size())
    return false;
  val = m_vec->get()[m_curr++];
  return true;
}