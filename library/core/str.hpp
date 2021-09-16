

#ifndef LIBRARY_CORE_STR_HPP
#define LIBRARY_CORE_STR_HPP

#include "VM/VM.hpp"
#include <algorithm>

VarBase *str_add(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarString>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected string argument for addition, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  std::string &lhs = STR(fd.args[0])->get();
  std::string &rhs = STR(fd.args[1])->get();
  return make<VarString>(lhs + rhs);
}

VarBase *str_mul(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected integer argument for String.repeat, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  std::string &lhs = STR(fd.args[0])->get();
  mpz_t i;
  mpz_init_set_si(i, 0);
  mpz_t &rhs = INT(fd.args[1])->get();
  std::string res;
  for (; mpz_cmp(i, rhs) < 0; mpz_add_ui(i, i, 1)) {
    res += lhs;
  }
  mpz_clear(i);
  return make<VarString>(res);
}

VarBase *str_mulassn(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected integer argument for String.repeat, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  std::string &lhs = STR(fd.args[0])->get();
  mpz_t i;
  mpz_init_set_si(i, 0);
  mpz_t &rhs = INT(fd.args[1])->get();
  std::string res;
  for (; mpz_cmp(i, rhs) < 0; mpz_add_ui(i, i, 1)) {
    res += lhs;
  }
  mpz_clear(i);
  
  STR(fd.args[0])->get() = res;

  return make<VarString>(res);
}

VarBase *str_addassn(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarString>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected string argument for addition assignment, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  STR(fd.args[0])->get() += STR(fd.args[1])->get();
  return fd.args[0];
}

// logical functions

VarBase *str_lt(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarString>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected string argument for logical less than, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  std::string &lhs = STR(fd.args[0])->get();
  std::string &rhs = STR(fd.args[1])->get();
  return lhs < rhs ? vm.tru : vm.fals;
}

VarBase *str_gt(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarString>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected string argument for logical greater than, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  std::string &lhs = STR(fd.args[0])->get();
  std::string &rhs = STR(fd.args[1])->get();
  return lhs > rhs ? vm.tru : vm.fals;
}

VarBase *str_le(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarString>()) {
    vm.fail(
        fd.src_id, fd.idx,
        "expected string argument for logical less than or equal, found: %s",
        vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  std::string &lhs = STR(fd.args[0])->get();
  std::string &rhs = STR(fd.args[1])->get();
  return lhs <= rhs ? vm.tru : vm.fals;
}

VarBase *str_ge(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarString>()) {
    vm.fail(
        fd.src_id, fd.idx,
        "expected string argument for logical greater than or equal, found: %s",
        vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  std::string &lhs = STR(fd.args[0])->get();
  std::string &rhs = STR(fd.args[1])->get();
  return lhs >= rhs ? vm.tru : vm.fals;
}

VarBase *str_eq(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarString>()) {
    return vm.fals;
  }
  std::string &lhs = STR(fd.args[0])->get();
  std::string &rhs = STR(fd.args[1])->get();
  return lhs == rhs ? vm.tru : vm.fals;
}

VarBase *str_ne(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarString>()) {
    return vm.tru;
  }
  std::string &lhs = STR(fd.args[0])->get();
  std::string &rhs = STR(fd.args[1])->get();
  return lhs != rhs ? vm.tru : vm.fals;
}

