

#include "VM/Vars/Base.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// VAR_MAP
/////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VarMap::VarMap(const std::map<std::string, VarBase*>& val, const bool& refs,
               const size_t& src_id, const size_t& idx)
    : VarBase(type_id<VarMap>(), src_id, idx, false, false), m_val(val),
      m_refs(refs) {}
VarMap::~VarMap() {
  for (auto& v : m_val)
    var_dref(v.second);
}

VarBase* VarMap::copy(const size_t& src_id, const size_t& idx) {
  std::map<std::string, VarBase*> new_map;
  for (auto& v : m_val) {
    new_map[v.first] = v.second->copy(src_id, idx);
  }
  return new VarMap(new_map, m_refs, src_id, idx);
}

void VarMap::set(VarBase* from) {
  for (auto& v : m_val) {
    var_dref(v.second);
  }
  m_val.clear();
  for (auto& v : MAP(from)->m_val) {
    var_iref(v.second);
  }
  m_val = MAP(from)->m_val;
  m_refs = MAP(from)->m_refs;
}

std::map<std::string, VarBase*>& VarMap::get() { return m_val; }
bool VarMap::is_ref_map() { return m_refs; }