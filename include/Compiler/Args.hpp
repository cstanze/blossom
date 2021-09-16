#ifndef COMPILER_ARGS_HPP
#define COMPILER_ARGS_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include "Common/Errors.hpp"

struct Args {
  bool showBytecode;
  bool dryRun;
  bool repl;
  bool classic;
  bool showParseTree;
  bool showTokens;
  bool showVersion;
  bool showRecursive;
  bool hasOutputFile;
  std::string outputFile;

  Args(bool showBytecode, bool dryRun, bool repl, bool classic,
       bool showParseTree, bool showTokens, bool showVersion,
       bool showRecursive, std::string outputFile)
    : showBytecode(showBytecode), dryRun(dryRun), repl(repl), classic(classic),
      showParseTree(showParseTree), showTokens(showTokens),
      showVersion(showVersion), showRecursive(showRecursive),
      hasOutputFile(!outputFile.empty()), outputFile(outputFile) {}
  Args()
    : showBytecode(false), dryRun(false), repl(false), classic(false),
      showParseTree(false), showTokens(false), showVersion(false),
      showRecursive(false), hasOutputFile(false), outputFile("") {}
  ~Args() {}
};

namespace args {
extern Args *parsedArgs;
void printUsage(const char *argv0);
Errors parse(const int argc, const char **argv,
             std::unordered_map<std::string, std::string> &args,
             std::vector<std::string> &code_args);
}

#endif // COMPILER_ARGS_HPP
