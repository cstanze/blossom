

#ifndef STRUCT_TYPE_HPP
#define STRUCT_TYPE_HPP

#include "../VM/VM.hpp"

class VarStructDef : public VarBase {
  std::vector<std::string> m_attr_order;
  std::unordered_map<std::string, VarBase *> m_attrs;
  // type id of struct which will be used as m_type for struct objects
  std::uintptr_t m_id;

public:
  VarStructDef(const std::uintptr_t &id,
               const std::vector<std::string> &attr_order,
               const std::unordered_map<std::string, VarBase *> &attrs,
               const size_t &src_id, const size_t &idx);
  ~VarStructDef();

  VarBase *copy(const size_t &src_id, const size_t &idx);
  void set(VarBase *from);

  // returns VarStruct
  VarBase *call(VMState &vm, const std::vector<VarBase *> &args,
                const std::vector<FnAssnArg> &assn_args,
                const std::unordered_map<std::string, size_t> &assn_args_loc,
                const size_t &src_id, const size_t &idx);

  bool attr_exists(const std::string &name) const;
  void attr_set(const std::string &name, VarBase *val, const bool iref);
  VarBase *attr_get(const std::string &name);

  const std::vector<std::string> &attr_order() const;
  const std::unordered_map<std::string, VarBase *> &attrs() const;
  std::uintptr_t typefn_id() const;
};
#define STRUCT_DEF(x) static_cast<VarStructDef *>(x)

class VarStruct : public VarBase {
  std::unordered_map<std::string, VarBase *> m_attrs;
  std::uintptr_t m_id;
  VarStructDef *m_base;

public:
  VarStruct(const std::uintptr_t &struct_id,
            const std::unordered_map<std::string, VarBase *> &attrs,
            VarStructDef *base, const size_t &src_id, const size_t &idx);
  ~VarStruct();

  std::uintptr_t typefn_id() const;

  VarBase *copy(const size_t &src_id, const size_t &idx);
  void set(VarBase *from);

  bool attr_exists(const std::string &name) const;
  void attr_set(const std::string &name, VarBase *val, const bool iref);
  VarBase *attr_get(const std::string &name);

  const std::vector<std::string> &attr_order() const;
  const std::unordered_map<std::string, VarBase *> &attrs() const;
  VarStructDef *base() const;
};
#define STRUCT(x) static_cast<VarStruct *>(x)

#endif // STRUCT_TYPE_HPP