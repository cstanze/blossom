

#ifndef VM_CONSTS_HPP
#define VM_CONSTS_HPP

#include "OpCodes.hpp"
#include "VM.hpp"
#include "Vars/Base.hpp"

namespace blossom {
namespace consts {
VarBase *get(VMState &vm, const OpDataType type, const OpData &opd,
             const size_t &src_id, const size_t &idx);
}
} // namespace blossom

#endif // VM_CONSTS_HPP
