

#include "std/struct_type.hpp"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// VAR_STRUCT_DEF
//////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VarStructDef::VarStructDef(
    const std::uintptr_t &id, const std::vector<std::string> &attr_order,
    const std::unordered_map<std::string, VarBase *> &attrs,
    const size_t &src_id, const size_t &idx)
    : VarBase(type_id<VarStructDef>(), src_id, idx, true, true),
      m_attr_order(attr_order), m_attrs(attrs), m_id(id) {}

VarStructDef::~VarStructDef() {
  for (auto &attr : m_attrs) {
    var_dref(attr.second);
  }
}

VarBase *VarStructDef::copy(const size_t &src_id, const size_t &idx) {
  std::unordered_map<std::string, VarBase *> attrs;
  for (auto &attr : m_attrs) {
    attr.second->copy(src_id, idx);
  }
  return new VarStructDef(m_id, m_attr_order, attrs, src_id, idx);
}

void VarStructDef::set(VarBase *from) {
  VarStructDef *st = STRUCT_DEF(from);
  m_attr_order = st->m_attr_order;
  for (auto &attr : m_attrs) {
    var_dref(attr.second);
  }
  for (auto &attr : st->m_attrs) {
    var_iref(attr.second);
  }
  m_attrs = st->m_attrs;
  m_id = st->m_id;
}

VarBase *
VarStructDef::call(VMState &vm, const std::vector<VarBase *> &args,
                   const std::vector<FnAssnArg> &assn_args,
                   const std::unordered_map<std::string, size_t> &assn_args_loc,
                   const size_t &src_id, const size_t &idx) {
  for (auto &aa : assn_args) {
    if (std::find(m_attr_order.begin(), m_attr_order.end(), aa.name) ==
        m_attr_order.end()) {
      vm.fail(aa.src_id, aa.idx,
              "no attribute named '%s' in the structure definition",
              aa.name.c_str());
      return nullptr;
    }
  }
  std::unordered_map<std::string, VarBase *> attrs;
  auto it = m_attr_order.begin();
  for (auto argit = args.begin() + 1; argit != args.end(); ++argit) {
    auto &arg = *argit;
    if (it == m_attr_order.end()) {
      vm.fail(arg->src_id(), arg->idx(),
              "provided more arguments than existing in structure definition");
      goto fail;
    }
    if (m_attrs[*it]->type() != arg->type()) {
      vm.fail(arg->src_id(), arg->idx(), "expected type: %s, found: %s",
              vm.type_name(m_attrs[*it]).c_str(), vm.type_name(arg).c_str());
      goto fail;
    }
    attrs[*it] = arg->copy(src_id, idx);
    ++it;
  }

  for (auto &a_arg : assn_args) {
    if (m_attrs.find(a_arg.name) == m_attrs.end()) {
      vm.fail(a_arg.src_id, a_arg.idx,
              "attribute %s does not exist in this strucutre",
              a_arg.name.c_str());
      goto fail;
    }
    if (m_attrs[a_arg.name]->type() != a_arg.val->type()) {
      vm.fail(a_arg.src_id, a_arg.idx, "expected type: %s, found: %s",
              vm.type_name(m_attrs[a_arg.name]).c_str(),
              vm.type_name(a_arg.val).c_str());
      goto fail;
    }
    if (attrs.find(a_arg.name) != attrs.end()) {
      var_dref(attrs[a_arg.name]);
    }
    attrs[a_arg.name] = a_arg.val->copy(src_id, idx);
  }

  while (it < m_attr_order.end()) {
    if (attrs.find(*it) != attrs.end()) {
      ++it;
      continue;
    }
    attrs[*it] = m_attrs[*it]->copy(src_id, idx);
    ++it;
  }

  return new VarStruct(m_id, attrs, this, src_id, idx);
fail:
  for (auto attr : attrs) {
    var_dref(attr.second);
  }
  return nullptr;
}

bool VarStructDef::attr_exists(const std::string &name) const {
  return m_attrs.find(name) != m_attrs.end();
}

void VarStructDef::attr_set(const std::string &name, VarBase *val,
                            const bool iref) {
  if (m_attrs.find(name) != m_attrs.end()) {
    var_dref(m_attrs[name]);
  }
  if (iref)
    var_iref(val);
  m_attrs[name] = val;
}

VarBase *VarStructDef::attr_get(const std::string &name) {
  if (m_attrs.find(name) == m_attrs.end())
    return nullptr;
  return m_attrs[name];
}

const std::vector<std::string> &VarStructDef::attr_order() const {
  return m_attr_order;
}
const std::unordered_map<std::string, VarBase *> &VarStructDef::attrs() const {
  return m_attrs;
}
std::uintptr_t VarStructDef::typefn_id() const { return m_id; }

//////////////////////////////////////////////////////////////////////////////////////////////////
////VarStructDef/////////////////////// VAR_STRUCT
//////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VarStruct::VarStruct(const std::uintptr_t &struct_id,
                     const std::unordered_map<std::string, VarBase *> &attrs,
                     VarStructDef *base, const size_t &src_id,
                     const size_t &idx)
    : VarBase(type_id<VarStruct>(), src_id, idx, false, true), m_attrs(attrs),
      m_id(struct_id), m_base(base) {
  var_iref(m_base);
}

VarStruct::~VarStruct() {
  for (auto &attr : m_attrs) {
    var_dref(attr.second);
  }
  var_dref(m_base);
}

std::uintptr_t VarStruct::typefn_id() const { return m_id; }

VarBase *VarStruct::copy(const size_t &src_id, const size_t &idx) {
  std::unordered_map<std::string, VarBase *> attrs;
  for (auto &attr : m_attrs) {
    attrs[attr.first] = attr.second->copy(src_id, idx);
  }
  return new VarStruct(m_id, attrs, m_base, src_id, idx);
}

void VarStruct::set(VarBase *from) {
  VarStruct *st = STRUCT(from);
  m_id = st->m_id;

  var_dref(m_base);
  var_iref(st->m_base);
  m_base = st->m_base;

  for (auto &attr : m_attrs) {
    var_dref(attr.second);
  }
  for (auto &attr : st->m_attrs) {
    var_iref(attr.second);
  }
  m_attrs = st->m_attrs;
}

bool VarStruct::attr_exists(const std::string &name) const {
  return m_attrs.find(name) != m_attrs.end();
}

void VarStruct::attr_set(const std::string &name, VarBase *val,
                         const bool iref) {
  if (m_attrs.find(name) != m_attrs.end()) {
    var_dref(m_attrs[name]);
  }
  if (iref)
    var_iref(val);
  m_attrs[name] = val;
}

VarBase *VarStruct::attr_get(const std::string &name) {
  if (m_attrs.find(name) == m_attrs.end()) {
    return m_base ? m_base->attr_get(name) : nullptr;
  }
  return m_attrs[name];
}

const std::unordered_map<std::string, VarBase *> &VarStruct::attrs() const {
  return m_attrs;
}