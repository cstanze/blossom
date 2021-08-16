

#include "VM/VM.hpp"
#include "VM/Vars/Base.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// VAR_SRC
/////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VarSrc::VarSrc(SrcFile *src, Vars *vars, const size_t &src_id,
               const size_t &idx, const bool owner, const bool is_thread_copy)
    : VarBase(type_id<VarSrc>(), src_id, idx, false, true), m_src(src),
      m_vars(vars), m_owner(owner), m_is_thread_copy(is_thread_copy) {}
VarSrc::~VarSrc() {
  if (m_owner) {
    if (m_vars)
      delete m_vars;
    if (!m_is_thread_copy && m_src)
      delete m_src;
  }
}

VarBase *VarSrc::copy(const size_t &src_id, const size_t &idx) {
  return new VarSrc(m_src, m_vars, src_id, idx, false);
}

VarBase *VarSrc::thread_copy(const size_t &src_id, const size_t &idx) {
  return new VarSrc(m_src, m_vars->thread_copy(src_id, idx), src_id, idx, true,
                    true);
}

void VarSrc::set(VarBase *from) {
  VarSrc *f = SRC(from);
  if (m_owner)
    delete m_vars;
  m_src = f->m_src;
  m_vars = f->m_vars;
  f->m_owner = false;
}

bool VarSrc::attr_exists(const std::string &name) const {
  return m_vars->exists(name);
}

void VarSrc::attr_set(const std::string &name, VarBase *val, const bool iref) {
  m_vars->add(name, val, iref);
}

VarBase *VarSrc::attr_get(const std::string &name) { return m_vars->get(name); }

void VarSrc::add_native_fn(const std::string &name, NativeFnPtr body,
                           const size_t &args_count, const bool is_va) {
  m_vars->add(name,
              new VarFn(m_src->path(), "", is_va ? "." : "",
                        std::vector<std::string>(args_count, ""), {},
                        {.native = body}, true, m_src->id(), 0),
              false);
}

void VarSrc::add_native_var(const std::string &name, VarBase *val,
                            const bool iref, const bool module_level) {
  if (module_level)
    m_vars->addm(name, val, iref);
  else
    m_vars->add(name, val, iref);
}

SrcFile *VarSrc::src() { return m_src; }
Vars *VarSrc::vars() { return m_vars; }
