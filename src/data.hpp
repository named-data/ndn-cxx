/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#include "common.hpp"
#include "name.hpp"
#include "encoding/block.hpp"

#include "signature.hpp"
#include "meta-info.hpp"
#include "key-locator.hpp"
#include "management/nfd-local-control-header.hpp"

namespace ndn {

class Data : public enable_shared_from_this<Data>
{
public:
  class Error : public std::runtime_error
  {
  public:
    explicit
    Error(const std::string& what)
      : std::runtime_error(what)
    {
    }
  };

  /**
   * @brief Create an empty Data object
   *
   * Note that in certain contexts that use Data::shared_from_this(), Data must be
   * created using `make_shared`:
   *
   *     shared_ptr<Data> data = make_shared<Data>();
   *
   * Otherwise, Data::shared_from_this() will throw an exception.
   */
  Data();

  /**
   * @brief Create a new Data object with the given name
   *
   * @param name A reference to the name
   *
   * Note that in certain contexts that use Data::shared_from_this(), Data must be
   * created using `make_shared`:
   *
   *     shared_ptr<Data> data = make_shared<Data>(name);
   *
   * Otherwise, Data::shared_from_this() will throw an exception.
   */
  Data(const Name& name);

  /**
   * @brief Create a new Data object from wire encoding
   *
   * Note that in certain contexts that use Data::shared_from_this(), Data must be
   * created using `make_shared`:
   *
   *     shared_ptr<Data> data = make_shared<Data>(wire);
   *
   * Otherwise, Data::shared_from_this() will throw an exception.
   */
  explicit
  Data(const Block& wire)
  {
    wireDecode(wire);
  }

  /**
   * @brief Fast encoding or block size estimation
   */
  template<bool T>
  size_t
  wireEncode(EncodingImpl<T>& block, bool unsignedPortion = false) const;

  /**
   * @brief Encode to a wire format
   */
  const Block&
  wireEncode() const;

  /**
   * @brief Decode from the wire format
   */
  void
  wireDecode(const Block& wire);

  /**
   * @brief Check if Data is already has wire encoding
   */
  bool
  hasWire() const;

  ////////////////////////////////////////////////////////////////////

  /**
   * @brief Get name of the Data packet
   */
  const Name&
  getName() const;

  /**
   * @brief Set name to a copy of the given Name
   *
   * @return This Data so that you can chain calls to update values
   */
  Data&
  setName(const Name& name);

  //

  /**
   * @brief Get MetaInfo block from Data packet
   */
  const MetaInfo&
  getMetaInfo() const;

  /**
   * @brief Set metaInfo to a copy of the given MetaInfo
   *
   * @return This Data so that you can chain calls to update values.
   */
  Data&
  setMetaInfo(const MetaInfo& metaInfo);

  //

  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  // MetaInfo proxy methods

  uint32_t
  getContentType() const;

  Data&
  setContentType(uint32_t type);

  //

  const time::milliseconds&
  getFreshnessPeriod() const;

  Data&
  setFreshnessPeriod(const time::milliseconds& freshnessPeriod);

  //

  const name::Component&
  getFinalBlockId() const;

  Data&
  setFinalBlockId(const name::Component& finalBlockId);

  //
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////

  /**
   * @brief Get content Block
   *
   * To access content value, one can use value()/value_size() or
   * value_begin()/value_end() methods of the Block class
   */
  const Block&
  getContent() const;

  /**
   * @brief Set the content from the buffer (buffer will be copied)
   *
   * @param buffer Pointer to first byte of the buffer
   * @param bufferSize Size of the buffer
   *
   * @return This Data so that you can chain calls to update values.
   */
  Data&
  setContent(const uint8_t* buffer, size_t bufferSize);

  /**
   * @brief Set the content from the block
   *
   * Depending on type of the supplied block, there are two cases:
   *
   * - if block.type() == Tlv::Content, then block will be used directly as Data packet's
   *   content (no extra copying)
   *
   * - if block.type() != Tlv::Content, then this method will create a new Block with type
   *   Tlv::Content and put block as a nested element in the content Block.
   *
   * @param block The Block containing the content to assign
   *
   * @return This Data so that you can chain calls to update values.
   */
  Data&
  setContent(const Block& block);

