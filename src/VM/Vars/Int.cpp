

#include "VM/Vars/Base.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// VAR_INT
/////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VarInt::VarInt(const mpz_t val, const size_t& src_id, const size_t& idx)
    : VarBase(type_id<VarInt>(), src_id, idx, false, false) {
  mpz_init_set(m_val, val);
}
VarInt::VarInt(const int& val, const size_t& src_id, const size_t& idx)
    : VarBase(type_id<VarInt>(), src_id, idx, false, false) {
  mpz_init_set_si(m_val, val);
}
VarInt::VarInt(const mpfr_t val, const size_t& src_id, const size_t& idx)
    : VarBase(type_id<VarInt>(), src_id, idx, false, false) {
  mpz_init(m_val);
  mpfr_get_z(m_val, val, mpfr_get_default_rounding_mode());
}
VarInt::VarInt(const char* val, const size_t& src_id, const size_t& idx)
    : VarBase(type_id<VarInt>(), src_id, idx, false, false) {
  mpz_init_set_str(m_val, val, 0);
}
VarInt::~VarInt() { mpz_clear(m_val); }

VarBase* VarInt::copy(const size_t& src_id, const size_t& idx) {
  return new VarInt(m_val, src_id, idx);
}
mpz_t& VarInt::get() { return m_val; }
void VarInt::set(VarBase* from) { mpz_set(m_val, INT(from)->get()); }