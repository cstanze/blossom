

#include "Common/FS.hpp"

#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include "Common/Env.hpp"

namespace FS {
bool exists(const std::string &loc) { return access(loc.c_str(), F_OK) != -1; }

std::string absPath(const std::string &loc, std::string *dir,
                    const bool &dir_add_double_dot) {
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

bool isAbsolute(const std::string &loc) {
  return loc.size() > 0 && (loc[0] == '/' || loc[0] == '\\');
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

std::vector<std::string>
commonPrefix(const std::vector<std::vector<std::string>> &paths) {
  if (paths.empty())
    return {};

  std::vector<std::string> s1 = paths[0];
  std::vector<std::string> s2 = paths[1];
  for (size_t i = 0; i < s1.size(); ++i) {
    std::string c = s1[i];
    if (c != s2[i]) {
      std::vector<std::string> res;
      for (size_t j = 0; j < i; ++j)
        res.push_back(s1[j]);
      return res;
    }
  }
  return s1;
}

std::string _absPath(const std::string &path) {
  char _abs[MAX_PATH_CHARS];
  realpath(path.c_str(), _abs);
  return std::string(_abs);
}

std::string relativePath(const std::string &path, const std::string &d) {
  if (path.empty())
    return "";
  std::string dir = d.empty() ? cwd() : d;

  std::string cur = _absPath(dir);
  std::string abs = _absPath(path);

  auto absParts = String::split(abs, "/");
  auto curParts = String::split(cur, "/");

  size_t i = commonPrefix({absParts, curParts}).size();

  std::vector<std::string> relParts;
  for (int j = 0; j < curParts.size() - i; ++j) {
    relParts.push_back("..");
  }
  for (int j = i; j < absParts.size(); ++j) {
    relParts.push_back(absParts[j]);
  }

  std::string res = String::join(relParts, '/');
  if (res.find('/') == std::string::npos)
    return "./" + res;
  return res;
}

} // namespace FS