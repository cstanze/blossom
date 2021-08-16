

#include <chrono>

#include "VM/VM.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////// Functions
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VarBase* sysclk_now(VMState& vm, const FnData& fd) {
  VarInt* res = make<VarInt>(0);
  mpz_set_ui(res->get(),
             std::chrono::duration_cast<std::chrono::nanoseconds>(
                 std::chrono::system_clock::now().time_since_epoch())
                 .count());
  return res;
}

VarBase* time_format(VMState& vm, const FnData& fd) {
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected integer argument as time for formatting, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  if (!fd.args[2]->istype<VarString>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected string argument as format for time formatting, found: %s",
            vm.type_name(fd.args[2]).c_str());
    return nullptr;
  }
  unsigned long val = mpz_get_ui(INT(fd.args[1])->get());
  std::chrono::nanoseconds nsval(val);
  std::chrono::system_clock::time_point tp(
      std::chrono::duration_cast<std::chrono::system_clock::duration>(nsval));
  std::time_t time = std::chrono::system_clock::to_time_t(tp);
  std::tm* t = std::localtime(&time);
  char fmt[1024] = {0};
  if (std::strftime(fmt, sizeof(fmt), STR(fd.args[2])->get().c_str(), t)) {
    return make<VarString>(fmt);
  }
  return vm.nil;
}

INIT_MODULE(time) {
  VarSrc* src = vm.current_source();
  src->add_native_fn("system_clock_now_native", sysclk_now);
  src->add_native_fn("format_native", time_format, 2);
  return true;
}