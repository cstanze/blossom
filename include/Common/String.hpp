

#ifndef COMMON_STRING_HPP
#define COMMON_STRING_HPP

#include <string>
#include <vector>

namespace String {
bool endsWith(const std::string& str, const std::string& suffix);
std::vector<std::string> split(const std::string &data, const std::string &delim);
std::string join(const std::vector<std::string> &data,
                              const char delim);
std::string stringify(const std::vector<std::string> &vec);
void replace(std::string &str, const std::string &from, const std::string &to);
} // namespace String

#endif // COMMON_STRING_HPP
