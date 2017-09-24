/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#ifndef NDN_DATA_HPP
#define NDN_DATA_HPP

#include "meta-info.hpp"
#include "name.hpp"
#include "packet-base.hpp"
#include "signature.hpp"
#include "encoding/block.hpp"

namespace ndn {

/** @brief Represents a Data packet
 */
class Data : public PacketBase, public enable_shared_from_this<Data>
{
public:
  class Error : public tlv::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : tlv::Error(what)
    {
    }
  };

  /** @brief Create a new Data with the given name and empty Content
   *  @warning In certain contexts that use Data::shared_from_this(), Data must be created
   *           using `make_shared`. Otherwise, .shared_from_this() will trigger undefined behavior.
   */
  explicit
  Data(const Name& name = Name());

  /** @brief Create from wire encoding
   *  @warning In certain contexts that use Data::shared_from_this(), Data must be created
   *           using `make_shared`. Otherwise, .shared_from_this() will trigger undefined behavior.
   */
  explicit
  Data(const Block& wire);

  /** @brief Fast encoding or block size estimation
   *  @param encoder EncodingEstimator or EncodingBuffer instance
   *  @param wantUnsignedPortionOnly If true, only prepends Name, MetaInfo, Content, and
   *         SignatureInfo to @p encoder, but omit SignatureValue and outmost Type-Length of Data
   *         element. This is intended to be used with wireEncode(encoder, signatureValue).
   *  @throw Error SignatureBits are not provided and wantUnsignedPortionOnly is false.
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder, bool wantUnsignedPortionOnly = false) const;

  /** @brief Finalize Data packet encoding with the specified SignatureValue
   *  @param encoder EncodingBuffer containing Name, MetaInfo, Content, and SignatureInfo, but
   *                 without SignatureValue or outmost Type-Length of Data element
   *  @param signatureValue SignatureValue element
   *
   *  This method is intended to be used in concert with Data::wireEncode(encoder, true)
   *  @code
   *     Data data;
   *     ...
   *     EncodingBuffer encoder;
   *     data.wireEncode(encoder, true);
   *     ...
   *     Block signatureValue = <sign_over_unsigned_portion>(encoder.buf(), encoder.size());
   *     data.wireEncode(encoder, signatureValue)
   *  @endcode
   */
  const Block&
  wireEncode(EncodingBuffer& encoder, const Block& signatureValue) const;

  /** @brief Encode to a wire format
   */
  const Block&
  wireEncode() const;

  /** @brief Decode from the wire format
   */
  void
  wireDecode(const Block& wire);

  /** @brief Check if already has wire
   */
  bool
  hasWire() const
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
  getName() const
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
  getMetaInfo() const
  {
    return m_metaInfo;
  }

  /** @brief Set MetaInfo
   *  @return a reference to this Data, to allow chaining
   */
  Data&
  setMetaInfo(const MetaInfo& metaInfo);

  /** @brief Get Content
   *
   *  The Content value is accessible through value()/value_size() or value_begin()/value_end()
   *  methods of the Block class.
   */
  const Block&
  getContent() const;

  /** @brief Set Content from a block
   *
   *  If block's TLV-TYPE is Content, it will be used directly as Data's Content element.
   *  If block's TLV-TYPE is not Content, it will be nested into a Content element.
   *
   *  @return a reference to this Data, to allow chaining
   */
  Data&
  setContent(const Block& block);

  /** @brief Copy Content value from raw buffer
   *  @param value pointer to the first octet of the value
   *  @param valueSize size of the raw buffer
   *  @return a reference to this Data, to allow chaining
   */
  Data&
  setContent(const uint8_t* value, size_t valueSize);

  /** @brief Set Content from wire buffer
   *  @param value Content value, which does not need to be a TLV element
   *  @return a reference to this Data, to allow chaining
   */
  Data&
  setContent(const ConstBufferPtr& value);

  /** @brief Get Signature
   */
  const Signature&
  getSignature() const
  {
    return m_signature;
  }

  /** @brief Set Signature
   *  @return a reference to this Data, to allow chaining
   */
  Data&
  setSignature(const Signature& signature);

  /** @brief Set SignatureValue
   *  @return a reference to this Data, to allow chaining
   */
  Data&
  setSignatureValue(const Block& value);

public: // MetaInfo fields
  uint32_t
  getContentType() const
  {
    return m_metaInfo.getType();
  }

  Data&
  setContentType(uint32_t type);

  const time::milliseconds&
  getFreshnessPeriod() const
  {
    return m_metaInfo.getFreshnessPeriod();
  }

  Data&
  setFreshnessPeriod(const time::milliseconds& freshnessPeriod);

  const name::Component&
  getFinalBlockId() const
  {
    return m_metaInfo.getFinalBlockId();
  }

  Data&
  setFinalBlockId(const name::Component& finalBlockId);

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
  Signature m_signature;

  mutable Block m_wire;
  mutable Name m_fullName; ///< cached FullName computed from m_wire
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

#endif // NDN_DATA_HPP
