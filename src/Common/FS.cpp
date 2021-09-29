

#include "Common/FS.hpp"

#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#ifndef _WIN32
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#else
#include <windows.h>
#include <direct.h>
#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include "Common/Env.hpp"

bool blossom::FS::exists(const std::string &loc) {
  #if defined(_WIN32)
  return _access(loc.c_str(), 0) == 0;
  #else
  return access(loc.c_str(), F_OK) == 0;
  #endif
}

std::string blossom::FS::absPath(const std::string &loc, std::string *dir,
                             const bool &dir_add_double_dot) {
  static char abs[MAX_PATH_CHARS];
  static char abs_tmp[MAX_PATH_CHARS];
#if defined(_WIN32)
  _fullpath(abs, loc.c_str(), MAX_PATH_CHARS);
#else
  realpath(loc.c_str(), abs);
#endif
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

bool blossom::FS::isAbsolute(const std::string &loc) {
  return loc.size() > 0 && (loc[0] == '/' || loc[0] == '\\');
}

std::string blossom::FS::cwd() {
  static char cwd[MAX_PATH_CHARS];
  cwd[0] = '\0';
  #if defined(_WIN32)
  _getcwd(cwd, MAX_PATH_CHARS);
  #else
  getcwd(cwd, MAX_PATH_CHARS);
  #endif
  // if empty, return empty string
  return cwd[0] == '\0' ? "" : cwd;
}

std::string blossom::FS::home() {
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
  #if defined(_WIN32)
  _fullpath(_abs, path.c_str(), MAX_PATH_CHARS);
  #else
  realpath(path.c_str(), _abs);
  #endif
  return _abs;
}

std::string blossom::FS::relativePath(const std::string &path,
                                      const std::string &d) {
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

std::vector<std::string>
blossom::FS::search(const std::string &dir,
       const std::function<bool(const std::string &)> &matcher)
{
  // recursively search for files (even in subdirectories)
  std::vector<std::string> files;
  std::vector<std::string> dirs;

  dirs.push_back(dir);
  while (!dirs.empty()) {
    std::string cur = dirs.back();
    dirs.pop_back();
    DIR *dp = opendir(cur.c_str());
    if (dp == nullptr)
      continue;
    struct dirent *ep;
    while ((ep = readdir(dp)) != nullptr) {
      std::string name = ep->d_name;
      if (name == "." || name == "..")
        continue;
      std::string path = cur + "/" + name;
      // if it's a directory, add it to the list of dirs to search
      // and continue (don't match directories)
      if (ep->d_type == DT_DIR) {
        dirs.push_back(path);
        continue;
      }
      if (matcher(path))
        files.push_back(path);
    }
    closedir(dp);
  }

  return files;
}