VarBase *str_at(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(
        fd.src_id, fd.idx,
        "expected argument to be of type integer for string.erase(), found: %s",
        vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  std::string &str = STR(fd.args[0])->get();
  size_t pos = mpz_get_ui(INT(fd.args[1])->get());
  if (pos >= str.size())
    return vm.nil;
  return make<VarString>(std::string(1, str[pos]));
}

// string functions

std::vector<VarBase *> _str_split(const std::string &data,
                                  const std::string &delim,
                                  const size_t &src_id, const size_t &idx);

static inline void trim(std::string &s);

VarBase *str_size(VMState &vm, const FnData &fd) {
  return make<VarInt>(STR(fd.args[0])->get().size());
}

VarBase *str_clear(VMState &vm, const FnData &fd) {
  STR(fd.args[0])->get().clear();
  return vm.nil;
}

VarBase *str_empty(VMState &vm, const FnData &fd) {
  return STR(fd.args[0])->get().size() == 0 ? vm.tru : vm.fals;
}

VarBase *str_front(VMState &vm, const FnData &fd) {
  std::string &str = STR(fd.args[0])->get();
  return str.size() == 0 ? vm.nil
                         : make<VarString>(std::string(1, str.front()));
}

VarBase *str_back(VMState &vm, const FnData &fd) {
  std::string &str = STR(fd.args[0])->get();
  return str.size() == 0 ? vm.nil : make<VarString>(std::string(1, str.back()));
}

VarBase *str_push(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarString>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected string argument for string.push(), found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  std::string &src = STR(fd.args[1])->get();
  std::string &dest = STR(fd.args[0])->get();
  if (src.size() > 0)
    dest += src;
  return fd.args[0];
}

VarBase *str_pop(VMState &vm, const FnData &fd) {
  std::string &str = STR(fd.args[0])->get();
  if (str.size() > 0)
    str.pop_back();
  return fd.args[0];
}

VarBase *str_has_char_at(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(
        fd.src_id, fd.idx,
        "expected first argument to be of type integer for index, found: %s",
        vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  if (!fd.args[2]->istype<VarString>() && !fd.args[2]->istype<VarInt>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected second argument to be of type string or integer for "
            "checking, found: %s",
            vm.type_name(fd.args[2]).c_str());
    return nullptr;
  }
  size_t pos = mpz_get_ui(INT(fd.args[1])->get());
  std::string &dest = STR(fd.args[0])->get();
  if (pos >= dest.size()) {
    vm.fail(fd.src_id, fd.idx,
            "position %zu is not within string of length %zu", pos,
            dest.size());
    return nullptr;
  }
  std::string chars;
  if (fd.args[2]->istype<VarInt>()) {
    chars = mpz_get_si(INT(fd.args[2])->get());
  } else if (fd.args[2]->istype<VarString>()) {
    chars = STR(fd.args[2])->get();
  }
  return chars.find(dest[pos]) == std::string::npos ? vm.fals : vm.tru;
}

VarBase *str_setat(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected first argument to be of type integer for string.set(), "
            "found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  if (!fd.args[2]->istype<VarString>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected second argument to be of type string for string.set(), "
            "found: %s",
            vm.type_name(fd.args[2]).c_str());
    return nullptr;
  }
  size_t pos = mpz_get_ui(INT(fd.args[1])->get());
  std::string &dest = STR(fd.args[0])->get();
  if (pos >= dest.size()) {
    vm.fail(fd.src_id, fd.idx,
            "position %zu is not within string of length %zu", pos,
            dest.size());
    return nullptr;
  }
  std::string &src = STR(fd.args[2])->get();
  if (src.size() == 0)
    return fd.args[0];
  dest[pos] = src[0];
  return fd.args[0];
}

VarBase *str_insert(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected first argument to be of type integer for "
            "string.insert(), found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  if (!fd.args[2]->istype<VarString>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected second argument to be of type string for "
            "string.insert(), found: %s",
            vm.type_name(fd.args[2]).c_str());
    return nullptr;
  }
  size_t pos = mpz_get_ui(INT(fd.args[1])->get());
  std::string &dest = STR(fd.args[0])->get();
  if (pos > dest.size()) {
    vm.fail(fd.src_id, fd.idx, "position %zu is greater than string length %zu",
            pos, dest.size());
    return nullptr;
  }
  std::string &src = STR(fd.args[2])->get();
  dest.insert(dest.begin() + pos, src.begin(), src.end());
  return fd.args[0];
}

