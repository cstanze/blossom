

#include "Compiler/Parser.hpp"

namespace parser {
Errors parse(ParseHelper &ph, lex::toks_t &toks, ParseTree *&ptree,
             const size_t begin) {
  if (parse_block(ph, (StmtBase *&)ptree, false) != E_OK)
    goto fail;
  return E_OK;
fail:
  delete ptree;
  return E_PARSE_FAIL;
}

} // namespace parser
