

#ifndef COMMON_ENV_HPP
#define COMMON_ENV_HPP

#include <string>

namespace blossom {
namespace Env {
using namespace blossom;
std::string get(const std::string &key);

std::string get_proc_path();

} // namespace Env
} // namespace blossom
#endif // COMMON_ENV_HPP
