

#ifndef VM_VM_HPP
#define VM_VM_HPP

#include <deque>
#include <string>
#include <unordered_map>

#include "DyLib.hpp"
#include "SrcFile.hpp"
#include "VMFailStack.hpp"
#include "VMStack.hpp"
#include "Vars.hpp"

#define _STRINGIZE(x) #x
#define STRINGIFY(x) _STRINGIZE(x)

typedef std::vector<VarSrc *> src_stack_t;

typedef std::unordered_map<std::string, VarSrc *> all_srcs_t;

#define EXEC_STACK_MAX_DEFAULT 2000

typedef Errors (*bmod_read_code_fn_t)(
    const std::string &data, const std::string &src_dir,
    const std::string &src_path, Bytecode &bc, const size_t &flags,
    const bool is_main_src, const bool &expr_only, const size_t &begin_idx,
    const size_t &end_idx);

typedef SrcFile *(*bmod_load_fn_t)(const std::string &src_file,
                                   const std::string &src_dir,
                                   const size_t &flags, const bool is_main_src,
                                   Errors &err, const size_t &begin_idx,
                                   const size_t &end_idx);

typedef bool (*ModInitFn)(VMState &vm, const size_t src_id, const size_t &idx);
typedef void (*ModDeInitFn)();
#define INIT_MODULE(name)                                                      \
  extern "C" bool init_##name(VMState &vm, const size_t src_id,                \
                              const size_t &idx)
#define DEINIT_MODULE(name) extern "C" void deinit_##name()

template <typename T, typename... Args> T *make(Args... args) {
  // 0, 0 for src_id and idx
  T *res = new T(args..., 0, 0);
  res->dref();
  return res;
}

template <typename T, typename... Args> T *make_all(Args... args) {
  // 0, 0 for src_id and idx
  T *res = new T(args...);
  res->dref();
  return res;
}

struct VMState {
  bool exit_called;
  bool exec_stack_count_exceeded;
  size_t exit_code;
  size_t exec_flags;
  size_t exec_stack_count;
  size_t exec_stack_max;

  // vm fail stack
  vm_failstack_t fails;

  src_stack_t src_stack;
  all_srcs_t all_srcs;
  VMStack *vm_stack;

  // globally common variables
  VarBase *tru;
  VarBase *fals;
  VarBase *nil;

  // this is a pointer since it must be explicitly deleted after everything else
  DynLib *dlib;

  // arguments for blossom source from command line
  VarBase *src_args;

  VMState(const std::string &self_bin, const std::string &self_base,
          const std::vector<std::string> &args, const size_t &flags,
          const bool &is_thread_copy = false);
  ~VMState();

  void push_src(SrcFile *src, const size_t &idx);
  void push_src(const std::string &src_path);
  void pop_src();

  // modules & imports
  // nmod = native module
  // bmod = blossom module
  bool mod_exists(const std::vector<std::string> &locs, std::string &mod,
                  const std::string &ext, std::string &dir);
  bool nmod_load(const std::string &mod_str, const size_t &src_id,
                 const size_t &idx);
  // updated mod_str with actual file name (full canonical path)
  int bmod_load(std::string &mod_str, const size_t &src_id, const size_t &idx);
  inline bmod_read_code_fn_t bmod_read_code_fn() { return m_src_read_code_fn; }

  inline void set_bmod_load_fn(bmod_load_fn_t load_fn) {
    m_src_load_fn = load_fn;
  }
  inline void set_bmod_read_code_fn(bmod_read_code_fn_t read_code_fn) {
    m_src_read_code_fn = read_code_fn;
  }

  inline bmod_load_fn_t get_bmod_load_fn() { return m_src_load_fn; }
  inline bmod_read_code_fn_t get_bmod_read_code_fn() {
    return m_src_read_code_fn;
  }

  inline const std::vector<std::string> &inc_locs() const { return m_inc_locs; }
  inline const std::vector<std::string> &dll_locs() const { return m_dll_locs; }

  inline VarSrc *current_source() const { return src_stack.back(); }
  inline SrcFile *current_source_file() const {
    return src_stack.back()->src();
  }

  void gadd(const std::string &name, VarBase *val, const bool iref = true);
  VarBase *gget(const std::string &name);

  template <typename... T>
  void register_type(const std::string &name, const size_t &src_id = 0,
                     const size_t &idx = 0) {
    set_typename(type_id<T...>(), name);
    VarTypeId *type_var = make_all<VarTypeId>(type_id<T...>(), src_id, idx);
    if (src_stack.empty())
      gadd(name, type_var);
    else
      src_stack.back()->add_native_var(name, type_var, true, true);
  }

  void add_typefn(const std::uintptr_t &type, const std::string &name,
                  VarBase *fn, const bool iref);
  template <typename... T>
  void add_native_typefn(const std::string &name, NativeFnPtr fn,
                         const size_t &args_count, const size_t &src_id,
                         const size_t &idx, const bool &is_va = false) {
    add_typefn(type_id<T...>(), name,
               new VarFn(src_stack.back()->src()->path(), "", is_va ? "." : "",
                         std::vector<std::string>(args_count, ""), {},
                         {.native = fn}, true, src_id, idx),
               false);
  }
  VarBase *get_typefn(VarBase *var, const std::string &name);

  // used to convert typeid -> name
  void set_typename(const std::uintptr_t &type, const std::string &name);
  std::string type_name(const std::uintptr_t &type);
  std::string type_name(const VarBase *val);

  inline const std::string &self_bin() const { return m_self_bin; }
  inline const std::string &self_base() const { return m_self_base; }

  void fail(const size_t &src_id, const size_t &idx, const char *msg, ...);

  // msg can be nullptr
  void fail(const size_t &src_id, const size_t &idx, VarBase *val,
            const char *msg, const bool &iref = true);

  bool load_core_mods();

  VMState *thread_copy(const size_t &src_id, const size_t &idx);

  inline bool is_thread_copy() { return m_is_thread_copy; }

private:
  // file loading function
  bmod_load_fn_t m_src_load_fn;
  // code loading function
  bmod_read_code_fn_t m_src_read_code_fn;
  // include and module locations - searches in increasing order of vector
  // elements
  std::vector<std::string> m_inc_locs;
  std::vector<std::string> m_dll_locs;
  // global vars/objects that are required
  std::unordered_map<std::string, VarBase *> m_globals;
  // functions for any and all C++ types
  std::unordered_map<std::uintptr_t, VarsFrame *> m_typefns;
  // names of types (optional)
  std::unordered_map<std::uintptr_t, std::string> m_typenames;
  // all functions to call before unloading dlls
  std::unordered_map<std::string, ModDeInitFn> m_dll_deinit_fns;
  // path where blossom binary exists (used by sys.self_bin())
  std::string m_self_bin;
  // parent directory of where blossom binary exists (used by sys.self_base())
  std::string m_self_base;
  // is this vm instance a thread copy
  bool m_is_thread_copy;
};

const char *nmod_ext();
const char *bmod_ext(const bool compiled = false);

namespace vm {
// end = 0 = till size of bcode
int exec(VMState &vm, const Bytecode *custom_bcode = nullptr,
         const size_t &begin = 0, const size_t &end = 0);
} // namespace vm

#endif // VM_VM_HPP
