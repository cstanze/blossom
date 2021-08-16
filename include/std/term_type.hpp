

#ifndef TERM_TYPE_HPP
#define TERM_TYPE_HPP

#include <termios.h>

#include "../VM/VM.hpp"

class VarTerm : public VarBase {
  struct termios m_term;

public:
  VarTerm(const struct termios &term, const size_t &src_id, const size_t &idx);

  VarBase *copy(const size_t &src_id, const size_t &idx);
  void set(VarBase *from);

  inline struct termios &get() { return m_term; }
};
#define TERM(x) static_cast<VarTerm *>(x)

#endif // TERM_TYPE_HPP