

#ifndef COMMON_FS_HPP
#define COMMON_FS_HPP

#include <string>
#include <vector>

#define MAX_PATH_CHARS 4096

namespace FS {
bool exists(const std::string &loc);

std::string absPath(const std::string &loc, std::string *dir = nullptr,
                    const bool &dir_add_double_dot = false);

std::string cwd();

std::string home();
} // namespace FS

#endif // COMMON_FS_HPP
