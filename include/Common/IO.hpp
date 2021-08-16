

#ifndef COMMON_IO_HPP
#define COMMON_IO_HPP

#include <cstdio>

namespace io {
void tadd(const bool show);
void trem(const size_t num = 1);
void print(const bool has_next, const char *fmt, ...);
} // namespace io

#endif // COMMON_IO_HPP
