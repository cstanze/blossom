

#include "std/fs_type.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// VAR_FILE
////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VarFile::VarFile(FILE* const file, const std::string& mode,
                 const size_t& src_id, const size_t& idx, const bool owner)
    : VarBase(type_id<VarFile>(), src_id, idx, false, false), m_file(file),
      m_mode(mode), m_owner(owner) {}
VarFile::~VarFile() {
  if (m_owner)
    fclose(m_file);
}

VarBase* VarFile::copy(const size_t& src_id, const size_t& idx) {
  return new VarFile(m_file, m_mode, src_id, idx, false);
}

void VarFile::set(VarBase* from) {
  if (m_owner)
    fclose(m_file);
  m_owner = false;
  m_file = FILE(from)->get();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// VAR_FILE_ITERABLE
///////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VarFileIterable::VarFileIterable(VarFile* file, const size_t& src_id,
                                 const size_t& idx)
    : VarBase(type_id<VarFileIterable>(), src_id, idx, false, false),
      m_file(file) {
  var_iref(m_file);
}
VarFileIterable::~VarFileIterable() { var_dref(m_file); }

VarBase* VarFileIterable::copy(const size_t& src_id, const size_t& idx) {
  return new VarFileIterable(m_file, src_id, idx);
}
void VarFileIterable::set(VarBase* from) {
  var_dref(m_file);
  m_file = FILE_ITERABLE(from)->m_file;
  var_iref(m_file);
}

bool VarFileIterable::next(VarBase*& val) {
  char* line_ptr = NULL;
  size_t len = 0;
  ssize_t read = 0;

  if ((read = getline(&line_ptr, &len, m_file->get())) != -1) {
    std::string line = line_ptr;
    free(line_ptr);
    while (line.back() == '\n')
      line.pop_back();
    while (line.back() == '\r')
      line.pop_back();
    val = make<VarString>(line);
    return true;
  }
  if (line_ptr)
    free(line_ptr);
  return false;
}