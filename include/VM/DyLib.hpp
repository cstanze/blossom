

#ifndef VM_DYN_LIB_HPP
#define VM_DYN_LIB_HPP

#include <string>
#include <unordered_map>

namespace blossom {
/* Wrapper class for dlfcn.h library */
class DynLib {
  std::unordered_map<std::string, void *> m_handles;

public:
  DynLib();
  ~DynLib();
  void *load(const std::string &file);
  void unload(const std::string &file);
  void *get(const std::string &file, const std::string &sym);
  inline bool fexists(const std::string &file) {
    return m_handles.find(file) != m_handles.end();
  }
};
}
#endif // VM_DYN_LIB_HPP
