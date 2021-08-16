

#include "VM/DyLib.hpp"

#include <dlfcn.h>

#include <cstdio>
#include <string>
#include <unordered_map>

DynLib::DynLib() {}

DynLib::~DynLib() {
  for (auto &e : m_handles) {
    if (e.second != nullptr)
      dlclose(e.second);
  }
}

void *DynLib::load(const std::string &file) {
  if (m_handles.find(file) == m_handles.end()) {
    // RTLD_GLOBAL is required for allowing unique type_id<>() across shared
    // library boundaries; see the following
    // https://cpptruths.blogspot.com/2018/11/non-colliding-efficient.html
    // (section: Dynamically Loaded Libraries)
    // https://linux.die.net/man/3/dlopen (section: RTLD_GLOBAL) RTLD_NOW is
    // simply used to ensure everything is resolved at dlopen, therefore,
    // showing the internal error if not resolved (since dlopen will return NULL
    // then) this ensures proper error output and exit instead of segfaulting or
    // something
    auto tmp = dlopen(file.c_str(), RTLD_NOW | RTLD_GLOBAL);
    if (tmp == nullptr) {
      fprintf(stderr, "internal error: dyn lib failed to open %s: %s\n",
              file.c_str(), dlerror());
      return nullptr;
    }
    m_handles[file] = tmp;
  }
  return m_handles[file];
}

void DynLib::unload(const std::string &file) {
  if (m_handles.find(file) == m_handles.end())
    return;
  dlclose(m_handles[file]);
  m_handles.erase(file);
}

void *DynLib::get(const std::string &file, const std::string &sym) {
  if (m_handles.find(file) == m_handles.end())
    return nullptr;
  return dlsym(m_handles[file], sym.c_str());
}
