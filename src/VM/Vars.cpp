

#include "VM/Vars.hpp"

#include "VM/Memory.hpp"

namespace blossom {

VarsFrame::VarsFrame() {}
VarsFrame::~VarsFrame() {
  for (auto &var : m_vars)
    var_dref(var.second);
}

VarBase *VarsFrame::get(const std::string &name) {
  if (m_vars.find(name) == m_vars.end())
    return nullptr;
  return m_vars[name];
}

void VarsFrame::add(const std::string &name, VarBase *val, const bool inc_ref) {
  if (m_vars.find(name) != m_vars.end()) {
    var_dref(m_vars[name]);
  }
  if (inc_ref)
    var_iref(val);
  m_vars[name] = val;
}
void VarsFrame::rem(const std::string &name, const bool dec_ref) {
  if (m_vars.find(name) == m_vars.end())
    return;
  if (dec_ref)
    var_dref(m_vars[name]);
  m_vars.erase(name);
}

void *VarsFrame::operator new(size_t sz) { return mem::alloc(sz); }
void VarsFrame::operator delete(void *ptr, size_t sz) { mem::free(ptr, sz); }

VarsFrame *VarsFrame::thread_copy(const size_t &src_id, const size_t &idx) {
  VarsFrame *f = new VarsFrame;
  for (auto &var : m_vars) {
    var_iref(var.second);
    f->m_vars[var.first] = var.second;
  }
  return f;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

vars_stack_t::vars_stack_t() : m_top(0) { m_stack.push_back(new VarsFrame()); }
vars_stack_t::~vars_stack_t() {
  for (auto layer = m_stack.rbegin(); layer != m_stack.rend(); ++layer) {
    delete *layer;
  }
}

bool vars_stack_t::exists(const std::string &name) {
  return m_stack.back()->exists(name);
}

VarBase *vars_stack_t::get(const std::string &name) {
  for (auto layer = m_stack.rbegin(); layer != m_stack.rend(); ++layer) {
    if ((*layer)->exists(name)) {
      return (*layer)->get(name);
    }
  }
  return nullptr;
}

void vars_stack_t::inc_top(const size_t &count) {
  for (size_t i = 0; i < count; ++i) {
    m_stack.push_back(new VarsFrame());
    ++m_top;
  }
}
void vars_stack_t::dec_top(const size_t &count) {
  if (m_top == 0)
    return;
  for (size_t i = 0; i < count && m_top > 0; ++i) {
    delete m_stack.back();
    m_stack.pop_back();
    --m_top;
  }
}

void vars_stack_t::push_loop() {
  m_loops_from.push_back(m_top + 1);
  inc_top(1);
}

void vars_stack_t::pop_loop() {
  assert(m_loops_from.size() > 0);
  if (m_top >= m_loops_from.back()) {
    dec_top(m_top - m_loops_from.back() + 1);
  }
  m_loops_from.pop_back();
}

void vars_stack_t::loop_continue() {
  assert(m_loops_from.size() > 0);
  if (m_top > m_loops_from.back()) {
    dec_top(m_top - m_loops_from.back());
  }
}

void vars_stack_t::add(const std::string &name, VarBase *val,
                       const bool inc_ref) {
  m_stack.back()->add(name, val, inc_ref);
}
void vars_stack_t::rem(const std::string &name, const bool dec_ref) {
  for (auto layer = m_stack.rbegin(); layer != m_stack.rend(); ++layer) {
    if ((*layer)->exists(name)) {
      (*layer)->rem(name, dec_ref);
      return;
    }
  }
}

vars_stack_t *vars_stack_t::thread_copy(const size_t &src_id,
                                        const size_t &idx) {
  vars_stack_t *s = new vars_stack_t;
  s->m_loops_from = m_loops_from;
  s->m_top = m_top;
  for (auto &f : m_stack) {
    s->m_stack.push_back(f->thread_copy(src_id, idx));
  }
  return s;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

Vars::Vars() : m_fn_stack(-1) { m_fn_vars[0] = new vars_stack_t; }
Vars::~Vars() {
  assert(m_fn_stack == 0 || m_fn_stack == -1);
  delete m_fn_vars[0];
}

bool Vars::exists(const std::string &name) {
  return m_fn_vars[m_fn_stack]->exists(name);
}

VarBase *Vars::get(const std::string &name) {
  assert(m_fn_stack != -1);
  VarBase *res = m_fn_vars[m_fn_stack]->get(name);
  if (res == nullptr && m_fn_stack != 0) {
    res = m_fn_vars[0]->get(name);
  }
  return res;
}

void Vars::blk_add(const size_t &count) {
  m_fn_vars[m_fn_stack]->inc_top(count);
  for (auto &s : m_stash) {
    m_fn_vars[m_fn_stack]->add(s.first, s.second, false);
  }
  m_stash.clear();
}

void Vars::blk_rem(const size_t &count) {
  m_fn_vars[m_fn_stack]->dec_top(count);
}

void Vars::push_fn() {
  ++m_fn_stack;
  if (m_fn_stack == 0)
    return;
  m_fn_vars[m_fn_stack] = new vars_stack_t;
}
void Vars::pop_fn() {
  if (m_fn_stack == 0)
    return;
  delete m_fn_vars[m_fn_stack];
  m_fn_vars.erase(m_fn_stack);
  --m_fn_stack;
}

void Vars::stash(const std::string &name, VarBase *val, const bool &iref) {
  if (iref)
    var_iref(val);
  m_stash[name] = val;
}

void Vars::unstash() {
  for (auto &s : m_stash)
    var_dref(s.second);
  m_stash.clear();
}

void Vars::add(const std::string &name, VarBase *val, const bool inc_ref) {
  m_fn_vars[m_fn_stack]->add(name, val, inc_ref);
}

void Vars::addm(const std::string &name, VarBase *val, const bool inc_ref) {
  m_fn_vars[0]->add(name, val, inc_ref);
}

void Vars::rem(const std::string &name, const bool dec_ref) {
  m_fn_vars[m_fn_stack]->rem(name, dec_ref);
}

Vars *Vars::thread_copy(const size_t &src_id, const size_t &idx) {
  Vars *v = new Vars;
  delete v->m_fn_vars[0];
  v->m_fn_stack = m_fn_stack;
  for (auto &s : m_stash) {
    var_iref(s.second);
    v->m_stash[s.first] = s.second;
  }
  for (auto &fv : m_fn_vars) {
    v->m_fn_vars[fv.first] = fv.second->thread_copy(src_id, idx);
  }
  return v;
}
}
