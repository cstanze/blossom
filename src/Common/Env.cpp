

#include <cstdlib>
#include <cstring>

#if __APPLE__
#include <mach-o/dyld.h> // for _NSGetExecutablePath()
#elif __FreeBSD__
#include <sys/sysctl.h> // for sysctl()
#include <sys/types.h>
#else
#include <unistd.h> // for readlink()
#endif

#include "Common/Env.hpp"
#include "Common/FS.hpp"

namespace blossom {
namespace Env {
std::string get(const std::string &key) {
  const char *env = getenv(key.c_str());
  return env == NULL ? "" : env;
}

std::string get_proc_path() {
  char path[MAX_PATH_CHARS];
  memset(path, 0, MAX_PATH_CHARS);
#if __linux__ || __ANDROID__
  (void)readlink("/proc/self/exe", path, MAX_PATH_CHARS);
#elif __FreeBSD__
  int mib[4];
  mib[0] = CTL_KERN;
  mib[1] = KERN_PROC;
  mib[2] = KERN_PROC_PATHNAME;
  mib[3] = -1;
  size_t sz = MAX_PATH_CHARS;
  sysctl(mib, 4, path, &sz, NULL, 0);
#elif __NetBSD__
  readlink("/proc/curproc/exe", path, MAX_PATH_CHARS);
#elif __OpenBSD__ || __bsdi__ || __DragonFly__
  readlink("/proc/curproc/file", path, MAX_PATH_CHARS);
#elif __APPLE__
  uint32_t sz = MAX_PATH_CHARS;
  _NSGetExecutablePath(path, &sz);
#endif
  return path;
}
} // namespace Env
}
