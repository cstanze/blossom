

#include "std/bytebuffer_type.hpp"

#include <cstring>

VarByteBuffer::VarByteBuffer(const size_t &buf_size, const size_t &src_id,
                             const size_t &idx)
    : VarBase(type_id<VarByteBuffer>(), src_id, idx, false, false),
      m_buffer(nullptr), m_size(buf_size), m_len(0) {
  if (m_size > 0)
    m_buffer = (char *)malloc(m_size);
}
VarByteBuffer::~VarByteBuffer() {
  if (m_size > 0)
    free(m_buffer);
}

VarBase *VarByteBuffer::copy(const size_t &src_id, const size_t &idx) {
  VarByteBuffer *newbuf = new VarByteBuffer(m_size, src_id, idx);
  newbuf->set(this);
  return newbuf;
}

void VarByteBuffer::set(VarBase *from) {
  VarByteBuffer *tmp = BYTEBUFFER(from);
  if (tmp->m_size == 0) {
    if (m_size > 0)
      free(m_buffer);
    m_size = 0;
    return;
  }
  if (m_size != tmp->m_size) {
    if (m_size == 0)
      m_buffer = (char *)malloc(tmp->m_size);
    else
      m_buffer = (char *)realloc(m_buffer, tmp->m_size);
  }
  memcpy(m_buffer, tmp->m_buffer, tmp->m_size);
  m_size = tmp->m_size;
  m_len = tmp->m_len;
}

void VarByteBuffer::resize(const size_t &new_size) {
  if (new_size == 0) {
    if (m_size > 0)
      free(m_buffer);
    m_size = 0;
    return;
  }
  if (m_size == 0)
    m_buffer = (char *)malloc(new_size);
  else
    m_buffer = (char *)realloc(m_buffer, new_size);
  m_size = new_size;
}

void VarByteBuffer::set_len(const size_t &new_len) { m_len = new_len; }