  /**
   * @brief Set the content from the pointer to immutable buffer
   *
   * This method will create a Block with Tlv::Content and set contentValue as a payload
   * for this block.  Note that this method is very different from setContent(const
   * Block&), since it does not require that payload should be a valid TLV element.
   *
   * @param contentValue The pointer to immutable buffer containing the content to assign
   *
   * @return This Data so that you can chain calls to update values.
   */
  Data&
  setContent(const ConstBufferPtr& contentValue);

  //

  const Signature&
  getSignature() const;

  /**
   * @brief Set the signature to a copy of the given signature.
   * @param signature The signature object which is cloned.
   */
  Data&
  setSignature(const Signature& signature);

  Data&
  setSignatureValue(const Block& value);

  ///////////////////////////////////////////////////////////////

  nfd::LocalControlHeader&
  getLocalControlHeader();

  const nfd::LocalControlHeader&
  getLocalControlHeader() const;

  uint64_t
  getIncomingFaceId() const;

  Data&
  setIncomingFaceId(uint64_t incomingFaceId);

public: // EqualityComparable concept
  bool
  operator==(const Data& other) const;

  bool
  operator!=(const Data& other) const;

private:
  /**
   * @brief Clear the wire encoding.
   */
  void
  onChanged();

private:
  Name m_name;
  MetaInfo m_metaInfo;
  mutable Block m_content;
  Signature m_signature;

  mutable Block m_wire;

