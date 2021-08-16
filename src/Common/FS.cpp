

#include "Common/FS.hpp"

#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include "Common/Env.hpp"

namespace FS {
bool exists(const std::string& loc) { return access(loc.c_str(), F_OK) != -1; }

std::string absPath(const std::string& loc, std::string* dir,
                    const bool& dir_add_double_dot) {
  static char abs[MAX_PATH_CHARS];
  static char abs_tmp[MAX_PATH_CHARS];
  realpath(loc.c_str(), abs);
  if (dir != nullptr) {
    std::string _abs = abs;
    *dir = _abs.substr(0, _abs.find_last_of('/'));
    if (dir_add_double_dot) {
      *dir += "/..";
      realpath(dir->c_str(), abs_tmp);
      *dir = abs_tmp;
    }
  }
  return abs;
}

std::string cwd() {
  static char cwd[MAX_PATH_CHARS];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    return cwd;
  }
  return "";
}

std::string home() {
  static std::string _home = Env::get("HOME");
  return _home;
}
} // namespace FS