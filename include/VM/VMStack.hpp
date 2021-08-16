

#ifndef VM_VM_STACK_HPP
#define VM_VM_STACK_HPP

#include <vector>

#include "Vars/Base.hpp"

class VMStack {
  std::vector<VarBase *> m_vec;

public:
  VMStack();
  ~VMStack();

  void push(VarBase *val, const bool iref = true);

  VarBase *pop(const bool dref = true);

  inline VarBase *&back() { return m_vec.back(); }

  inline std::vector<VarBase *> &get() { return m_vec; }

  inline size_t size() const { return m_vec.size(); }

  inline bool empty() const { return m_vec.empty(); }

  VMStack *thread_copy();
};

#endif // VM_VM_STACK_HPP
