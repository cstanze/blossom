

#ifndef COMMON_ENV_HPP
#define COMMON_ENV_HPP

#include <string>

namespace Env {
std::string get(const std::string &key);

std::string get_proc_path();

} // namespace Env

#endif // COMMON_ENV_HPP
