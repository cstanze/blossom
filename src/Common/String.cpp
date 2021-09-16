

#include "Common/String.hpp"

#include <sstream>
#include <string>
#include <vector>

namespace String {
bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::vector<std::string> split(const std::string &str,
                               const std::string &delim) {
  std::vector<std::string> tokens = std::vector<std::string>();
  std::string strCopy = str;

  std::size_t pos = 0;
  std::string token;

  while ((pos = strCopy.find(delim)) != std::string::npos) {
    token = strCopy.substr(0, pos);
    strCopy.erase(0, pos + delim.length());

    tokens.push_back(token);
  }

  if (strCopy.length() > 0) {
    tokens.push_back(strCopy);
  }

  return tokens;
}

std::string join(const std::vector<std::string> &data, const char delim) {
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

void replace(std::string &str, const std::string &from,
                const std::string &to) {
  if (from.empty())
    return;
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length(); // In case 'to' contains 'from', like replacing
                              // 'x' with 'yx'
  }
}
} // namespace String
