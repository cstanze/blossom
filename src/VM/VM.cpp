

#include "VM/VM.hpp"

#include <cstdarg>
#include <cstdlib>
#include <string>

#include "Common/Env.hpp"
#include "Common/FS.hpp"
#include "Common/String.hpp"
#include "VM/Vars.hpp"

// env: BLOSSOM_PATHS
VMState::VMState(const std::string &self_bin, const std::string &self_base,
                 const std::vector<std::string> &args, const size_t &flags,
                 const bool &is_thread_copy)
    : exit_called(false), exec_stack_count_exceeded(false), exit_code(0),
      exec_flags(flags), exec_stack_max(EXEC_STACK_MAX_DEFAULT),
      exec_stack_count(0), tru(new VarBool(true, 0, 0)),
      fals(new VarBool(false, 0, 0)), nil(new VarNil(0, 0)),
      vm_stack(new VMStack()), dlib(is_thread_copy ? nullptr : new DynLib()),
      src_args(nullptr), m_self_bin(self_bin), m_self_base(self_base),
      m_src_load_fn(nullptr), m_src_read_code_fn(nullptr),
      m_is_thread_copy(is_thread_copy) {
  if (m_is_thread_copy)
    return;

  init_typenames(*this);

  std::vector<VarBase *> src_args_vec;

  for (auto &arg : args) {
    src_args_vec.push_back(new VarString(arg, 0, 0));
  }
  src_args = new VarVec(src_args_vec, false, 0, 0);

  std::vector<std::string> extra_search_paths =
      String::split(Env::get("BLOSSOM_PATHS"), ';');

  for (auto &path : extra_search_paths) {
    m_inc_locs.push_back(path + "/include/blossom");
    m_dll_locs.push_back(path + "/lib/blossom");
  }

  m_inc_locs.push_back(m_self_base + "/include/blossom");
  m_dll_locs.push_back(m_self_base + "/lib/blossom");
}

VMState::~VMState() {
  delete vm_stack;
  if (!m_is_thread_copy)
    for (auto &typefn : m_typefns)
      delete typefn.second;
  for (auto &g : m_globals)
    var_dref(g.second);
  for (auto &src : all_srcs)
    var_dref(src.second);
  var_dref(nil);
  var_dref(fals);
  var_dref(tru);
  var_dref(src_args);
  for (auto &deinit_fn : m_dll_deinit_fns) {
    deinit_fn.second();
  }
  if (m_is_thread_copy)
    return;
  delete dlib;
}

void VMState::push_src(SrcFile *src, const size_t &idx) {
  if (all_srcs.find(src->path()) == all_srcs.end()) {
    all_srcs[src->path()] = new VarSrc(src, new Vars(), src->id(), idx);
  }
  var_iref(all_srcs[src->path()]);
  src_stack.push_back(all_srcs[src->path()]);
}

void VMState::push_src(const std::string &src_path) {
  assert(all_srcs.find(src_path) != all_srcs.end());
  var_iref(all_srcs[src_path]);
  src_stack.push_back(all_srcs[src_path]);
}

void VMState::pop_src() {
  var_dref(src_stack.back());
  src_stack.pop_back();
}

void VMState::add_typefn(const std::uintptr_t &type, const std::string &name,
                         VarBase *fn, const bool iref) {
  if (m_typefns.find(type) == m_typefns.end()) {
    m_typefns[type] = new VarsFrame();
  }
  if (m_typefns[type]->exists(name)) {
    fprintf(stderr, "function '%s' for type '%s' already exists\n",
            name.c_str(), type_name(type).c_str());
    assert(false);
    return;
  }
  m_typefns[type]->add(name, fn, iref);
}
VarBase *VMState::get_typefn(VarBase *var, const std::string &name) {
  auto it = m_typefns.find(var->typefn_id());
  VarBase *res = nullptr;
  if (it == m_typefns.end()) {
    if (var->attr_based())
      goto attr_based;
    return m_typefns[type_id<VarAll>()]->get(name);
  }
  res = it->second->get(name);
  if (res)
    return res;
  return m_typefns[type_id<VarAll>()]->get(name);
attr_based:
  it = m_typefns.find(var->type());
  if (it == m_typefns.end())
    return m_typefns[type_id<VarAll>()]->get(name);
  res = it->second->get(name);
  if (res)
    return res;
  return m_typefns[type_id<VarAll>()]->get(name);
}

