/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
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
   */
  Data();

  /**
   * @brief Create a new Data object with the given name
   * @param name A reference to the name which is copied.
   */
  Data(const Name& name);

  /**
   * @brief Create a new Data object from wire encoding
   */
  explicit
  Data(const Block& wire)
  {
    wireDecode(wire);
  }

  /**
   * @brief The destructor
   */
  ~Data();

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
   * @brief Check if already has wire
   */
  bool
  hasWire() const;

  ////////////////////////////////////////////////////////////////////

  const Name&
  getName() const;

  /**
   * @brief Set name to a copy of the given Name.
   *
   * @param name The Name which is copied.
   * @return This Data so that you can chain calls to update values.
   */
  void
  setName(const Name& name);

  //

  const MetaInfo&
  getMetaInfo() const;

  /**
   * @brief Set metaInfo to a copy of the given MetaInfo.
   * @param metaInfo The MetaInfo which is copied.
   * @return This Data so that you can chain calls to update values.
   */
  void
  setMetaInfo(const MetaInfo& metaInfo);

  //

  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////
  // MetaInfo proxy methods

  uint32_t
  getContentType() const;

  void
  setContentType(uint32_t type);

  //

  const time::milliseconds&
  getFreshnessPeriod() const;

  void
  setFreshnessPeriod(const time::milliseconds& freshnessPeriod);

  //

  const name::Component&
  getFinalBlockId() const;

  void
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
   * @brief Set the content to a copy of the data in the vector.
   * @param content A vector whose contents are copied.
   * @return This Data so that you can chain calls to update values.
   */
  void
  setContent(const uint8_t* content, size_t contentLength);

  void
  setContent(const Block& content);

  void
  setContent(const ConstBufferPtr& contentValue);

  //

  const Signature&
  getSignature() const;

  /**
   * @brief Set the signature to a copy of the given signature.
   * @param signature The signature object which is cloned.
   */
  void
  setSignature(const Signature& signature);

  void
  setSignatureValue(const Block& value);

  ///////////////////////////////////////////////////////////////

  nfd::LocalControlHeader&
  getLocalControlHeader();

  const nfd::LocalControlHeader&
  getLocalControlHeader() const;

  uint64_t
  getIncomingFaceId() const;

  void
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

inline
Data::~Data()
{
}

template<bool T>
inline size_t
Data::wireEncode(EncodingImpl<T>& block, bool unsignedPortion/* = false*/) const
{
  size_t total_len = 0;

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
      total_len += prependBlock(block, m_signature.getValue());
    }

  // SignatureInfo
  total_len += prependBlock(block, m_signature.getInfo());

  // Content
  total_len += prependBlock(block, getContent());

  // MetaInfo
  total_len += getMetaInfo().wireEncode(block);

  // Name
  total_len += getName().wireEncode(block);

  if (!unsignedPortion)
    {
      total_len += block.prependVarNumber (total_len);
      total_len += block.prependVarNumber (Tlv::Data);
    }
  return total_len;
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

/**
 * Decode the input using a particular wire format and update this Data.
 * @param input The input byte array to be decoded.
 */
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

inline void
Data::setName(const Name& name)
{
  onChanged();
  m_name = name;
}

inline const MetaInfo&
Data::getMetaInfo() const
{
  return m_metaInfo;
}

inline void
Data::setMetaInfo(const MetaInfo& metaInfo)
{
  onChanged();
  m_metaInfo = metaInfo;
}

inline uint32_t
Data::getContentType() const
{
  return m_metaInfo.getType();
}

inline void
Data::setContentType(uint32_t type)
{
  onChanged();
  m_metaInfo.setType(type);
}

inline const time::milliseconds&
Data::getFreshnessPeriod() const
{
  return m_metaInfo.getFreshnessPeriod();
}

inline void
Data::setFreshnessPeriod(const time::milliseconds& freshnessPeriod)
{
  onChanged();
  m_metaInfo.setFreshnessPeriod(freshnessPeriod);
}

inline const name::Component&
Data::getFinalBlockId() const
{
  return m_metaInfo.getFinalBlockId();
}

inline void
Data::setFinalBlockId(const name::Component& finalBlockId)
{
  onChanged();
  m_metaInfo.setFinalBlockId(finalBlockId);
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

inline void
Data::setContent(const uint8_t* content, size_t contentLength)
{
  onChanged();

  m_content = dataBlock(Tlv::Content, content, contentLength);
}

inline void
Data::setContent(const ConstBufferPtr& contentValue)
{
  onChanged();

  m_content = Block(Tlv::Content, contentValue); // not real a wire encoding yet
}

inline void
Data::setContent(const Block& content)
{
  onChanged();

  if (content.type() == Tlv::Content)
    m_content = content;
  else {
    m_content = Block(Tlv::Content, content);
  }
}

inline const Signature&
Data::getSignature() const
{
  return m_signature;
}

inline void
Data::setSignature(const Signature& signature)
{
  onChanged();
  m_signature = signature;
}

inline void
Data::setSignatureValue(const Block& value)
{
  onChanged();
  m_signature.setValue(value);
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

inline void
Data::setIncomingFaceId(uint64_t incomingFaceId)
{
  getLocalControlHeader().setIncomingFaceId(incomingFaceId);
  // ! do not reset Data's wire !
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
