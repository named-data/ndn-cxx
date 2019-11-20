/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 */

#ifndef NDN_IMPL_NAME_COMPONENT_TYPES_HPP
#define NDN_IMPL_NAME_COMPONENT_TYPES_HPP

#include "ndn-cxx/name-component.hpp"
#include "ndn-cxx/util/sha256.hpp"
#include "ndn-cxx/util/string-helper.hpp"

#include <array>
#include <unordered_map>

namespace ndn {
namespace name {
namespace detail {

/** \brief Declare rules for a NameComponent type.
 */
class ComponentType : noncopyable
{
public:
  using Error = Component::Error;

  virtual
  ~ComponentType() = default;

  /** \brief Throw Component::Error if \p comp is invalid.
   */
  virtual void
  check(const Component& comp) const
  {
  }

  /** \brief Calculate the successor of \p comp.
   *
   *  If \p comp is the maximum possible value of this component type, return true to indicate
   *  that the successor should have a greater TLV-TYPE.
   */
  virtual std::pair<bool, Component>
  getSuccessor(const Component& comp) const
  {
    return {false, getSuccessorImpl(comp).second};
  }

  /** \brief Return the minimum allowable TLV-VALUE of this component type.
   */
  virtual const std::vector<uint8_t>&
  getMinValue() const
  {
    static std::vector<uint8_t> value;
    return value;
  }

  /** \brief Return the prefix of the alternate URI representation.
   *
   *  NDN URI specification allows a name component type to declare an alternate URI representation
   *  in the form of `<prefix>=<value>`, in addition to the plain `<type-number>=<escaped-value>`
   *  syntax.
   *
   *  \return the `<prefix>` portion of the alternate URI representation.
   *  \retval nullptr this component does not have an alternate URI representation.
   */
  virtual const char*
  getAltUriPrefix() const
  {
    return nullptr;
  }

  /** \brief Parse component from alternate URI representation.
   *  \param input the `<value>` portion of the alternate URI representation.
   *  \throw Component::Error
   *  \pre getAltUriPrefix() != nullptr
   */
  virtual Component
  parseAltUriValue(const std::string&) const
  {
    NDN_CXX_UNREACHABLE;
  }

  /** \brief Write URI representation of \p comp to \p os.
   *
   *  This base class implementation encodes the component in the plain
   *  `<type-number>=<escaped-value>` syntax.
   */
  virtual void
  writeUri(std::ostream& os, const Component& comp) const
  {
    os << comp.type() << '=';
    writeUriEscapedValue(os, comp);
  }

protected:
  /** \brief Calculate the successor of \p comp, extending TLV-LENGTH if value overflows.
   *  \return whether TLV-LENGTH was extended, and the successor
   */
  std::pair<bool, Block>
  getSuccessorImpl(const Component& comp) const
  {
    EncodingBuffer encoder(comp.size() + 9, 9);
    // leave room for additional byte when TLV-VALUE overflows, and for TLV-LENGTH size increase

    bool isOverflow = true;
    size_t i = comp.value_size();
    for (; isOverflow && i > 0; i--) {
      uint8_t newValue = static_cast<uint8_t>((comp.value()[i - 1] + 1) & 0xFF);
      encoder.prependByte(newValue);
      isOverflow = (newValue == 0);
    }
    encoder.prependByteArray(comp.value(), i);

    if (isOverflow) {
      // new name component has to be extended
      encoder.appendByte(0);
    }

    encoder.prependVarNumber(encoder.size());
    encoder.prependVarNumber(comp.type());
    return {isOverflow, encoder.block()};
  }

