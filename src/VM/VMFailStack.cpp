

#include "VM/VMFailStack.hpp"

vm_failstack_t::vm_failstack_t() {}

vm_failstack_t::~vm_failstack_t() { assert(m_stack.size() == 0); }

void vm_failstack_t::push(VarBase* val, const bool iref) {
  if (iref)
    var_iref(val);
  m_stack.back().push_back(val);
}

VarBase* vm_failstack_t::pop(const bool dref) {
  if (m_stack.size() == 0 || m_stack.back().size() == 0)
    return nullptr;
  VarBase* front = nullptr;
  front = m_stack.back().front();
  m_stack.back().pop_front();
  if (dref)
    var_dref(front);
  return front;
}
