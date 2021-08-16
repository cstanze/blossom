

#include "VM/Vars/Base.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// VAR_VEC
/////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VarVec::VarVec(const std::vector<VarBase *> &val, const bool &refs,
               const size_t &src_id, const size_t &idx)
    : VarBase(type_id<VarVec>(), src_id, idx, false, false), m_val(val),
      m_refs(refs) {}
VarVec::~VarVec() {
  for (auto &v : m_val)
    var_dref(v);
}

VarBase *VarVec::copy(const size_t &src_id, const size_t &idx) {
  std::vector<VarBase *> new_vec;
  if (m_refs) {
    for (auto &v : m_val) {
      var_iref(v);
      new_vec.push_back(v);
    }
  } else {
    for (auto &v : m_val) {
      new_vec.push_back(v->copy(src_id, idx));
    }
  }
  return new VarVec(new_vec, m_refs, src_id, idx);
}
std::vector<VarBase *> &VarVec::get() { return m_val; }
bool VarVec::is_ref_vec() { return m_refs; }
void VarVec::set(VarBase *from) {
  for (auto &v : m_val) {
    var_dref(v);
  }
  m_val.clear();
  for (auto &v : VEC(from)->m_val) {
    var_iref(v);
  }
  m_val = VEC(from)->m_val;
  m_refs = VEC(from)->m_refs;
}