void VMState::set_typename(const std::uintptr_t &type,
                           const std::string &name) {
  m_typenames[type] = name;
}
std::string VMState::type_name(const std::uintptr_t &type) {
  if (m_typenames.find(type) != m_typenames.end()) {
    return m_typenames[type];
  }
  return "typeid<" + std::to_string(type) + ">";
}
std::string VMState::type_name(const VarBase *val) {
  return type_name(val->type());
}
void VMState::gadd(const std::string &name, VarBase *val, const bool iref) {
  if (m_globals.find(name) != m_globals.end())
    return;
  if (iref)
    var_iref(val);
  m_globals[name] = val;
}

VarBase *VMState::gget(const std::string &name) {
  if (m_globals.find(name) == m_globals.end())
    return nullptr;
  return m_globals[name];
}

bool VMState::mod_exists(const std::vector<std::string> &locs, std::string &mod,
                         const std::string &ext, std::string &dir) {
  if (mod.front() != '~' && mod.front() != '/' && mod.front() != '.') {
    for (auto &loc : locs) {
      if (FS::exists(loc + "/" + mod + ext)) {
        mod = FS::absPath(loc + "/" + mod + ext, &dir);
        return true;
      }
    }
  } else {
    if (mod.front() == '~') {
      mod.erase(mod.begin());
      std::string home = Env::get("HOME");
      mod.insert(mod.begin(), home.begin(), home.end());
    } else if (mod.front() == '.') {
      // cannot have a module exists query with '.' outside all srcs
      assert(src_stack.size() > 0);
      mod.erase(mod.begin());
      mod = src_stack.back()->src()->dir() + mod;
    }
    if (FS::exists(mod + ext)) {
      mod = FS::absPath(mod + ext, &dir);
      return true;
    }
  }
  return false;
}

bool VMState::nmod_load(const std::string &mod_str, const size_t &src_id,
                        const size_t &idx) {
  std::string mod = mod_str.substr(mod_str.find_last_of('/') + 1);
  std::string mod_file = mod_str;
  std::string mod_dir;
  mod_file.insert(mod_file.find_last_of('/') + 1, "libblossom");
  if (!mod_exists(m_dll_locs, mod_file, nmod_ext(), mod_dir)) {
    fail(src_id, idx, "module file: %s not found in locations: %s",
         (mod_file + nmod_ext()).c_str(),
         String::stringify(m_dll_locs).c_str());
    return false;
  }

  if (dlib->fexists(mod_file))
    return true;

  if (!dlib->load(mod_file)) {
    fail(src_id, idx, "unable to load module file: %s", mod_file.c_str(),
         String::stringify(m_dll_locs).c_str());
    return false;
  }
  ModInitFn init_fn = (ModInitFn)dlib->get(mod_file, "init_" + mod);
  if (init_fn == nullptr) {
    fail(src_id, idx, "module file: %s does not contain init function (%s)",
         mod_file.c_str(), ("init_" + mod).c_str());
    dlib->unload(mod_file);
    return false;
  }
  if (!init_fn(*this, src_id, idx)) {
    dlib->unload(mod_file);
    fail(src_id, idx, "init function in module file: %s didn't return okay",
         mod_file.c_str());
    return false;
  }
  // set deinit function if available
  ModDeInitFn deinit_fn = (ModDeInitFn)dlib->get(mod_file, "deinit_" + mod);
  if (deinit_fn)
    m_dll_deinit_fns[mod_file] = deinit_fn;
  return true;
}