VarBase *str_erase(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(
        fd.src_id, fd.idx,
        "expected argument to be of type integer for string.erase(), found: %s",
        vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  size_t pos = mpz_get_ui(INT(fd.args[1])->get());
  std::string &str = STR(fd.args[0])->get();
  if (pos < str.size())
    str.erase(str.begin() + pos);
  return fd.args[0];
}

VarBase *str_find(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarString>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected argument to be of type str for string.find(), found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  std::string &str = STR(fd.args[0])->get();
  std::string &what = STR(fd.args[1])->get();
  size_t pos = str.find(what);
  if (pos == std::string::npos) {
    return make<VarInt>(-1);
  }
  return make<VarInt>(pos);
}

VarBase *str_substr(VMState &vm, const FnData &fd) {
  if (!fd.args[1]->istype<VarInt>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected begin argument to be of type integer for string.erase(), "
            "found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  if (!fd.args[2]->istype<VarInt>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected length argument to be of type integer for "
            "string.erase(), found: %s",
            vm.type_name(fd.args[2]).c_str());
    return nullptr;
  }
  size_t pos = mpz_get_ui(INT(fd.args[1])->get());
  size_t len = mpz_get_ui(INT(fd.args[2])->get());
  std::string &str = STR(fd.args[0])->get();
  return make<VarString>(str.substr(pos, len));
}

VarBase *str_zlength(VMState &vm, const FnData &fd) {
  return make<VarInt>(STR(fd.args[0])->get().size() - 1);
}

VarBase *str_trim(VMState &vm, const FnData &fd) {
  std::string &str = STR(fd.args[0])->get();
  trim(str);
  return fd.args[0];
}

VarBase *str_upper(VMState &vm, const FnData &fd) {
  std::string str = STR(fd.args[0])->get();
  std::for_each(str.begin(), str.end(), [](char &c) { c = ::toupper(c); });
  return make<VarString>(str);
}

VarBase *str_lower(VMState &vm, const FnData &fd) {
  std::string str = STR(fd.args[0])->get();
  std::for_each(str.begin(), str.end(), [](char &c) { c = ::tolower(c); });
  return make<VarString>(str);
}

VarBase *str_split(VMState &vm, const FnData &fd) {
  VarString *str = STR(fd.args[0]);
  if (!fd.args[1]->istype<VarString>()) {
    vm.fail(fd.src_id, fd.idx,
            "expected string argument for delimiter, found: %s",
            vm.type_name(fd.args[1]).c_str());
    return nullptr;
  }
  if (STR(fd.args[1])->get().size() == 0) {
    vm.fail(fd.src_id, fd.idx, "found empty delimiter for String.split()");
    return nullptr;
  }
  std::string delim = STR(fd.args[1])->get();
  std::vector<VarBase *> res_vec =
      _str_split(str->get(), delim, fd.src_id, fd.src_id);
  return make<VarVec>(res_vec, false);
}

// character (str[0]) to its ASCII (int)
VarBase *byt(VMState &vm, const FnData &fd) {
  const std::string &str = STR(fd.args[0])->get();
  if (str.empty())
    return make<VarInt>(0);
  return make<VarInt>(str[0]);
}

// ASCII (int) to character (str)
VarBase *chr(VMState &vm, const FnData &fd) {
  return make<VarString>(
      std::string(1, (char)mpz_get_si(INT(fd.args[0])->get())));
}

std::vector<VarBase *> _str_split(const std::string &data, const std::string &delim,
                                  const size_t &src_id, const size_t &idx) {
  std::vector<VarBase *> vec;
  std::string tmp = data;

  std::string token;

  size_t pos = tmp.find(delim);
  while (pos != std::string::npos) {
    token = tmp.substr(0, pos);
    tmp.erase(0, pos + delim.length());

    vec.push_back(new VarString(token, src_id, idx));
    pos = tmp.find(delim);
  }

  if (tmp.length() > 0) {
    vec.push_back(new VarString(tmp, src_id, idx));
  }

  return vec;
}

// trim from start (in place)
static inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                  [](int ch) { return !std::isspace(ch); }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](int ch) { return !std::isspace(ch); })
              .base(),
          s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
  ltrim(s);
  rtrim(s);
}

#endif // LIBRARY_CORE_STR_HPP