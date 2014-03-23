/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_MANAGEMENT_NFD_CONTROL_PARAMETERS_HPP
#define NDN_MANAGEMENT_NFD_CONTROL_PARAMETERS_HPP

#include "../name.hpp"
#include "../encoding/tlv-nfd.hpp"

namespace ndn {
namespace nfd {

class ControlParameters;
typedef ControlParameters FaceManagementOptions;
typedef ControlParameters FibManagementOptions;
typedef ControlParameters StrategyChoiceOptions;

enum LocalControlFeature {
  LOCAL_CONTROL_FEATURE_INCOMING_FACE_ID = 1,
  LOCAL_CONTROL_FEATURE_NEXT_HOP_FACE_ID = 2
};

class ControlParameters {
public:
  class Error : public Tlv::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : Tlv::Error(what)
    {
    }
  };

  ControlParameters();

  explicit
  ControlParameters(const Block& block)
  {
    wireDecode(block);
  }

  template<bool T>
  size_t
  wireEncode(EncodingImpl<T>& encoder) const;

  const Block&
  wireEncode() const;

  void
  wireDecode(const Block& wire);

public: // getters & setters
  bool
  hasName() const
  {
    return m_hasName;
  }

  const Name&
  getName() const
  {
    BOOST_ASSERT(m_hasName);
    return m_name;
  }

  ControlParameters&
  setName(const Name& name)
  {
    m_wire.reset();
    m_name = name;
    m_hasName = true;
    return *this;
  }

  ControlParameters&
  unsetName()
  {
    m_wire.reset();
    m_hasName = false;
    return *this;
  }

  bool
  hasFaceId() const
  {
    return m_hasFaceId;
  }

  uint64_t
  getFaceId() const
  {
    BOOST_ASSERT(m_hasFaceId);
    return m_faceId;
  }

  ControlParameters&
  setFaceId(uint64_t faceId)
  {
    m_wire.reset();
    m_faceId = faceId;
    m_hasFaceId = true;
    return *this;
  }

  ControlParameters&
  unsetFaceId()
  {
    m_wire.reset();
    m_hasFaceId = false;
    return *this;
  }

  bool
  hasUri() const
  {
    return m_hasUri;
  }

  const std::string&
  getUri() const
  {
    BOOST_ASSERT(m_hasUri);
    return m_uri;
  }

  ControlParameters&
  setUri(const std::string& uri)
  {
    m_wire.reset();
    m_uri = uri;
    m_hasUri = true;
    return *this;
  }

  ControlParameters&
  unsetUri()
  {
    m_wire.reset();
    m_hasUri = false;
    return *this;
  }

  bool
  hasLocalControlFeature() const
  {
    return m_hasLocalControlFeature;
  }

  LocalControlFeature
  getLocalControlFeature() const
  {
    BOOST_ASSERT(m_hasLocalControlFeature);
    return m_localControlFeature;
  }

  ControlParameters&
  setLocalControlFeature(LocalControlFeature localControlFeature)
  {
    m_wire.reset();
    m_localControlFeature = localControlFeature;
    m_hasLocalControlFeature = true;
    return *this;
  }

  ControlParameters&
  unsetLocalControlFeature()
  {
    m_wire.reset();
    m_hasLocalControlFeature = false;
    return *this;
  }

  bool
  hasCost() const
  {
    return m_hasCost;
  }

  uint64_t
  getCost() const
  {
    BOOST_ASSERT(m_hasCost);
    return m_cost;
  }

  ControlParameters&
  setCost(uint64_t cost)
  {
    m_wire.reset();
    m_cost = cost;
    m_hasCost = true;
    return *this;
  }

  ControlParameters&
  unsetCost()
  {
    m_wire.reset();
    m_hasCost = false;
    return *this;
  }

  bool
  hasStrategy() const
  {
    return m_hasStrategy;
  }

  const Name&
  getStrategy() const
  {
    BOOST_ASSERT(m_hasStrategy);
    return m_strategy;
  }

  ControlParameters&
  setStrategy(const Name& strategy)
  {
    m_wire.reset();
    m_strategy = strategy;
    m_hasStrategy = true;
    return *this;
  }

  ControlParameters&
  unsetStrategy()
  {
    m_wire.reset();
    m_hasStrategy = false;
    return *this;
  }

private: // fields
  Name                m_name;
  uint64_t            m_faceId;
  std::string         m_uri;
  LocalControlFeature m_localControlFeature;
  uint64_t            m_cost;
  Name                m_strategy;