// updated mod_str with actual file name (full canonical path)
int VMState::bmod_load(std::string &mod_file, const size_t &src_id,
                       const size_t &idx) {
  std::string mod_dir;
  if (!mod_exists(m_inc_locs, mod_file, bmod_ext(), mod_dir)) {
    fail(src_id, idx, "import file: %s not found in locations: %s",
         (mod_file + bmod_ext()).c_str(),
         String::stringify(m_inc_locs).c_str());
    return E_FAIL;
  }
  if (all_srcs.find(mod_file) != all_srcs.end())
    return E_OK;

  Errors err = E_OK;
  SrcFile *src =
      m_src_load_fn(mod_file, mod_dir, exec_flags, false, err, 0, -1);
  if (err != E_OK) {
    if (src)
      delete src;
    return err;
  }

  push_src(src, 0);
  int res = vm::exec(*this);
  pop_src();
  return res;
}

void VMState::fail(const size_t &src_id, const size_t &idx, const char *msg,
                   ...) {
  va_list vargs;
  va_start(vargs, msg);
  if (fails.empty() || this->exit_called) {
    for (auto &src : all_srcs) {
      if (src.second->src()->id() == src_id) {
        src.second->src()->fail(idx, msg, vargs);
        break;
      }
    }
  } else {
    static char err[4096];
    vsprintf(err, msg, vargs);
    fails.push(new VarString(err, src_id, idx), false);
  }
  va_end(vargs);
}

void VMState::fail(const size_t &src_id, const size_t &idx, VarBase *val,
                   const char *msg, const bool &iref) {
  if (iref)
    var_iref(val);

  if (fails.empty() || this->exit_called) {
    for (auto &src : all_srcs) {
      if (src.second->src()->id() == src_id) {
        std::string data;
        val->to_str(*this, data, src_id, idx);
        var_dref(val);
        if (msg)
          src.second->src()->fail(idx, "%s (%s)", msg, data.c_str());
        else
          src.second->src()->fail(idx, data.c_str());
        break;
      }
    }
  } else {
    fails.push(val, false);
  }
}

bool VMState::load_core_mods() {
  std::vector<std::string> mods = {"core", "utils"};
  for (auto &mod : mods) {
    if (!nmod_load(mod, 0, 0))
      return false;
  }
  return true;
}

VMState *VMState::thread_copy(const size_t &src_id, const size_t &idx) {
  VMState *vm = new VMState(m_self_bin, m_self_base, {}, exec_flags, true);
  for (auto &s : all_srcs) {
    vm->all_srcs[s.first] =
        static_cast<VarSrc *>(s.second->thread_copy(src_id, idx));
  }
  for (auto &s : src_stack) {
    vm->src_stack.push_back(vm->all_srcs[s->src()->path()]);
  }
  vm->dlib = dlib; // don't delete in destructor
  var_iref(src_args);
  vm->src_args = src_args;
  vm->m_src_load_fn = m_src_load_fn;
  vm->m_src_read_code_fn = m_src_read_code_fn;
  vm->m_inc_locs = m_inc_locs;
  vm->m_dll_locs = m_dll_locs;
  vm->m_globals = m_globals;
  for (auto &glob : vm->m_globals) {
    var_iref(glob.second);
  }
  vm->m_typefns = m_typefns; // do not delete in destructor
  vm->m_typenames = m_typenames;
  // don't copy m_dll_deinit_fns as that will be called by the main thread
  return vm;
}

const char *nmod_ext() {
#if __linux__ || __FreeBSD__ || __NetBSD__ || __OpenBSD__ || __bsdi__ ||       \
    __DragonFly__
  return ".so";
#elif __APPLE__
  return ".dylib";
#endif
}

const char *bmod_ext(const bool compiled) {
  if (compiled)
    return ".cbl";
  return ".bls";
}