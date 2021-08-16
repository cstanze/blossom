

#ifndef VM_VM_FAIL_STACK_HPP
#define VM_VM_FAIL_STACK_HPP

#include <deque>
#include <vector>

#include "Vars/Base.hpp"

class vm_failstack_t {
  std::vector<std::deque<VarBase *>> m_stack;

public:
  vm_failstack_t();
  ~vm_failstack_t();

  inline void blka() { m_stack.push_back(std::deque<VarBase *>{}); }
  inline void blkr() {
    for (auto &e : m_stack.back())
      var_dref(e);
    m_stack.pop_back();
  }

  void push(VarBase *val, const bool iref = true);
  VarBase *pop(const bool dref = true);

  inline size_t size() const { return m_stack.size(); }
  inline bool empty() const { return m_stack.empty(); }
  inline bool backempty() const { return m_stack.back().empty(); }
};

#endif // VM_VM_FAIL_STACK_HPP