  nfd::LocalControlHeader m_localControlHeader;
  friend class nfd::LocalControlHeader;
};

inline
Data::Data()
  : m_content(Tlv::Content) // empty content
{
}

inline
Data::Data(const Name& name)
  : m_name(name)
{
}

template<bool T>
inline size_t
Data::wireEncode(EncodingImpl<T>& block, bool unsignedPortion/* = false*/) const
{
  size_t totalLength = 0;

  // Data ::= DATA-TLV TLV-LENGTH
  //            Name
  //            MetaInfo
  //            Content
  //            Signature

  // (reverse encoding)

  if (!unsignedPortion && !m_signature)
    {
      throw Error("Requested wire format, but data packet has not been signed yet");
    }

  if (!unsignedPortion)
    {
      // SignatureValue
      totalLength += prependBlock(block, m_signature.getValue());
    }

  // SignatureInfo
  totalLength += prependBlock(block, m_signature.getInfo());

  // Content
  totalLength += prependBlock(block, getContent());

  // MetaInfo
  totalLength += getMetaInfo().wireEncode(block);

  // Name
  totalLength += getName().wireEncode(block);

  if (!unsignedPortion)
    {
      totalLength += block.prependVarNumber (totalLength);
      totalLength += block.prependVarNumber (Tlv::Data);
    }
  return totalLength;
}

inline const Block&
Data::wireEncode() const
{
  if (m_wire.hasWire())
    return m_wire;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  const_cast<Data*>(this)->wireDecode(buffer.block());
  return m_wire;
}

inline void
Data::wireDecode(const Block& wire)
{
  m_wire = wire;
  m_wire.parse();

  // Data ::= DATA-TLV TLV-LENGTH
  //            Name
  //            MetaInfo
  //            Content
  //            Signature

  // Name
  m_name.wireDecode(m_wire.get(Tlv::Name));

  // MetaInfo
  m_metaInfo.wireDecode(m_wire.get(Tlv::MetaInfo));

  // Content
  m_content = m_wire.get(Tlv::Content);

  ///////////////
  // Signature //
  ///////////////

  // SignatureInfo
  m_signature.setInfo(m_wire.get(Tlv::SignatureInfo));

  // SignatureValue
  Block::element_const_iterator val = m_wire.find(Tlv::SignatureValue);
  if (val != m_wire.elements_end())
    m_signature.setValue(*val);
}

inline bool
Data::hasWire() const
{
  return m_wire.hasWire();
}

inline const Name&
Data::getName() const
{
  return m_name;
}

inline Data&
Data::setName(const Name& name)
{
  onChanged();
  m_name = name;

  return *this;
}

inline const MetaInfo&
Data::getMetaInfo() const
{
  return m_metaInfo;
}

inline Data&
Data::setMetaInfo(const MetaInfo& metaInfo)
{
  onChanged();
  m_metaInfo = metaInfo;

  return *this;
}

inline uint32_t
Data::getContentType() const
{
  return m_metaInfo.getType();
}

inline Data&
Data::setContentType(uint32_t type)
{
  onChanged();
  m_metaInfo.setType(type);

  return *this;
}

inline const time::milliseconds&
Data::getFreshnessPeriod() const
{
  return m_metaInfo.getFreshnessPeriod();
}

inline Data&
Data::setFreshnessPeriod(const time::milliseconds& freshnessPeriod)
{
  onChanged();
  m_metaInfo.setFreshnessPeriod(freshnessPeriod);

  return *this;
}

inline const name::Component&
Data::getFinalBlockId() const
{
  return m_metaInfo.getFinalBlockId();
}

inline Data&
Data::setFinalBlockId(const name::Component& finalBlockId)
{
  onChanged();
  m_metaInfo.setFinalBlockId(finalBlockId);

  return *this;
}

inline const Block&
Data::getContent() const
{
  if (m_content.empty())
    m_content = dataBlock(Tlv::Content, reinterpret_cast<const uint8_t*>(0), 0);

  if (!m_content.hasWire())
      m_content.encode();
  return m_content;
}

inline Data&
Data::setContent(const uint8_t* content, size_t contentLength)
{
  onChanged();

  m_content = dataBlock(Tlv::Content, content, contentLength);

  return *this;
}

inline Data&
Data::setContent(const ConstBufferPtr& contentValue)
{
  onChanged();

  m_content = Block(Tlv::Content, contentValue); // not real a wire encoding yet

  return *this;
}

inline Data&
Data::setContent(const Block& content)
{
  onChanged();

  if (content.type() == Tlv::Content)
    m_content = content;
  else {
    m_content = Block(Tlv::Content, content);
  }

  return *this;
}

inline const Signature&
Data::getSignature() const
{
  return m_signature;
}

inline Data&
Data::setSignature(const Signature& signature)
{
  onChanged();
  m_signature = signature;

  return *this;
}

inline Data&
Data::setSignatureValue(const Block& value)
{
  onChanged();
  m_signature.setValue(value);

  return *this;
}

//

inline nfd::LocalControlHeader&
Data::getLocalControlHeader()
{
  return m_localControlHeader;
}

inline const nfd::LocalControlHeader&
Data::getLocalControlHeader() const
{
  return m_localControlHeader;
}

inline uint64_t
Data::getIncomingFaceId() const
{
  return getLocalControlHeader().getIncomingFaceId();
}

inline Data&
Data::setIncomingFaceId(uint64_t incomingFaceId)
{
  getLocalControlHeader().setIncomingFaceId(incomingFaceId);
  // ! do not reset Data's wire !

  return *this;
}

inline void
Data::onChanged()
{
  // The values have changed, so the wire format is invalidated

  // !!!Note!!! Signature is not invalidated and it is responsibility of
  // the application to do proper re-signing if necessary

  m_wire.reset();
}

inline bool
Data::operator==(const Data& other) const
{
  return getName() == other.getName() &&
    getMetaInfo() == other.getMetaInfo() &&
    getContent() == other.getContent() &&
    getSignature() == other.getSignature();
}

inline bool
Data::operator!=(const Data& other) const
{
  return !(*this == other);
}

inline std::ostream&
operator<<(std::ostream& os, const Data& data)
{
  os << "Name: " << data.getName() << "\n";
  os << "MetaInfo: " << data.getMetaInfo() << "\n";
  os << "Content: (size: " << data.getContent().value_size() << ")\n";
  os << "Signature: (type: " << data.getSignature().getType() <<
    ", value_length: "<< data.getSignature().getValue().value_size() << ")";
  os << std::endl;

  return os;
}

} // namespace ndn

#endif
