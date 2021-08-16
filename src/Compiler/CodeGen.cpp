

#include "Compiler/CodeGen.hpp"

namespace gen {
bool generate(const ParseTree* ptree, Bytecode& bc) {
  return ptree->codegen(bc);
}
} // namespace gen
