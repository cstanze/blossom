

#ifndef COMPILER_LOAD_FILE_HPP
#define COMPILER_LOAD_FILE_HPP

#include <unordered_map>

#include "../Common/Errors.hpp"
#include "../VM/SrcFile.hpp"

Errors bmod_read_code(const std::string &data, const std::string &src_dir,
                      const std::string &src_path, Bytecode &bc,
                      const size_t &flags, const bool is_main_src,
                      const bool &expr_only, const size_t &begin_idx = 0,
                      const size_t &end_idx = -1);

SrcFile *bmod_load(const std::string &src_file, const std::string &src_dir,
                   const size_t &flags, const bool is_main_src, Errors &err,
                   const size_t &begin_idx = 0, const size_t &end_idx = -1);

#endif // COMPILER_LOAD_FILE_HPP