  bool m_hasName;
  bool m_hasFaceId;
  bool m_hasUri;
  bool m_hasLocalControlFeature;
  bool m_hasCost;
  bool m_hasStrategy;

private:
  mutable Block m_wire;
};


inline
ControlParameters::ControlParameters()
  : m_hasName(false)
  , m_hasFaceId(false)
  , m_hasUri(false)
  , m_hasLocalControlFeature(false)
  , m_hasCost(false)
  , m_hasStrategy(false)
{
}

template<bool T>
inline size_t
ControlParameters::wireEncode(EncodingImpl<T>& encoder) const
{
  size_t totalLength = 0;

  if (m_hasStrategy) {
    totalLength += prependNestedBlock(encoder, tlv::nfd::Strategy, m_strategy);
  }
  if (m_hasCost) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::Cost, m_cost);
  }
  if (m_hasLocalControlFeature) {
    totalLength += prependNonNegativeIntegerBlock(encoder,
                   tlv::nfd::LocalControlFeature, m_localControlFeature);
  }
  if (m_hasUri) {
    size_t valLength = encoder.prependByteArray(
                       reinterpret_cast<const uint8_t*>(m_uri.c_str()), m_uri.size());
    totalLength += valLength;
    totalLength += encoder.prependVarNumber(valLength);
    totalLength += encoder.prependVarNumber(tlv::nfd::Uri);
  }
  if (m_hasFaceId) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::nfd::FaceId, m_faceId);
  }
  if (m_hasName) {
    totalLength += m_name.wireEncode(encoder);
  }

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::nfd::ControlParameters);
  return totalLength;
}

inline const Block&
ControlParameters::wireEncode() const
{
  if (m_wire.hasWire())
    return m_wire;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  m_wire = buffer.block();
  return m_wire;
}

inline void
ControlParameters::wireDecode(const Block& block)
{
  if (block.type() != tlv::nfd::ControlParameters) {
    throw Error("expecting TLV-TYPE ControlParameters");
  }
  m_wire = block;
  m_wire.parse();
  Block::element_const_iterator val;

  val = m_wire.find(Tlv::Name);
  m_hasName = val != m_wire.elements_end();
  if (m_hasName) {
    m_name.wireDecode(*val);
  }

  val = m_wire.find(tlv::nfd::FaceId);
  m_hasFaceId = val != m_wire.elements_end();
  if (m_hasFaceId) {
    m_faceId = static_cast<uint64_t>(readNonNegativeInteger(*val));
  }

  val = m_wire.find(tlv::nfd::Uri);
  m_hasUri = val != m_wire.elements_end();
  if (m_hasUri) {
    m_uri.assign(reinterpret_cast<const char*>(val->value()), val->value_size());
  }

  val = m_wire.find(tlv::nfd::LocalControlFeature);
  m_hasLocalControlFeature = val != m_wire.elements_end();
  if (m_hasLocalControlFeature) {
    m_localControlFeature = static_cast<LocalControlFeature>(readNonNegativeInteger(*val));
  }

  val = m_wire.find(tlv::nfd::Cost);
  m_hasCost = val != m_wire.elements_end();
  if (m_hasCost) {
    m_cost = static_cast<uint64_t>(readNonNegativeInteger(*val));
  }

  val = m_wire.find(tlv::nfd::Strategy);
  m_hasStrategy = val != m_wire.elements_end();
  if (m_hasStrategy) {
    val->parse();
    if (val->elements().empty()) {
      throw Error("expecting Strategy/Name");
    }
    else {
      m_strategy.wireDecode(*val->elements_begin());
    }
  }
}

inline std::ostream&
operator<<(std::ostream& os, const ControlParameters& parameters)
{
  os << "ControlParameters(";

  if (parameters.hasName()) {
    os << "Name: " << parameters.getName() << ", ";
  }

  if (parameters.hasFaceId()) {
    os << "FaceId: " << parameters.getFaceId() << ", ";
  }

  if (parameters.hasUri()) {
    os << "Uri: " << parameters.getUri() << ", ";
  }

  if (parameters.hasLocalControlFeature()) {
    os << "LocalControlFeature: " << parameters.getLocalControlFeature() << ", ";
  }

  if (parameters.hasCost()) {
    os << "Cost: " << parameters.getCost() << ", ";
  }

  if (parameters.hasStrategy()) {
    os << "Strategy: " << parameters.getStrategy() << ", ";
  }

  os << ")";
  return os;
}


} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_CONTROL_PARAMETERS_HPP
