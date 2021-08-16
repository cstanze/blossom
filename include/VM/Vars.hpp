

#ifndef VM_VARS_HPP
#define VM_VARS_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "Vars/Base.hpp"

class VarsFrame {
  std::unordered_map<std::string, VarBase *> m_vars;

public:
  VarsFrame();
  ~VarsFrame();

  inline const std::unordered_map<std::string, VarBase *> &all() const {
    return m_vars;
  }

  inline bool exists(const std::string &name) {
    return m_vars.find(name) != m_vars.end();
  }
  VarBase *get(const std::string &name);

  void add(const std::string &name, VarBase *val, const bool inc_ref);
  void rem(const std::string &name, const bool dec_ref);

  static void *operator new(size_t sz);
  static void operator delete(void *ptr, size_t sz);

  VarsFrame *thread_copy(const size_t &src_id, const size_t &idx);
};

class vars_stack_t {
  std::vector<size_t> m_loops_from;
  // each VarsFrame is a stack frame
  // it is a pointer to remove the issue of recreation of object when vector
  // increases size since recreation will cause the object to be deleted (and
  // destructor be called) and invalidate the variable pointers (since
  // destructor contains var_dref() calls)
  std::vector<VarsFrame *> m_stack;
  size_t m_top;

public:
  vars_stack_t();
  ~vars_stack_t();

  // checks if a variable exists in CURRENT scope ONLY
  bool exists(const std::string &name);
  VarBase *get(const std::string &name);

  void inc_top(const size_t &count);
  void dec_top(const size_t &count);

  void push_loop();
  // 'break' also uses this
  void pop_loop();
  void loop_continue();

  void add(const std::string &name, VarBase *val, const bool inc_ref);
  void rem(const std::string &name, const bool dec_ref);

  vars_stack_t *thread_copy(const size_t &src_id, const size_t &idx);
};

/*
 * vars for each source file
 * stash exists to add variables to a function BEFORE the block of function
 * starts this is useful for declaring function variables inside the function
 * without extra scope
 *
 * 0 cannot be a function id as it specifies source level scope and hence is
 * created in constructor
 */
class Vars {
  size_t m_fn_stack;
  std::unordered_map<std::string, VarBase *> m_stash;
  // maps function id to VarsFrame
  std::unordered_map<size_t, vars_stack_t *> m_fn_vars;

public:
  Vars();
  ~Vars();

  // checks if a variable exists in CURRENT scope ONLY
  bool exists(const std::string &name);

  VarBase *get(const std::string &name);

  void blk_add(const size_t &count);
  void blk_rem(const size_t &count);

  void push_fn();
  void pop_fn();

  void stash(const std::string &name, VarBase *val, const bool &iref = true);
  void unstash();

  inline void push_loop() { m_fn_vars[m_fn_stack]->push_loop(); }
  inline void pop_loop() { m_fn_vars[m_fn_stack]->pop_loop(); }
  inline void loop_continue() { m_fn_vars[m_fn_stack]->loop_continue(); }

  void add(const std::string &name, VarBase *val, const bool inc_ref);
  // add variable to module level unconditionally (for vm.register_new_type())
  void addm(const std::string &name, VarBase *val, const bool inc_ref);
  void rem(const std::string &name, const bool dec_ref);

  Vars *thread_copy(const size_t &src_id, const size_t &idx);
};

#endif // VM_VARS_HPP