  /** \brief Write TLV-VALUE as `<escaped-value>` of NDN URI syntax.
   */
  void
  writeUriEscapedValue(std::ostream& os, const Component& comp) const
  {
    bool isAllPeriods = std::all_of(comp.value_begin(), comp.value_end(),
                                    [] (uint8_t x) { return x == '.'; });
    if (isAllPeriods) {
      os << "...";
    }
    escape(os, reinterpret_cast<const char*>(comp.value()), comp.value_size());
  }
};

/** \brief Rules for GenericNameComponent.
 *
 *  GenericNameComponent has an alternate URI representation that omits the `<type-number>` prefix.
 *  This must be special-cased in the caller, and is not handled by this class.
 */
class GenericNameComponentType final : public ComponentType
{
public:
  void
  writeUri(std::ostream& os, const Component& comp) const final
  {
    writeUriEscapedValue(os, comp);
  }
};

/** \brief Rules for a component type holding a SHA256 digest value, written as
 *         a hex string in URI representation.
 */
class Sha256ComponentType final : public ComponentType
{
public:
  Sha256ComponentType(uint32_t type, const std::string& typeName, const std::string& uriPrefix)
    : m_type(type)
    , m_typeName(typeName)
    , m_uriPrefix(uriPrefix)
  {
  }

  bool
  match(const Component& comp) const
  {
    return comp.type() == m_type && comp.value_size() == util::Sha256::DIGEST_SIZE;
  }

  void
  check(const Component& comp) const final
  {
    BOOST_ASSERT(comp.type() == m_type);
    if (comp.value_size() != util::Sha256::DIGEST_SIZE) {
      NDN_THROW(Error(m_typeName + " TLV-LENGTH must be " + to_string(util::Sha256::DIGEST_SIZE)));
    }
  }

  Component
  create(ConstBufferPtr value) const
  {
    return Component(Block(m_type, std::move(value)));
  }

  Component
  create(const uint8_t* value, size_t valueSize) const
  {
    return Component(makeBinaryBlock(m_type, value, valueSize));
  }

  std::pair<bool, Component>
  getSuccessor(const Component& comp) const final
  {
    bool isExtended = false;
    Block successor;
    std::tie(isExtended, successor) = getSuccessorImpl(comp);
    if (isExtended) {
      return {true, comp};
    }
    return {false, Component(successor)};
  }

  const std::vector<uint8_t>&
  getMinValue() const final
  {
    static std::vector<uint8_t> value(util::Sha256::DIGEST_SIZE);
    return value;
  }

  const char*
  getAltUriPrefix() const final
  {
    return m_uriPrefix.data();
  }

  Component
  parseAltUriValue(const std::string& input) const final
  {
    shared_ptr<Buffer> value;
    try {
      value = fromHex(input);
    }
    catch (const StringHelperError&) {
      NDN_THROW(Error("Cannot convert to " + m_typeName + " (invalid hex encoding)"));
    }
    return Component(m_type, std::move(value));
  }

  void
  writeUri(std::ostream& os, const Component& comp) const final
  {
    os << m_uriPrefix << '=';
    printHex(os, comp.value(), comp.value_size(), false);
  }

private:
  const uint32_t m_type;
  const std::string m_typeName;
  const std::string m_uriPrefix;
};

inline const Sha256ComponentType&
getComponentType1()
{
  static const Sha256ComponentType ct1(tlv::ImplicitSha256DigestComponent,
                                       "ImplicitSha256DigestComponent", "sha256digest");
  return ct1;
}

inline const Sha256ComponentType&
getComponentType2()
{
  static const Sha256ComponentType ct2(tlv::ParametersSha256DigestComponent,
                                       "ParametersSha256DigestComponent", "params-sha256");
  return ct2;
}

/** \brief Rules for a component type holding a nonNegativeInteger value, written as
 *         a decimal number in URI representation.
 */
class DecimalComponentType final : public ComponentType
{
public:
  DecimalComponentType(uint32_t type, const std::string& typeName, const std::string& uriPrefix)
    : m_type(type)
    , m_typeName(typeName)
    , m_uriPrefix(uriPrefix)
  {
  }

