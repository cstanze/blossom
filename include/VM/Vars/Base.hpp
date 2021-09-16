

#ifndef VM_VARS_BASE_HPP
#define VM_VARS_BASE_HPP

#include <cassert>
#include <gmp.h>
#include <map>
#include <mpfr.h>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../SrcFile.hpp"

class VarBase;
struct FnAssnArg {
  size_t src_id;
  size_t idx;
  std::string name;
  VarBase *val;
};

enum VARINFO {
  VI_CALLABLE = 1 << 0,
  VI_ATTR_BASED = 1 << 1,
  VI_LOAD_AS_REF = 1 << 2,
};

struct VMState;
class VarBase {
  std::mutex m_mtx; // yes i know this is very large (40 bytes actually), but i
                    // am looking for a better way for this to work with threads
  std::uintptr_t m_type;
  size_t m_src_id;
  size_t m_idx;
  size_t m_ref;

  // right most bit = 0 => callable (bool)
  // 1 => attr_based (bool)
  // 2 => load_as_reference (bool)
  char m_info;

  // https://stackoverflow.com/questions/51332851/alternative-id-generators-for-types
  template <typename T> static inline std::uintptr_t _type_id() {
    // no need for static variable as function is inlined
    return reinterpret_cast<std::uintptr_t>(&_type_id<T>);
  }
  template <typename T> friend size_t type_id();

public:
  VarBase(const std::uintptr_t &type, const size_t &src_id, const size_t &idx,
          const bool &callable, const bool &attr_based);
  virtual ~VarBase();

  template <typename T> bool istype() const {
    return m_type == VarBase::_type_id<T>();
  }

  // must always be overridden
  virtual VarBase *copy(const size_t &src_id, const size_t &idx) = 0;
  virtual void set(VarBase *from) = 0;

  bool to_str(VMState &vm, std::string &data, const size_t &src_id,
              const size_t &idx);
  bool to_bool(VMState &vm, bool &data, const size_t &src_id,
               const size_t &idx);

  inline void set_src_id_idx(const size_t &src_id, const size_t &idx) {
    m_src_id = src_id;
    m_idx = idx;
  }

  inline std::uintptr_t type() const { return m_type; }
  // used for denoting things like structs in typefns
  virtual std::uintptr_t typefn_id() const;

  inline size_t src_id() const { return m_src_id; }
  inline size_t idx() const { return m_idx; }

  inline void iref() {
    std::lock_guard<std::mutex> lock(m_mtx);
    ++m_ref;
  }
  inline size_t dref() {
    std::lock_guard<std::mutex> lock(m_mtx);
    assert(m_ref > 0);
    --m_ref;
    return m_ref;
  }
  inline size_t ref() const { return m_ref; }

  inline bool callable() { return m_info & VI_CALLABLE; }
  inline bool attr_based() { return m_info & VI_ATTR_BASED; }

  inline void set_load_as_ref() { m_info |= VI_LOAD_AS_REF; }
  inline void unset_load_as_ref() { m_info &= ~VI_LOAD_AS_REF; }
  inline bool load_as_ref() { return m_info & VI_LOAD_AS_REF; }

  virtual VarBase *
  call(VMState &vm, const std::vector<VarBase *> &args,
       const std::vector<FnAssnArg> &assn_args,
       const std::unordered_map<std::string, size_t> &assn_args_loc,
       const size_t &src_id, const size_t &idx);

  virtual bool attr_exists(const std::string &name) const;
  virtual void attr_set(const std::string &name, VarBase *val, const bool iref);
  virtual VarBase *attr_get(const std::string &name);

  static void *operator new(size_t sz);
  static void operator delete(void *ptr, size_t sz);
};

template <typename T> std::uintptr_t type_id() {
  return VarBase::_type_id<T>();
}

template <typename T> inline void var_iref(T *var) {
  if (var == nullptr)
    return;
  var->iref();
}
template <typename T> inline void var_dref(T *&var) {
  if (var == nullptr)
    return;
  var->dref();
  if (var->ref() == 0) {
    delete var;
    var = nullptr;
  }
}
// used in std/threads library
template <typename T> inline void var_dref_const(T *const var) {
  if (var == nullptr)
    return;
  var->dref();
  if (var->ref() == 0) {
    delete var;
  }
}

// dummy type to denote all other types
class VarAll : public VarBase {
public:
  VarAll(const size_t &src_id, const size_t &idx);

  VarBase *copy(const size_t &src_id, const size_t &idx);
  void set(VarBase *from);
};

class VarNil : public VarBase {
public:
  VarNil(const size_t &src_id, const size_t &idx);

  VarBase *copy(const size_t &src_id, const size_t &idx);
  void set(VarBase *from);
};
#define NIL(x) static_cast<VarNil *>(x)

class VarTypeId : public VarBase {
  std::uintptr_t m_val;

public:
  VarTypeId(const std::uintptr_t &val, const size_t &src_id, const size_t &idx);

  VarBase *copy(const size_t &src_id, const size_t &idx);
  void set(VarBase *from);

  std::uintptr_t &get();
};
#define TYPEID(x) static_cast<VarTypeId *>(x)

class VarBool : public VarBase {
  bool m_val;

public:
  VarBool(const bool val, const size_t &src_id, const size_t &idx);

  VarBase *copy(const size_t &src_id, const size_t &idx);
  void set(VarBase *from);

  bool &get();
};
#define BOOL(x) static_cast<VarBool *>(x)

class VarInt : public VarBase {
  mpz_t m_val;

public:
  VarInt(const mpz_t val, const size_t &src_id, const size_t &idx);
  VarInt(const int &val, const size_t &src_id, const size_t &idx);
  VarInt(const mpfr_t val, const size_t &src_id, const size_t &idx);
  VarInt(const char *val, const size_t &src_id, const size_t &idx);
  ~VarInt();

