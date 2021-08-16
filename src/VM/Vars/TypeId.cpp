

#include "VM/Vars/Base.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// VAR_TYPEID
///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VarTypeId::VarTypeId(const std::uintptr_t& val, const size_t& src_id,
                     const size_t& idx)
    : VarBase(type_id<VarTypeId>(), src_id, idx, false, false), m_val(val) {}

VarBase* VarTypeId::copy(const size_t& src_id, const size_t& idx) {
  return new VarTypeId(m_val, src_id, idx);
}
void VarTypeId::set(VarBase* from) { m_val = TYPEID(from)->get(); }
std::uintptr_t& VarTypeId::get() { return m_val; }