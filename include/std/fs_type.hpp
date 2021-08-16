

#ifndef FS_TYPE_HPP
#define FS_TYPE_HPP

#include "../VM/VM.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////// VAR_FILE
//////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

class VarFile : public VarBase {
  FILE *m_file;
  std::string m_mode;
  bool m_owner;

public:
  VarFile(FILE *const file, const std::string &mode, const size_t &src_id,
          const size_t &idx, const bool owner = true);
  ~VarFile();

  VarBase *copy(const size_t &src_id, const size_t &idx);
  void set(VarBase *from);

  inline FILE *&get() { return m_file; }
  inline std::string &mode() { return m_mode; }
  inline bool &owner() { return m_owner; }
};
#define FILE(x) static_cast<VarFile *>(x)

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// VAR_FILE_ITERABLE
////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

class VarFileIterable : public VarBase {
  VarFile *m_file;

public:
  VarFileIterable(VarFile *file, const size_t &src_id, const size_t &idx);
  ~VarFileIterable();

  VarBase *copy(const size_t &src_id, const size_t &idx);
  void set(VarBase *from);

  bool next(VarBase *&val);
};
#define FILE_ITERABLE(x) static_cast<VarFileIterable *>(x)

#endif // FS_TYPE_HPP