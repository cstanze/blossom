

#include "Common/String.hpp"

#include <string>
#include <vector>
#include <sstream>

namespace String {
std::vector<std::string> split(const std::string &data, const char delim) {
  std::vector<std::string> result;
  std::stringstream ss(data);
  std::string item;
  while (std::getline(ss, item, delim)) {
    result.push_back(item);
  }
  return result;
}

std::string join(const std::vector<std::string> &data,
                              const char delim)
{
  std::string result;
  for (auto &s : data) {
    if (!result.empty())
      result += delim;
    result += s;
  }
  return result;
}

std::string stringify(const std::vector<std::string> &vec) {
  std::string res = "[";
  for (auto &e : vec) {
    res += e + ", ";
  }
  if (vec.size() > 0) {
    res.pop_back();
    res.pop_back();
  }
  res += "]";
  return res;
}
} // namespace String
