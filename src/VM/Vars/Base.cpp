

#include "VM/Vars/Base.hpp"

#include "VM/Memory.hpp"
#include "VM/VM.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// VAR_BASE
////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
namespace blossom {
VarBase::VarBase(const std::uintptr_t &type, const size_t &src_id,
                 const size_t &idx, const bool &callable,
                 const bool &attr_based)
    : m_type(type), m_src_id(src_id), m_idx(idx), m_ref(1), m_info(0) {
    m_info |= VI_CALLABLE;
  if (attr_based)
    m_info |= VI_ATTR_BASED;
}
VarBase::~VarBase() {}

std::uintptr_t VarBase::typefn_id() const { return m_type; }

bool VarBase::to_str(VMState &vm, std::string &data, const size_t &src_id,
                     const size_t &idx) {
  VarBase *str_fn = nullptr;
  if (attr_based())
    str_fn = attr_get("str");
  if (str_fn == nullptr)
    str_fn = vm.get_typefn(this, "str");

  if (!str_fn) {
    vm.fail(this->src_id(), this->idx(),
            "no 'str' function implemented for type: '%zu' or global type",
            this->type());
    return false;
  }
  if (!str_fn->call(vm, {this}, {}, {}, src_id, idx)) {
    vm.fail(this->src_id(), this->idx(),
            "function call 'str' for type: %zu failed", this->type());
    return false;
  }
  VarBase *str = vm.vm_stack->pop(false);
  if (!str->istype<VarString>()) {
    vm.fail(this->src_id(), this->idx(),
            "expected string return type from 'str' function, received: %s",
            vm.type_name(str).c_str());
    var_dref(str);
    return false;
  }
  data = STR(str)->get();
  var_dref(str);
  return true;
}

bool VarBase::to_bool(VMState &vm, bool &data, const size_t &src_id,
                      const size_t &idx) {
  VarBase *bool_fn = nullptr;
  if (attr_based())
    bool_fn = attr_get("bool");
  if (bool_fn == nullptr)
    bool_fn = vm.get_typefn(this, "bool");

  if (!bool_fn) {
    vm.fail(this->src_id(), this->idx(),
            "no 'bool' function implemented for type: '%zu' or global type",
            this->type());
    return false;
  }
  if (!bool_fn->call(vm, {this}, {}, {}, src_id, idx)) {
    vm.fail(this->src_id(), this->idx(),
            "function call 'bool' for type: %zu failed", this->type());
    return false;
  }
  VarBase *b = vm.vm_stack->pop(false);
  if (!b->istype<VarBool>()) {
    vm.fail(this->src_id(), this->idx(),
            "expected string return type from 'bool' function, received: %s",
            vm.type_name(b).c_str());
    var_dref(b);
    return false;
  }
  data = BOOL(b)->get();
  var_dref(b);
  return true;
}

// VarBase *
// VarBase::call(VMState &vm, const std::vector<VarBase *> &args,
//              const std::vector<FnAssnArg> &assn_args,
//              const std::unordered_map<std::string, size_t> &assn_args_loc,
//              const size_t &src_id, const size_t &idx) {
//   // check if we have a function named 'apply' for this type
//   VarBase *apply_fn = nullptr;
//   if (this->attr_based())
//     apply_fn = this->attr_get("apply");
//   if (apply_fn == nullptr)
//     apply_fn = vm.get_typefn(this, "apply");
//   if (apply_fn) {
//     if (!apply_fn->call(vm, args, assn_args, assn_args_loc, src_id, idx)) {
//       vm.fail(args[0]->src_id(), args[0]->idx(),
//               "apply function for type: %s failed",
//               vm.type_name(args[0]).c_str());
//       return nullptr;
//     }
//     return vm.vm_stack->pop(false);
//   }
//   return nullptr;
// }

VarBase *VarBase::call(VMState &vm, const std::vector<VarBase *> &args,
              const std::vector<FnAssnArg> &assn_args,
              const std::unordered_map<std::string, size_t> &assn_args_loc,
              const size_t &src_id, const size_t &idx) {
  return nullptr;
}

bool VarBase::attr_exists(const std::string &name) const { return false; }
void VarBase::attr_set(const std::string &name, VarBase *val, const bool iref) {
}
VarBase *VarBase::attr_get(const std::string &name) { return nullptr; }

void *VarBase::operator new(size_t sz) { return mem::alloc(sz); }
void VarBase::operator delete(void *ptr, size_t sz) { mem::free(ptr, sz); }

void init_typenames(VMState &vm) {
  vm.register_type<VarAll>("All");

  vm.register_type<VarNil>("Nil");
  vm.register_type<VarTypeId>("TypeId");
  vm.register_type<VarBool>("Bool");
  vm.register_type<VarInt>("Int");
  vm.register_type<VarFloat>("Float");
  vm.register_type<VarString>("String");
  vm.register_type<VarVec>("Vec");
  vm.register_type<VarMap>("Map");
  vm.register_type<VarFn>("Func");
  vm.register_type<VarSrc>("Src");
}
}
