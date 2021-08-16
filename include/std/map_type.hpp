

#ifndef MAP_TYPE_HPP
#define MAP_TYPE_HPP

#include "../VM/VM.hpp"

class VarMapIterable : public VarBase {
  VarMap *m_map;
  std::map<std::string, VarBase *>::iterator m_curr;

public:
  VarMapIterable(VarMap *map, const size_t &src_id, const size_t &idx);
  ~VarMapIterable();

  VarBase *copy(const size_t &src_id, const size_t &idx);
  void set(VarBase *from);

  bool next(VarBase *&val, const size_t &src_id, const size_t &idx);
};
#define MAP_ITERABLE(x) static_cast<VarMapIterable *>(x)

#endif // MAP_TYPE_HPP