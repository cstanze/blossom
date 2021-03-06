

#include "VM/VMStack.hpp"

blossom::VMStack::VMStack() {}

blossom::VMStack::~VMStack() {
  for (auto &val : m_vec) {
    var_dref(val);
  }
}

void blossom::VMStack::push(VarBase *val, const bool iref) {
  if (iref)
    var_iref(val);
  m_vec.push_back(val);
}

VarBase *blossom::VMStack::pop(const bool dref) {
  if (m_vec.size() == 0)
    return nullptr;
  VarBase *back = nullptr;
  back = m_vec.back();
  m_vec.pop_back();
  if (dref)
    var_dref(back);
  return back;
}

VMStack *blossom::VMStack::thread_copy() {
  VMStack *newstack = new VMStack;
  for (auto &item : m_vec) {
    newstack->m_vec.push_back(item->copy(item->src_id(), item->idx()));
  }
  return newstack;
}