  VarBase *copy(const size_t &src_id, const size_t &idx);
  void set(VarBase *from);

  mpz_t &get();
};
#define INT(x) static_cast<VarInt *>(x)

class VarFloat : public VarBase {
  mpfr_t m_val;

public:
  VarFloat(const mpfr_t val, const size_t &src_id, const size_t &idx);
  VarFloat(const double &val, const size_t &src_id, const size_t &idx);
  VarFloat(const mpz_t val, const size_t &src_id, const size_t &idx);
  VarFloat(const char *val, const size_t &src_id, const size_t &idx);
  ~VarFloat();

  VarBase *copy(const size_t &src_id, const size_t &idx);
  void set(VarBase *from);

  mpfr_t &get();
};
#define FLOAT(x) static_cast<VarFloat *>(x)

class VarString : public VarBase {
  std::string m_val;

public:
  VarString(const std::string &val, const size_t &src_id, const size_t &idx);

  VarBase *copy(const size_t &src_id, const size_t &idx);
  void set(VarBase *from);

  std::string &get();
};
#define STR(x) static_cast<VarString *>(x)

class VarVec : public VarBase {
  std::vector<VarBase *> m_val;
  bool m_refs;

public:
  VarVec(const std::vector<VarBase *> &val, const bool &refs,
         const size_t &src_id, const size_t &idx);
  ~VarVec();

  VarBase *copy(const size_t &src_id, const size_t &idx);
  void set(VarBase *from);

  std::vector<VarBase *> &get();
  bool is_ref_vec();
};
#define VEC(x) static_cast<VarVec *>(x)

class VarMap : public VarBase {
  std::map<std::string, VarBase *> m_val;
  bool m_refs;

public:
  VarMap(const std::map<std::string, VarBase *> &val, const bool &refs,
         const size_t &src_id, const size_t &idx);
  ~VarMap();

  VarBase *copy(const size_t &src_id, const size_t &idx);
  void set(VarBase *from);

  std::map<std::string, VarBase *> &get();
  bool is_ref_map();
};
#define MAP(x) static_cast<VarMap *>(x)

struct FnBodySpan {
  size_t begin;
  size_t end;
};

struct FnData {
  size_t src_id;
  size_t idx;
  std::vector<VarBase *> args;
  std::vector<FnAssnArg> assn_args;
  std::unordered_map<std::string, size_t> assn_args_loc;
};

typedef VarBase *(*NativeFnPtr)(VMState &vm, const FnData &fd);

union FnBody {
  NativeFnPtr native;
  FnBodySpan blossom;
};

class VarFn : public VarBase {
  std::string m_src_name;
  std::string m_fn_name;
  std::string m_kw_arg;
  std::string m_var_arg;
  std::vector<std::string> m_args;
  std::unordered_map<std::string, VarBase *> m_assn_args;
  FnBody m_body;
  bool m_is_native;

public:
  VarFn(const std::string &src_name, const std::string &kw_arg,
        const std::string &var_arg, const std::vector<std::string> &args,
        const std::unordered_map<std::string, VarBase *> &assn_args,
        const FnBody &body, const bool is_native, const std::string &fn_name,
        const size_t &src_id, const size_t &idx);
  VarFn(const std::string &src_name, const std::string &kw_arg,
        const std::string &var_arg, const std::vector<std::string> &args,
        const std::unordered_map<std::string, VarBase *> &assn_args,
        const FnBody &body, const bool is_native, const size_t &src_id,
        const size_t &idx);
  VarFn(const std::string &src_name, const std::vector<std::string> &args,
        const std::unordered_map<std::string, VarBase *> &assn_args,
        const FnBody &body, const size_t &src_id, const size_t &idx);
  ~VarFn();

  VarBase *copy(const size_t &src_id, const size_t &idx);
  void set(VarBase *from);

  std::string &src_name();
  std::string &kw_arg();
  std::string &var_arg();
  std::string &fn_name();
  std::vector<std::string> &args();
  std::unordered_map<std::string, VarBase *> &assn_args();
  FnBody &body();
  bool is_native();

  VarBase *call(VMState &vm, const std::vector<VarBase *> &args,
                const std::vector<FnAssnArg> &assn_args,
                const std::unordered_map<std::string, size_t> &assn_args_loc,
                const size_t &src_id, const size_t &idx);
};
#define FN(x) static_cast<VarFn *>(x)

class Vars;
class VarSrc : public VarBase {
  SrcFile *m_src;
  Vars *m_vars;
  bool m_owner;
  bool m_is_thread_copy;

public:
  VarSrc(SrcFile *src, Vars *vars, const size_t &src_id, const size_t &idx,
         const bool owner = true, const bool is_thread_copy = false);
  ~VarSrc();

  VarBase *copy(const size_t &src_id, const size_t &idx);
  VarBase *thread_copy(const size_t &src_id, const size_t &idx);
  void set(VarBase *from);

  bool attr_exists(const std::string &name) const;
  void attr_set(const std::string &name, VarBase *val, const bool iref);
  VarBase *attr_get(const std::string &name);

  void add_native_fn(const std::string &name, NativeFnPtr body,
                     const size_t &args_count = 0, const bool is_va = false);

  void add_native_var(const std::string &name, VarBase *val,
                      const bool iref = true, const bool module_level = false);

  SrcFile *src();
  Vars *vars();
};
#define SRC(x) static_cast<VarSrc *>(x)

void init_typenames(VMState &vm);

#endif // VM_VARS_BASE_HPP
