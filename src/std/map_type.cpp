

#include "std/map_type.hpp"

#include "std/struct_type.hpp"

VarMapIterable::VarMapIterable(VarMap *map, const size_t &src_id,
                               const size_t &idx)
    : VarBase(type_id<VarMapIterable>(), src_id, idx, false, false), m_map(map),
      m_curr(map->get().begin()) {
  var_iref(m_map);
}
VarMapIterable::~VarMapIterable() { var_dref(m_map); }

VarBase *VarMapIterable::copy(const size_t &src_id, const size_t &idx) {
  return new VarMapIterable(m_map, src_id, idx);
}
void VarMapIterable::set(VarBase *from) {
  var_dref(m_map);
  m_map = MAP_ITERABLE(from)->m_map;
  var_iref(m_map);
  m_curr = MAP_ITERABLE(from)->m_curr;
}

bool VarMapIterable::next(VarBase *&val, const size_t &src_id,
                          const size_t &idx) {
  if (m_curr == m_map->get().end())
    return false;
  std::unordered_map<std::string, VarBase *> attrs;
  var_iref(m_curr->second);
  attrs["0"] = new VarString(m_curr->first, src_id, idx);
  attrs["1"] = m_curr->second;
  val = make<VarStruct>(type_id<VarMapIterable>(), attrs, nullptr);
  ++m_curr;
  return true;
}