

#include "Compiler/Parser/ParseHelper.hpp"

#include <cstdarg>

ParseHelper::ParseHelper(lex::toks_t &toks, const size_t begin)
    : m_toks(toks), m_invalid(0, TOK_INVALID, ""),
      m_eof(toks.size() > 0 ? toks.back().pos : 0, TOK_EOF, ""), m_idx(begin) {}

const lex::tok_t *ParseHelper::peak(const int offset) const {
  if (offset < 0 && m_idx < (-offset))
    return &m_eof;
  if (m_idx + offset >= m_toks.size())
    return &m_eof;
  return &m_toks[m_idx + offset];
}

TokType ParseHelper::peakt(const int offset) const {
  if (offset < 0 && m_idx < (-offset))
    return m_eof.type;
  if (m_idx + offset >= m_toks.size())
    return m_eof.type;
  return m_toks[m_idx + offset].type;
}

const lex::tok_t *ParseHelper::next() {
  ++m_idx;
  if (m_idx >= m_toks.size())
    return &m_eof;
  return &m_toks[m_idx];
}

TokType ParseHelper::nextt() {
  ++m_idx;
  if (m_idx >= m_toks.size())
    return m_eof.type;
  return m_toks[m_idx].type;
}

const lex::tok_t *ParseHelper::prev() {
  if (m_idx == 0)
    return &m_invalid;
  --m_idx;
  return &m_toks[m_idx];
}

TokType ParseHelper::prevt() {
  if (m_idx == 0)
    return m_invalid.type;
  --m_idx;
  return m_toks[m_idx].type;
}

const lex::tok_t *ParseHelper::at(const size_t &idx) const {
  if (idx >= m_toks.size())
    return &m_invalid;
  return &m_toks[idx];
}