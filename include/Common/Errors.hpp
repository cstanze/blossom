

#ifndef COMMON_ERRORS_HPP
#define COMMON_ERRORS_HPP

#include <string>

enum Errors {
  E_OK,

  E_FILE_IO,
  E_FILE_EMPTY,

  E_LEX_FAIL,

  E_PARSE_FAIL,

  E_CODEGEN_FAIL,

  E_EXEC_FAIL,

  E_FAIL,
};

namespace Err {
size_t &code();
size_t &val();
std::string &str();

// err_val is for things like idx, etc.
void set(const size_t &err_code, const size_t &err_val, const char *msg, ...);
} // namespace Err

#endif // COMMON_ERRORS_HPP
