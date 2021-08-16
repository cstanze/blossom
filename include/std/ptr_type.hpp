

#ifndef PTR_TYPE_HPP
#define PTR_TYPE_HPP

#include "../VM/VM.hpp"

class VarPtr : public VarBase {
  VarBase *m_val;

public:
  VarPtr(VarBase *val, const size_t &src_id, const size_t &idx);
  ~VarPtr();

  VarBase *copy(const size_t &src_id, const size_t &idx);
  void set(VarBase *from);

  void update(VarBase *with);

  VarBase *get();
};
#define PTR(x) static_cast<VarPtr *>(x)

#endif // PTR_TYPE_HPP