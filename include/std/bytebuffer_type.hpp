

#ifndef BYTEBUFFER_TYPE_HPP
#define BYTEBUFFER_TYPE_HPP

#include "../VM/VM.hpp"

class VarByteBuffer : public VarBase {
  char *m_buffer;
  size_t m_size;
  size_t m_len;

public:
  VarByteBuffer(const size_t &buf_size, const size_t &src_id,
                const size_t &idx);
  ~VarByteBuffer();

  VarBase *copy(const size_t &src_id, const size_t &idx);
  void set(VarBase *from);

  void resize(const size_t &new_size);
  void set_len(const size_t &new_len);

  inline char *&get_buf() { return m_buffer; }
  inline const size_t &get_size() { return m_size; }
  inline const size_t &get_len() { return m_len; }
};
#define BYTEBUFFER(x) static_cast<VarByteBuffer *>(x)

#endif // BYTEBUFFER_TYPE_HPP