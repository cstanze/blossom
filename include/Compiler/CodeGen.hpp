

#ifndef COMPILER_CODE_GEN_HPP
#define COMPILER_CODE_GEN_HPP

#include "CodeGen/Internal.hpp"

namespace gen {
bool generate(const ParseTree *ptree, Bytecode &bc);
}

#endif // COMPILER_CODE_GEN_HPP
