

#include <unistd.h>

#include "VM/VM.hpp"
#include "std/term_type.hpp"

static struct termios attrs_orig;

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// Functions
///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VarBase* term_get_attrs(VMState& vm, const FnData& fd) {
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected file descriptor id for get_attrs, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  struct termios term;
  tcgetattr(mpz_get_si(INT(fd.args[1])->get()), &term);
  return make<VarTerm>(term);
}

VarBase* term_set_attrs(VMState& vm, const FnData& fd) {
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected file descriptor id for set_attrs, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  if (!fd.args[2]->istype<VarTerm>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected file descriptor id for set_attrs, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  bool done = tcsetattr(mpz_get_si(INT(fd.args[1])->get()), TCSAFLUSH,
                        &TERM(fd.args[2])->get()) != -1;
  return done ? vm.tru : vm.fals;
}

INIT_MODULE(term) {
  VarSrc* src = vm.current_source();

  tcgetattr(STDIN_FILENO, &attrs_orig);

  struct termios attrs_raw = attrs_orig;
  attrs_raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  attrs_raw.c_oflag &= ~(OPOST);
  attrs_raw.c_cflag |= (CS8);
  attrs_raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  attrs_raw.c_cc[VMIN] = 1;
  attrs_raw.c_cc[VTIME] = 0;
  src->add_native_var("attrs_orig", make_all<VarTerm>(attrs_orig, src_id, idx));
  src->add_native_var("attrs_raw", make_all<VarTerm>(attrs_raw, src_id, idx));

  src->add_native_fn("get_attrs", term_get_attrs, 1);
  src->add_native_fn("set_attrs", term_set_attrs, 2);

  src->add_native_var("fd_stdin", make_all<VarInt>(STDIN_FILENO, src_id, idx));
  src->add_native_var("fd_stdout",
                      make_all<VarInt>(STDOUT_FILENO, src_id, idx));
  src->add_native_var("fd_stderr",
                      make_all<VarInt>(STDERR_FILENO, src_id, idx));

  return true;
}