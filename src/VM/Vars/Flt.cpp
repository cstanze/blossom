

#include "VM/Vars/Base.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// VAR_float
/////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VarFloat::VarFloat(const mpfr_t val, const size_t &src_id, const size_t &idx)
    : VarBase(type_id<VarFloat>(), src_id, idx, false, false) {
  mpfr_init_set(m_val, val, mpfr_get_default_rounding_mode());
}
VarFloat::VarFloat(const double &val, const size_t &src_id, const size_t &idx)
    : VarBase(type_id<VarFloat>(), src_id, idx, false, false) {
  mpfr_init_set_d(m_val, val, mpfr_get_default_rounding_mode());
}
VarFloat::VarFloat(const mpz_t val, const size_t &src_id, const size_t &idx)
    : VarBase(type_id<VarFloat>(), src_id, idx, false, false) {
  mpfr_init_set_z(m_val, val, mpfr_get_default_rounding_mode());
}
VarFloat::VarFloat(const char *val, const size_t &src_id, const size_t &idx)
    : VarBase(type_id<VarFloat>(), src_id, idx, false, false) {
  mpfr_init_set_str(m_val, val, 0, mpfr_get_default_rounding_mode());
}
VarFloat::~VarFloat() { mpfr_clear(m_val); }

VarBase *VarFloat::copy(const size_t &src_id, const size_t &idx) {
  return new VarFloat(m_val, src_id, idx);
}
mpfr_t &VarFloat::get() { return m_val; }
void VarFloat::set(VarBase *from) {
  mpfr_set(m_val, FLOAT(from)->get(), mpfr_get_default_rounding_mode());
}