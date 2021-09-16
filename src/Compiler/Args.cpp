#include "Compiler/Args.hpp"

#include <cstring>
#include <iostream>

namespace args {
Args *parsedArgs = nullptr;

void printUsage(const char *argv0) {
  std::cout << "Usage: " << argv0 << " [options] <file> [code_args]" << std::endl;
  std::cout << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -b        Show byte code" << std::endl;
  std::cout << "  -d        Dry run" << std::endl;
  // std::cout << "  -e REPL" << std::endl;
  std::cout << "  -f        Classic compiler" << std::endl;
  std::cout << "  -p        Show parse tree (AST)" << std::endl;
  std::cout << "  -t        Show tokens" << std::endl;
  std::cout << "  -v        Show version" << std::endl;
  std::cout << "  -r        Recursively show everything (FrontEnd->VM->Import->FrontEnd...)" << std::endl;
  std::cout << "  -o <file> Output bytecode to file" << std::endl;
  std::cout << "  -h        Show this help message" << std::endl;
}

Errors parse(const int argc, const char **argv,
             std::unordered_map<std::string, std::string> &args,
             std::vector<std::string> &code_args) {
  bool main_done = false;
  char prev_flag = '\0';
  parsedArgs = new Args();

  for (int i = 1; i < argc; ++i) {
    if (main_done) {
      code_args.push_back(argv[i]);
      continue;
    }

    size_t len = strlen(argv[i]);

    if (len > 2 && argv[i][0] == '-' && argv[i][1] == '-') {
      args.emplace("__long_opt__", argv[i]);
      continue;
    }

    if (argv[i][0] != '-') {
      args.emplace("__main__", argv[i]);
      main_done = true;
      continue;
    }

    if (argv[i][0] == '-') {
      for (size_t j = 1; j < len; ++j) {
        switch (argv[i][j]) {
        case 'b':
          parsedArgs->showBytecode = true;
          break;
        case 'd':
          parsedArgs->dryRun = true;
          break;
        case 'e':
          parsedArgs->repl = true;
          break;
        case 'f':
          parsedArgs->classic = true;
          break;
        case 'p':
          parsedArgs->showParseTree = true;
          break;
        case 'r':
          parsedArgs->showRecursive = true;
          break;
        case 't':
          parsedArgs->showTokens = true;
          break;
        case 'v':
          parsedArgs->showVersion = true;
          break;
        case 'h':
          printUsage(argv[0]);
          exit(0);
          break;
        case 'o':
          if(argc > i + 1) {
            parsedArgs->hasOutputFile = true;
            parsedArgs->outputFile = argv[i + 1];
            i++;
          } else {
            std::cerr << "Error: -o requires an argument" << std::endl;
            return E_ARGS_FAIL;
          }
          break;
        default:
          std::cerr << "Error: Unknown flag: " << argv[i][j] << std::endl;
          return E_ARGS_FAIL;
        }
      }
    }
  }

  return E_OK;
}

} // namespace args
