/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#ifndef NDN_CXX_DATA_HPP
#define NDN_CXX_DATA_HPP

#include "ndn-cxx/detail/packet-base.hpp"
#include "ndn-cxx/encoding/block.hpp"
#include "ndn-cxx/meta-info.hpp"
#include "ndn-cxx/name.hpp"
#include "ndn-cxx/security/security-common.hpp"
#include "ndn-cxx/signature-info.hpp"

namespace ndn {

/** @brief Represents a %Data packet.
 *  @sa https://named-data.net/doc/NDN-packet-spec/0.3/data.html
 */
class Data : public PacketBase, public std::enable_shared_from_this<Data>
{
public:
  class Error : public tlv::Error
  {
  public:
    using tlv::Error::Error;
  };

  /** @brief Construct an unsigned Data packet with given @p name and empty Content.
   *  @warning In certain contexts that use `Data::shared_from_this()`, Data must be created using
   *           `std::make_shared`. Otherwise, `shared_from_this()` may trigger undefined behavior.
   *           One example where this is necessary is storing Data into a subclass of InMemoryStorage.
   */
  explicit
  Data(const Name& name = Name());

  /** @brief Construct a Data packet by decoding from @p wire.
   *  @param wire TLV block of type tlv::Data; may be signed or unsigned.
   *  @warning In certain contexts that use `Data::shared_from_this()`, Data must be created using
   *           `std::make_shared`. Otherwise, `shared_from_this()` may trigger undefined behavior.
   *           One example where this is necessary is storing Data into a subclass of InMemoryStorage.
   */
  explicit
  Data(const Block& wire);

  /**
   * @brief Prepend wire encoding to @p encoder.
   * @param encoder EncodingEstimator or EncodingBuffer instance.
   * @param wantUnsignedPortionOnly If true, prepend only Name, MetaInfo, Content, and
   *        SignatureInfo to @p encoder, but omit SignatureValue and the outermost TLV
   *        Type and Length of the %Data element. This is intended to be used with
   *        wireEncode(EncodingBuffer&, span<const uint8_t>) const.
   * @throw Error Signature is not present and @p wantUnsignedPortionOnly is false.
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder, bool wantUnsignedPortionOnly = false) const;

  /**
   * @brief Finalize Data packet encoding with the specified signature.
   * @param encoder EncodingBuffer containing Name, MetaInfo, Content, and SignatureInfo, but
   *                without SignatureValue and the outermost Type-Length of the %Data element.
   * @param signature Raw signature bytes, without TLV Type and Length; this will become the
   *                  TLV-VALUE of the SignatureValue element added to the packet.
   *
   * This method is intended to be used in concert with `wireEncode(encoder, true)`, e.g.:
   * @code
   * Data data;
   * ...
   * EncodingBuffer encoder;
   * data.wireEncode(encoder, true);
   * ...
   * auto signature = create_signature_over_signed_portion(encoder.data(), encoder.size());
   * data.wireEncode(encoder, signature);
   * @endcode
   */
  const Block&
  wireEncode(EncodingBuffer& encoder, span<const uint8_t> signature) const;

  /** @brief Encode into a Block.
   *  @pre Data must be signed.
   */
  const Block&
  wireEncode() const;

  /** @brief Decode from @p wire.
   */
  void
  wireDecode(const Block& wire);

  /** @brief Check if this instance has cached wire encoding.
   */
  bool
  hasWire() const noexcept
  {
    return m_wire.hasWire();
  }

  /** @brief Get full name including implicit digest
   *  @pre hasWire() == true; i.e. wireEncode() must have been called
   *  @throw Error Data has no wire encoding
   */
  const Name&
  getFullName() const;

public: // Data fields
  /** @brief Get name
   */
  const Name&
  getName() const noexcept
  {
    return m_name;
  }

  /** @brief Set name
   *  @return a reference to this Data, to allow chaining
   */
  Data&
  setName(const Name& name);

  /** @brief Get MetaInfo
   */
  const MetaInfo&
  getMetaInfo() const noexcept
  {
    return m_metaInfo;
  }

  /** @brief Set MetaInfo
   *  @return a reference to this Data, to allow chaining
   */
  Data&
  setMetaInfo(const MetaInfo& metaInfo);

  /**
   * @brief Return whether this Data has a Content element
   */
  bool
  hasContent() const noexcept
  {
    return m_content.isValid();
  }

  /**
   * @brief Get the Content element
   *
   * If the element is not present (hasContent() == false), an invalid Block will be returned.
   *
   * The value of the returned Content Block (if valid) can be accessed through
   *   - Block::value_bytes(), or
   *   - Block::value() and Block::value_size(), or
   *   - Block::value_begin() and Block::value_end().
   *
   * @sa hasContent()
   * @sa Block::value_bytes(), Block::blockFromValue(), Block::parse()
   */
  const Block&
  getContent() const noexcept
  {
    return m_content;
  }

