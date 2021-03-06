

#ifndef VM_MEMORY_HPP
#define VM_MEMORY_HPP

#include <cstddef>
#include <list>
#include <map>
#include <vector>

namespace blossom {
typedef unsigned char u8;

struct __sys_align_t {
  char c;
  size_t sz;
};

static constexpr size_t POOL_SIZE = 4 * 1024;
static constexpr size_t ALIGNMENT = sizeof(__sys_align_t) - sizeof(size_t);

struct mem_pool_t {
  u8 *head;
  u8 *mem;
};

class mem_mgr_t {
  std::vector<mem_pool_t> m_pools;
  std::map<size_t, std::list<u8 *>> m_free_chunks;

  void alloc_pool();

public:
  mem_mgr_t();
  ~mem_mgr_t();
  static mem_mgr_t &instance();
  void *alloc(size_t sz);
  void free(void *ptr, size_t sz);
};

namespace mem {
size_t mult8_roundup(size_t sz);

inline void *alloc(size_t sz) { return mem_mgr_t::instance().alloc(sz); }

inline void free(void *ptr, size_t sz) {
  return mem_mgr_t::instance().free(ptr, sz);
}
} // namespace mem
} // namespace blossom
#endif // VM_MEMORY_HPP
