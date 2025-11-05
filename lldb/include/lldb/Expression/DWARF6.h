//===-- DWARF6.h ----------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
#include "lldb/Core/Value.h"
#include "lldb/Utility/Scalar.h"
#include "lldb/lldb-enumerations.h"
#include "lldb/lldb-types.h"
#include <cassert>
#include <cctype>
#include <cstdint>

namespace lldb_private {
class DWARFLocation {
public:
  enum class StorageKind { Memory, Register, Implicit, Undefined, Composite };
  using AddressSpace = int;

  DWARFLocation() = default;
  static DWARFLocation Memory(uint64_t bit_offset, AddressSpace address_space) {
    return DWARFLocation(bit_offset, address_space);
  }
  static DWARFLocation Register(lldb::RegisterKind reg_kind,
                                lldb::regnum64_t reg_num) {
    return DWARFLocation(reg_kind, reg_num);
  }

  DWARFLocation(StorageKind kind) : DWARFLocation(kind, 0) {}

  DWARFLocation(StorageKind kind, uint64_t bit_offset)
      : m_kind(kind), m_bit_offset(bit_offset) {}

  DWARFLocation(StorageKind kind, uint64_t bit_offset,
                AddressSpace address_space, lldb::RegisterKind reg_kind,
                lldb::regnum64_t reg_num)
      : m_kind(kind), m_bit_offset(bit_offset), m_address_space(address_space),
        m_reg_kind(reg_kind), m_reg_num(reg_num) {}

  uint64_t GetBitOffset() { return m_bit_offset; }

  AddressSpace GetAddressSpace() {
    assert(m_kind == StorageKind::Memory);
    return m_address_space;
  }

  void GetRegisterInfo(lldb::RegisterKind &reg_kind,
                       lldb::regnum64_t &reg_num) {
    assert(m_kind == StorageKind::Register);
    reg_kind = m_reg_kind;
    reg_num = m_reg_num;
  }

private:
  StorageKind m_kind = StorageKind::Undefined;
  uint64_t m_bit_offset = 0;
  AddressSpace m_address_space = 0;
  lldb::RegisterKind m_reg_kind = lldb::RegisterKind::eRegisterKindDWARF;
  lldb::regnum64_t m_reg_num = LLDB_INVALID_REGNUM;

  DWARFLocation(lldb::RegisterKind reg_kind, lldb::regnum64_t reg_num)
      : m_kind(StorageKind::Register), m_reg_kind(reg_kind),
        m_reg_num(reg_num) {}
  DWARFLocation(uint64_t bit_offset, AddressSpace address_space)
      : m_kind(StorageKind::Memory), m_bit_offset(bit_offset),
        m_address_space(address_space) {}
};

class DWARFExpressionStackElement {
public:
  DWARFExpressionStackElement(Value type)
      : m_type(ElementType::Value), m_value(type) {}
  DWARFExpressionStackElement(DWARFLocation loc)
      : m_type(ElementType::Location), m_location(loc) {}

  // Implicit conversion from Scalar - constructs a Value from the Scalar
  DWARFExpressionStackElement(const Scalar &scalar)
      : m_type(ElementType::Value), m_value(scalar) {}

  // Forwards from Value class.
  const Scalar &GetScalar() const { return GetValue().GetScalar(); }

  Scalar &GetScalar() { return GetValue().GetScalar(); }

  Value::ValueType GetValueType() const { return GetValue().GetValueType(); };

  void ClearContext() { GetValue().ClearContext(); }

  void SetValueType(Value::ValueType value_type) {
    GetValue().SetValueType(value_type);
  }

  Scalar &ResolveValue(ExecutionContext *exe_ctx, Module *module = nullptr) {
    return GetValue().ResolveValue(exe_ctx, module);
  }

  void Dump(Stream *strm) { GetValue().Dump(strm); }

  operator Value() { return GetValue(); }

  Value &GetAsValue() { return GetValue(); }

private:
  enum class ElementType { Value, Location };
  ElementType m_type;
  Value m_value;
  DWARFLocation m_location;

  Value &GetValue() {
    assert(m_type == ElementType::Value);
    return m_value;
  }
  const Value &GetValue() const {
    assert(m_type == ElementType::Value);
    return m_value;
  }
};
} // namespace lldb_private
