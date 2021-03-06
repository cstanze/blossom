#include "VM/SrcFile.hpp"
#include "Common/FS.hpp"

#include <cstdarg>

static size_t src_id() {
  static size_t sid = 0;
  return sid++;
}

namespace blossom {

SrcFile::SrcFile(const std::string &dir, const std::string &path,
                 const bool is_main)
    : m_id(src_id()), m_dir(dir), m_path(path), m_is_main(is_main) {}

Errors SrcFile::load_file() {
  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  fp = fopen(m_path.c_str(), "r");
  if (fp == NULL) {
    fprintf(stderr, "failed to open source file: %s\n", m_path.c_str());
    return E_FILE_IO;
  }

  size_t prefix_idx = m_data.size();
  std::string code;
  std::vector<src_col_range_t> cols;
  size_t begin, end;
  while ((read = getline(&line, &len, fp)) != -1) {
    begin = code.size();
    code += line;
    end = code.size();
    cols.push_back({prefix_idx + begin, prefix_idx + end});
  }

  fclose(fp);
  if (line)
    free(line);

  if (code.empty()) {
    fprintf(stderr, "encountered empty file: %s\n", m_path.c_str());
    return E_FILE_EMPTY;
  }

  add_data(code);
  add_cols(cols);
  return E_OK;
}

void SrcFile::add_data(const std::string &data) { m_data += data; }
void SrcFile::add_cols(const std::vector<src_col_range_t> &cols) {
  m_cols.insert(m_cols.end(), cols.begin(), cols.end());
}

void SrcFile::fail(const size_t &idx, const char *msg, ...) const {
  va_list vargs;
  va_start(vargs, msg);
  fail(idx, msg, vargs);
  va_end(vargs);
}

void SrcFile::fail(const size_t &idx, const char *msg, va_list vargs) const {
  size_t line, col_begin, col_end, col;
  bool found = false;
  for (size_t i = 0; i < m_cols.size(); ++i) {
    if (idx >= m_cols[i].begin && idx < m_cols[i].end) {
      line = i;
      col_begin = m_cols[i].begin;
      col_end = m_cols[i].end;
      col = idx - col_begin;
      found = true;
      break;
    }
  }
  if (!found && !m_is_bytecode_loaded) {
    fprintf(stderr, "could not find error: ");
    vfprintf(stderr, msg, vargs);
    fprintf(stderr, "\n");
    fprintf(stderr, "in file: %s, with index: %zu\n", m_path.c_str(), idx);
    return;
  }

  fprintf(stderr,
          "%s:%zu:%zu: error: ", FS::relativePath(m_path, m_dir).c_str(),
          line + 1, col + 1);

  vfprintf(stderr, msg, vargs);
  fprintf(stderr, "\n");

  if (m_is_bytecode_loaded) return; // no source code available

  std::string err_line = m_data.substr(col_begin, col_end - col_begin);
  if (err_line.back() == '\n')
    err_line.pop_back();
  fprintf(stderr, "%s\n", err_line.c_str());

  std::string spcs;
  int tab_count = 0;
  for (auto &ch : err_line) {
    if (ch == '\t')
      ++tab_count;
  }
  for (int i = 0; i < tab_count; ++i)
    spcs += '\t';
  for (int i = 0; i < col - tab_count; ++i) {
    spcs += " ";
  }
  fprintf(stderr, "%s^\n", spcs.c_str());
}
}