  /**
   * @brief Set Content from a Block
   * @param block TLV block to be used as Content; must be valid
   * @return a reference to this Data, to allow chaining
   *
   * If the block's TLV-TYPE is tlv::Content, it will be used directly as this Data's
   * Content element. Otherwise, the block will be nested into a Content element.
   */
  Data&
  setContent(const Block& block);

  /**
   * @brief Set Content by copying from a contiguous sequence of bytes
   * @param value buffer with the TLV-VALUE of the content
   * @return a reference to this Data, to allow chaining
   */
  Data&
  setContent(span<const uint8_t> value);

  /**
   * @brief Set Content by copying from a raw buffer
   * @param value buffer with the TLV-VALUE of the content; may be nullptr if @p length is zero
   * @param length size of the buffer
   * @return a reference to this Data, to allow chaining
   * @deprecated Use setContent(span<const uint8_t>)
   */
  [[deprecated("use the overload that takes a span<>")]]
  Data&
  setContent(const uint8_t* value, size_t length);

  /**
   * @brief Set Content from a shared buffer
   * @param value buffer with the TLV-VALUE of the content; must not be nullptr
   * @return a reference to this Data, to allow chaining
   */
  Data&
  setContent(ConstBufferPtr value);

  /**
   * @brief Remove the Content element
   * @return a reference to this Data, to allow chaining
   * @post hasContent() == false
   */
  Data&
  unsetContent();

  /** @brief Get SignatureInfo
   */
  const SignatureInfo&
  getSignatureInfo() const noexcept
  {
    return m_signatureInfo;
  }

  /** @brief Set SignatureInfo
   *
   *  This is a low-level function that should not normally be called directly by applications.
   *  Instead, provide a SignatureInfo to the SigningInfo object passed to KeyChain::sign().
   *
   *  @return a reference to this Data, to allow chaining
   *  @warning SignatureInfo is overwritten when the packet is signed via KeyChain::sign().
   *  @sa SigningInfo
   */
  Data&
  setSignatureInfo(const SignatureInfo& info);

  /** @brief Get SignatureValue
   */
  const Block&
  getSignatureValue() const noexcept
  {
    return m_signatureValue;
  }

  /** @brief Set SignatureValue
   *  @param value buffer containing the TLV-VALUE of the SignatureValue; must not be nullptr
   *
   *  This is a low-level function that should not normally be called directly by applications.
   *  Instead, use KeyChain::sign() to sign the packet.
   *
   *  @return a reference to this Data, to allow chaining
   *  @warning SignatureValue is overwritten when the packet is signed via KeyChain::sign().
   */
  Data&
  setSignatureValue(ConstBufferPtr value);

  /** @brief Extract ranges of Data covered by the signature
   *  @throw Error Data cannot be encoded or is missing ranges necessary for signing
   *  @warning The returned pointers will be invalidated if wireDecode() or wireEncode() are called.
   */
  InputBuffers
  extractSignedRanges() const;

public: // MetaInfo fields
  uint32_t
  getContentType() const
  {
    return m_metaInfo.getType();
  }

  Data&
  setContentType(uint32_t type);

  time::milliseconds
  getFreshnessPeriod() const
  {
    return m_metaInfo.getFreshnessPeriod();
  }

  Data&
  setFreshnessPeriod(time::milliseconds freshnessPeriod);

  const optional<name::Component>&
  getFinalBlock() const
  {
    return m_metaInfo.getFinalBlock();
  }

  Data&
  setFinalBlock(optional<name::Component> finalBlockId);

public: // SignatureInfo fields
  /** @brief Get SignatureType
   *  @return tlv::SignatureTypeValue, or -1 to indicate the signature is invalid
   */
  int32_t
  getSignatureType() const noexcept
  {
    return m_signatureInfo.getSignatureType();
  }

  /** @brief Get KeyLocator
   */
  optional<KeyLocator>
  getKeyLocator() const noexcept
  {
    return m_signatureInfo.hasKeyLocator() ? make_optional(m_signatureInfo.getKeyLocator()) : nullopt;
  }

protected:
  /** @brief Clear wire encoding and cached FullName
   *  @note This does not clear the SignatureValue.
   */
  void
  resetWire();

private:
  Name m_name;
  MetaInfo m_metaInfo;
  Block m_content;
  SignatureInfo m_signatureInfo;
  Block m_signatureValue;

  mutable Block m_wire;
  mutable Name m_fullName; // cached FullName computed from m_wire
};

#ifndef DOXYGEN
extern template size_t
Data::wireEncode<encoding::EncoderTag>(EncodingBuffer&, bool) const;

extern template size_t
Data::wireEncode<encoding::EstimatorTag>(EncodingEstimator&, bool) const;
#endif

std::ostream&
operator<<(std::ostream& os, const Data& data);

bool
operator==(const Data& lhs, const Data& rhs);

inline bool
operator!=(const Data& lhs, const Data& rhs)
{
  return !(lhs == rhs);
}

} // namespace ndn

#endif // NDN_CXX_DATA_HPP
