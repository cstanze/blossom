#ifndef COMPILER_DUMP_FILE_HPP
#define COMPILER_DUMP_FILE_HPP

#include "Common/Config.hpp"
#include "VM/VM.hpp"
#include "VM/OpCodes.hpp"
#include <cstring>

#if defined(__FreeBSD__) || defined(__NetBSD__)
#include <sys/endian.h>
#define htonll(x) htobe64(x)
#define htonl(x) htole32(x)
#elif defined(__OpenBSD__)
#include <sys/types.h>
#define htonll(x) htobe64(x)
#define htonl(x) htole32(x)
#elif defined(__linux__)
#include <endian.h>
#define htonll(x) htobe64(x)
#define htonl(x) htole32(x)
#elif defined(__APPLE__)
#include <libkern/OSByteOrder.h>
#endif

Errors bmod_dump_file(Bytecode &bc, const char *filename);

#endif
