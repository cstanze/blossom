

#ifndef COMMON_FS_HPP
#define COMMON_FS_HPP

#include "String.hpp"
#include <string>
#include <vector>

#define MAX_PATH_CHARS 4096

namespace FS {
bool exists(const std::string &loc);

std::string absPath(const std::string &loc, std::string *dir = nullptr,
                    const bool &dir_add_double_dot = false);

bool isAbsolute(const std::string &loc);

std::string cwd();
std::string home();

std::string relativePath(const std::string &path, const std::string &dir = "");

} // namespace FS

#endif // COMMON_FS_HPP
