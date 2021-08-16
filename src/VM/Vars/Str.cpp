

#include "VM/Vars/Base.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// VAR_STR
/////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VarString::VarString(const std::string& val, const size_t& src_id,
                     const size_t& idx)
    : VarBase(type_id<VarString>(), src_id, idx, false, false), m_val(val) {}

VarBase* VarString::copy(const size_t& src_id, const size_t& idx) {
  return new VarString(m_val, src_id, idx);
}
std::string& VarString::get() { return m_val; }
void VarString::set(VarBase* from) { m_val = STR(from)->get(); }
