

#ifndef COMMON_STRING_HPP
#define COMMON_STRING_HPP

#include <string>
#include <vector>

namespace String {
std::vector<std::string> split(const std::string &data, const char delim);
std::string join(const std::vector<std::string> &data,
                              const char delim);
std::string stringify(const std::vector<std::string> &vec);
} // namespace String

#endif // COMMON_STRING_HPP