

#include "std/term_type.hpp"

VarTerm::VarTerm(const struct termios& term, const size_t& src_id,
                 const size_t& idx)
    : VarBase(type_id<VarTerm>(), src_id, idx, false, false), m_term(term) {}

VarBase* VarTerm::copy(const size_t& src_id, const size_t& idx) {
  return new VarTerm(m_term, src_id, idx);
}

void VarTerm::set(VarBase* from) { m_term = TERM(from)->get(); }