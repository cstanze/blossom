

#include "VM/VM.hpp"
#include "std/bytebuffer_type.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// Functions
///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VarBase *bytebuffer_new_native(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected int argument for bytebuffer size, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  return make<VarByteBuffer>(mpz_get_ui(INT(fd.args[1])->get()));
}

VarBase *bytebuffer_resize(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected int argument for bytebuffer size, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  VarByteBuffer *self = BYTEBUFFER(fd.args[0]);
  self->resize(mpz_get_ui(INT(fd.args[1])->get()));
  return fd.args[0];
}

VarBase *bytebuffer_set_len(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected int argument for bytebuffer len, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  VarByteBuffer *self = BYTEBUFFER(fd.args[0]);
  self->set_len(mpz_get_ui(INT(fd.args[1])->get()));
  return fd.args[0];
}

VarBase *bytebuffer_size(VMState &vm, const FnData &fd) {
  return make<VarInt>(BYTEBUFFER(fd.args[0])->get_size());
}

VarBase *bytebuffer_len(VMState &vm, const FnData &fd) {
  return make<VarInt>(BYTEBUFFER(fd.args[0])->get_len());
}

VarBase *bytebuffer_to_str(VMState &vm, const FnData &fd) {
  VarByteBuffer *self = BYTEBUFFER(fd.args[0]);
  if (self->get_len() == 0)
    return make<VarString>("");
  return make<VarString>(std::string(self->get_buf(), self->get_len()));
}

INIT_MODULE(bytebuffer) {
  VarSrc *src = vm.current_source();
  src->add_native_fn("new_native", bytebuffer_new_native, 1);

  vm.register_type<VarByteBuffer>("ByteBuffer", src_id, idx);

  vm.add_native_typefn<VarByteBuffer>("resize", bytebuffer_resize, 1, src_id,
                                      idx);
  vm.add_native_typefn<VarByteBuffer>("set_len", bytebuffer_set_len, 1, src_id,
                                      idx);
  vm.add_native_typefn<VarByteBuffer>("cap", bytebuffer_size, 0, src_id, idx);
  vm.add_native_typefn<VarByteBuffer>("len", bytebuffer_len, 0, src_id, idx);
  vm.add_native_typefn<VarByteBuffer>("str", bytebuffer_to_str, 0, src_id, idx);
  return true;
}