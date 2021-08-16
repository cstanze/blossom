

#include "std/ptr_type.hpp"

VarPtr::VarPtr(VarBase *val, const size_t &src_id, const size_t &idx)
    : VarBase(type_id<VarPtr>(), src_id, idx, false, false), m_val(val) {
  var_iref(m_val);
}
VarPtr::~VarPtr() { var_dref(m_val); }

VarBase *VarPtr::copy(const size_t &src_id, const size_t &idx) {
  return new VarPtr(m_val, src_id, idx);
}
void VarPtr::set(VarBase *from) {
  var_dref(m_val);
  m_val = PTR(from)->m_val;
  var_iref(m_val);
}

void VarPtr::update(VarBase *with) {
  var_dref(m_val);
  m_val = with;
  var_iref(m_val);
}

VarBase *VarPtr::get() { return m_val; }