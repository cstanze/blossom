

#ifndef COMMON_FS_HPP
#define COMMON_FS_HPP

#include "String.hpp"
#include <string>
#include <vector>

#define MAX_PATH_CHARS 4096

namespace blossom {
namespace FS {
bool exists(const std::string &loc);

std::string absPath(const std::string &loc, std::string *dir = nullptr,
                    const bool &dir_add_double_dot = false);

bool isAbsolute(const std::string &loc);

std::string cwd();
std::string home();

std::string relativePath(const std::string &path, const std::string &dir = "");

std::vector<std::string> search(const std::string &dir,
                                const std::function<bool(const std::string &)> &matcher);

} // namespace FS
} // namespace blossom
#endif // COMMON_FS_HPP
