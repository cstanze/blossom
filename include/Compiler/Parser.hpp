

#ifndef COMPILER_PARSER_HPP
#define COMPILER_PARSER_HPP

#include "Parser/Internal.hpp"

// TODO: add checks for incorrect position of continue and break statements

namespace parser {
Errors parse(ParseHelper &ph, lex::toks_t &toks, ParseTree *&ptree,
             const size_t begin = 0);
}

#endif // COMPILER_PARSER_HPP
