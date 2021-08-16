

#include "Common/Errors.hpp"

#include <cstdarg>
#include <cstring>

namespace Err {
size_t &code() {
  static size_t ecode = E_OK;
  return ecode;
}

size_t &val() {
  static size_t _val = 0;
  return _val;
}

std::string &str() {
  static std::string estr = "";
  return estr;
}

void set(const size_t &err_code, const size_t &err_val, const char *msg, ...) {
  static char err[2048];
  memset(err, 0, sizeof(err));

  va_list vargs;
  va_start(vargs, msg);
  vsprintf(err, msg, vargs);
  va_end(vargs);

  code() = err_code;
  val() = err_val;
  str() = std::string(err);
}
} // namespace Err