  // NOTE:
  // We do not override check() and ensure that the component value is a well-formed
  // nonNegativeInteger, because the application may be using the same typed component
  // with different syntax and semantics.

  const char*
  getAltUriPrefix() const final
  {
    return m_uriPrefix.data();
  }

  Component
  parseAltUriValue(const std::string& input) const final
  {
    uint64_t n = 0;
    try {
      n = std::stoull(input);
    }
    catch (const std::invalid_argument&) {
      NDN_THROW(Error("Cannot convert to " + m_typeName + " (invalid format)"));
    }
    catch (const std::out_of_range&) {
      NDN_THROW(Error("Cannot convert to " + m_typeName + " (out of range)"));
    }
    if (to_string(n) != input) {
      NDN_THROW(Error("Cannot convert to " + m_typeName + " (invalid format)"));
    }
    return Component::fromNumber(n, m_type);
  }

  void
  writeUri(std::ostream& os, const Component& comp) const final
  {
    if (comp.isNumber()) {
      os << m_uriPrefix << '=' << comp.toNumber();
    }
    else {
      ComponentType::writeUri(os, comp);
    }
  }

private:
  const uint32_t m_type;
  const std::string m_typeName;
  const std::string m_uriPrefix;
};

/** \brief Rules regarding NameComponent types.
 */
class ComponentTypeTable : noncopyable
{
public:
  ComponentTypeTable();

  /** \brief Retrieve ComponentType by TLV-TYPE.
   */
  const ComponentType&
  get(uint32_t type) const
  {
    if (type >= m_table.size() || m_table[type] == nullptr) {
      return m_baseType;
    }
    return *m_table[type];
  }

  /** \brief Retrieve ComponentType by alternate URI prefix.
   */
  const ComponentType*
  findByUriPrefix(const std::string& prefix) const
  {
    auto it = m_uriPrefixes.find(prefix);
    if (it == m_uriPrefixes.end()) {
      return nullptr;
    }
    return it->second;
  }

private:
  void
  set(uint32_t type, const ComponentType& ct)
  {
    m_table.at(type) = &ct;
    if (ct.getAltUriPrefix() != nullptr) {
      m_uriPrefixes[ct.getAltUriPrefix()] = &ct;
    }
  }

private:
  const ComponentType m_baseType;
  std::array<const ComponentType*, 38> m_table;
  std::unordered_map<std::string, const ComponentType*> m_uriPrefixes;
};

inline
ComponentTypeTable::ComponentTypeTable()
{
  m_table.fill(nullptr);

  set(tlv::ImplicitSha256DigestComponent, getComponentType1());
  set(tlv::ParametersSha256DigestComponent, getComponentType2());

  static const GenericNameComponentType ct8;
  set(tlv::GenericNameComponent, ct8);

  static const DecimalComponentType ct33(tlv::SegmentNameComponent, "SegmentNameComponent", "seg");
  set(tlv::SegmentNameComponent, ct33);
  static const DecimalComponentType ct34(tlv::ByteOffsetNameComponent, "ByteOffsetNameComponent", "off");
  set(tlv::ByteOffsetNameComponent, ct34);
  static const DecimalComponentType ct35(tlv::VersionNameComponent, "VersionNameComponent", "v");
  set(tlv::VersionNameComponent, ct35);
  static const DecimalComponentType ct36(tlv::TimestampNameComponent, "TimestampNameComponent", "t");
  set(tlv::TimestampNameComponent, ct36);
  static const DecimalComponentType ct37(tlv::SequenceNumNameComponent, "SequenceNumNameComponent", "seq");
  set(tlv::SequenceNumNameComponent, ct37);
}

/** \brief Get the global ComponentTypeTable.
 */
inline const ComponentTypeTable&
getComponentTypeTable()
{
  static ComponentTypeTable ctt;
  return ctt;
}

} // namespace detail
} // namespace name
} // namespace ndn

#endif // NDN_IMPL_NAME_COMPONENT_TYPES_HPP
