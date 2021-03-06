

#include <sys/errno.h> // errno
#include <sys/stat.h>  // stat()

#include <cstring> // strerror()

#include "VM/VM.hpp"
#include "std/struct_type.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// Functions
///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VarBase *stat_native(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarStruct>()) {
    vm.fail(fd.args[1]->src_id(), fd.args[1]->idx(),
            "expected a struct (of type stat_t) as first argument, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  static const std::vector<std::string> reqd_keys = {
      "dev",  "ino",   "mode",  "nlink", "uid",     "gid",   "rdev",
      "size", "atime", "mtime", "ctime", "blksize", "blocks"};
  VarStruct *st = STRUCT(fd.args[1]);
  for (auto &key : reqd_keys) {
    VarBase *val = st->attr_get(key);
    if (val == nullptr) {
      vm.fail(fd.args[1]->src_id(), fd.args[1]->idx(),
              "expected attribute '%s' in struct of type stat_t (provided "
              "invalid struct)",
              key.c_str());
      return nullptr;
    } else if (!val->istype<VarInt>()) {
      vm.fail(fd.args[1]->src_id(), fd.args[1]->idx(),
              "expected attribute '%s' to be of type 'int', found: '%s'",
              key.c_str(), vm.type_name(val).c_str());
      return nullptr;
    }
  }
  if (!fd.args[2]->istype<VarString>()) {
    vm.fail(
        fd.args[2]->src_id(), fd.args[2]->idx(),
        "expected a file name string parameter as second argument, found: %s",
        vm.type_name(fd.args[2]).c_str());
    return nullptr;
  }

  struct stat _stat;
  int res = stat(STR(fd.args[2])->get().c_str(), &_stat);
  if (res != 0) {
    // vm.fail( fd.args[ 2 ]->src_id(), fd.args[ 2 ]->idx(),
    // 	 "stat for '%s' failed with error: '%s'",
    // 	 STR( fd.args[ 2 ] )->get().c_str(), strerror( errno ) );
    vm.fail(fd.src_id, fd.idx, new VarInt(errno, fd.src_id, fd.idx),
            strerror(errno), false);
    return nullptr;
  }

  mpz_set_si(INT(st->attr_get("dev"))->get(), _stat.st_dev);
  mpz_set_si(INT(st->attr_get("ino"))->get(), _stat.st_ino);
  mpz_set_si(INT(st->attr_get("mode"))->get(), _stat.st_mode);
  mpz_set_si(INT(st->attr_get("nlink"))->get(), _stat.st_nlink);
  mpz_set_si(INT(st->attr_get("uid"))->get(), _stat.st_uid);
  mpz_set_si(INT(st->attr_get("gid"))->get(), _stat.st_gid);
  mpz_set_si(INT(st->attr_get("rdev"))->get(), _stat.st_rdev);
  mpz_set_si(INT(st->attr_get("size"))->get(), _stat.st_size);
  mpz_set_si(INT(st->attr_get("atime"))->get(), _stat.st_atime);
  mpz_set_si(INT(st->attr_get("mtime"))->get(), _stat.st_mtime);
  mpz_set_si(INT(st->attr_get("ctime"))->get(), _stat.st_ctime);
  mpz_set_si(INT(st->attr_get("blksize"))->get(), _stat.st_blksize);
  mpz_set_si(INT(st->attr_get("blocks"))->get(), _stat.st_blocks);

  return vm.nil;
}

VarBase *stat_isreg(VMState &vm, const FnData &fd) {
  VarStruct *st = STRUCT(fd.args[1]);
  int mode = mpz_get_si(INT(st->attr_get("mode"))->get());
  return S_ISREG(mode) ? vm.tru : vm.fals;
}

VarBase *stat_isdir(VMState &vm, const FnData &fd) {
  VarStruct *st = STRUCT(fd.args[1]);
  int mode = mpz_get_si(INT(st->attr_get("mode"))->get());
  return S_ISDIR(mode) ? vm.tru : vm.fals;
}

VarBase *stat_ischr(VMState &vm, const FnData &fd) {
  VarStruct *st = STRUCT(fd.args[1]);
  int mode = mpz_get_si(INT(st->attr_get("mode"))->get());
  return S_ISCHR(mode) ? vm.tru : vm.fals;
}

VarBase *stat_isblk(VMState &vm, const FnData &fd) {
  VarStruct *st = STRUCT(fd.args[1]);
  int mode = mpz_get_si(INT(st->attr_get("mode"))->get());
  return S_ISBLK(mode) ? vm.tru : vm.fals;
}

VarBase *stat_isfifo(VMState &vm, const FnData &fd) {
  VarStruct *st = STRUCT(fd.args[1]);
  int mode = mpz_get_si(INT(st->attr_get("mode"))->get());
  return S_ISFIFO(mode) ? vm.tru : vm.fals;
}

VarBase *stat_islnk(VMState &vm, const FnData &fd) {
  VarStruct *st = STRUCT(fd.args[1]);
  int mode = mpz_get_si(INT(st->attr_get("mode"))->get());
  return S_ISLNK(mode) ? vm.tru : vm.fals;
}

VarBase *stat_issock(VMState &vm, const FnData &fd) {
  VarStruct *st = STRUCT(fd.args[1]);
  int mode = mpz_get_si(INT(st->attr_get("mode"))->get());
  return S_ISSOCK(mode) ? vm.tru : vm.fals;
}

INIT_MODULE(stat) {
  VarSrc *src = vm.current_source();
  src->add_native_fn("stat_native", stat_native, 2);
  src->add_native_fn("isreg_native", stat_isreg, 1);
  src->add_native_fn("isdir_native", stat_isdir, 1);
  src->add_native_fn("ischr_native", stat_ischr, 1);
  src->add_native_fn("isblk_native", stat_isblk, 1);
  src->add_native_fn("isfifo_native", stat_isfifo, 1);
  src->add_native_fn("islnk_native", stat_islnk, 1);
  src->add_native_fn("issock_native", stat_issock, 1);
  return true;
}