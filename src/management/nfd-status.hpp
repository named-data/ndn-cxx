/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_MANAGEMENT_NFD_STATUS_HPP
#define NDN_MANAGEMENT_NFD_STATUS_HPP

#include "../encoding/tlv-nfd.hpp"
#include "../encoding/tlv.hpp"
#include "../encoding/encoding-buffer.hpp"
#include <boost/chrono/include.hpp>

namespace ndn {
namespace nfd {

class Status {
public:
  struct Error : public Tlv::Error
  {
    Error(const std::string& what)
      : Tlv::Error(what)
    {
    }
  };

  Status();

  explicit
  Status(const Block& payload)
  {
    this->wireDecode(payload);
  }

  // Status is encoded as a series of blocks within Data Payload, not a single block.
  // Thus there is no "const Block& wireEncode() const" method.

  template<bool T>
  size_t
  wireEncode(EncodingImpl<T>& encoder) const;

  void
  wireDecode(const Block& payload);

public:
  typedef boost::chrono::time_point<boost::chrono::system_clock, boost::chrono::seconds> Timestamp;

  int
  getNfdVersion() const
  {
    return m_nfdVersion;
  }

  void
  setNfdVersion(int nfdVersion)
  {
    m_nfdVersion = nfdVersion;
  }

  Timestamp
  getStartTimestamp() const
  {
    return m_startTimestamp;
  }

  void
  setStartTimestamp(Timestamp startTimestamp)
  {
    m_startTimestamp = startTimestamp;
  }

  Timestamp
  getCurrentTimestamp() const
  {
    return m_currentTimestamp;
  }

  void
  setCurrentTimestamp(Timestamp currentTimestamp)
  {
    m_currentTimestamp = currentTimestamp;
  }

  size_t
  getNNameTreeEntries() const
  {
    return m_nNameTreeEntries;
  }

  void
  setNNameTreeEntries(size_t nNameTreeEntries)
  {
    m_nNameTreeEntries = nNameTreeEntries;
  }

  size_t
  getNFibEntries() const
  {
    return m_nFibEntries;
  }

  void
  setNFibEntries(size_t nFibEntries)
  {
    m_nFibEntries = nFibEntries;
  }

  size_t
  getNPitEntries() const
  {
    return m_nPitEntries;
  }

  void
  setNPitEntries(size_t nPitEntries)
  {
    m_nPitEntries = nPitEntries;
  }

  size_t
  getNMeasurementsEntries() const
  {
    return m_nMeasurementsEntries;
  }

  void
  setNMeasurementsEntries(size_t nMeasurementsEntries)
  {
    m_nMeasurementsEntries = nMeasurementsEntries;
  }

  size_t
  getNCsEntries() const
  {
    return m_nCsEntries;
  }

  void
  setNCsEntries(size_t nCsEntries)
  {
    m_nCsEntries = nCsEntries;
  }

  int
  getNInInterests() const
  {
    return m_nInInterests;
  }

  void
  setNInInterests(int nInInterests)
  {
    m_nInInterests = nInInterests;
  }

  int
  getNOutInterests() const
  {
    return m_nOutInterests;
  }

  void
  setNOutInterests(int nOutInterests)
  {
    m_nOutInterests = nOutInterests;
  }

  int
  getNInDatas() const
  {
    return m_nInDatas;
  }

  void
  setNInDatas(int nInDatas)
  {
    m_nInDatas = nInDatas;
  }

  int
  getNOutDatas() const
  {
    return m_nOutDatas;
  }

  void
  setNOutDatas(int nOutDatas)
  {
    m_nOutDatas = nOutDatas;
  }

private:
  int       m_nfdVersion;
  Timestamp m_startTimestamp;
  Timestamp m_currentTimestamp;
  size_t    m_nNameTreeEntries;
  size_t    m_nFibEntries;
  size_t    m_nPitEntries;
  size_t    m_nMeasurementsEntries;
  size_t    m_nCsEntries;
  int       m_nInInterests;
  int       m_nOutInterests;
  int       m_nInDatas;
  int       m_nOutDatas;
};

BOOST_STATIC_ASSERT((boost::is_same<Status::Timestamp::period, boost::ratio<1> >::value));

inline
Status::Status()
{
  m_nfdVersion = 0;
  m_startTimestamp = Timestamp::min();
  m_currentTimestamp = Timestamp::min();
  m_nNameTreeEntries = 0;
  m_nFibEntries = 0;
  m_nPitEntries = 0;
  m_nMeasurementsEntries = 0;
  m_nCsEntries = 0;
  m_nInInterests = 0;
  m_nOutInterests = 0;
  m_nInDatas = 0;
  m_nOutDatas = 0;
}

template<bool T>
inline size_t
Status::wireEncode(EncodingImpl<T>& encoder) const
{
  size_t total_len = 0;

  total_len += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NOutDatas,
                                              m_nOutDatas);
  total_len += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NInDatas,
                                              m_nInDatas);
  total_len += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NOutInterests,
                                              m_nOutInterests);
  total_len += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NInInterests,
                                              m_nInInterests);
  total_len += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NCsEntries,
                                              m_nCsEntries);
  total_len += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NMeasurementsEntries,
                                              m_nMeasurementsEntries);
  total_len += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NPitEntries,
                                              m_nPitEntries);
  total_len += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NFibEntries,
                                              m_nFibEntries);
  total_len += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NNameTreeEntries,
                                              m_nNameTreeEntries);
  total_len += prependNonNegativeIntegerBlock(encoder, tlv::nfd::CurrentTimestamp,
               m_currentTimestamp.time_since_epoch().count());
  total_len += prependNonNegativeIntegerBlock(encoder, tlv::nfd::StartTimestamp,
               m_startTimestamp.time_since_epoch().count());
  total_len += prependNonNegativeIntegerBlock(encoder, tlv::nfd::NfdVersion,
                                              m_nfdVersion);

  return total_len;
}

inline void
Status::wireDecode(const Block& payload)
{
  m_nfdVersion = 0;
  m_startTimestamp = Timestamp::min();
  m_currentTimestamp = Timestamp::min();
  m_nNameTreeEntries = 0;
  m_nFibEntries = 0;
  m_nPitEntries = 0;
  m_nMeasurementsEntries = 0;
  m_nCsEntries = 0;
  m_nInInterests = 0;
  m_nOutInterests = 0;
  m_nInDatas = 0;
  m_nOutDatas = 0;

  if (payload.type() != Tlv::Content) {
    throw Error("Requested decoding of Status Payload, but block is not Content");
  }
  payload.parse();

  Block::element_const_iterator val;

  val = payload.find(tlv::nfd::NfdVersion);
  if (val != payload.elements_end()) {
    m_nfdVersion = static_cast<int>(readNonNegativeInteger(*val));
  }

  val = payload.find(tlv::nfd::StartTimestamp);
  if (val != payload.elements_end()) {
    m_startTimestamp = Timestamp(boost::chrono::seconds(readNonNegativeInteger(*val)));
  }

  val = payload.find(tlv::nfd::CurrentTimestamp);
  if (val != payload.elements_end()) {
    m_currentTimestamp = Timestamp(boost::chrono::seconds(readNonNegativeInteger(*val)));
  }

  val = payload.find(tlv::nfd::NNameTreeEntries);
  if (val != payload.elements_end()) {
    m_nNameTreeEntries = static_cast<size_t>(readNonNegativeInteger(*val));
  }

  val = payload.find(tlv::nfd::NFibEntries);
  if (val != payload.elements_end()) {
    m_nFibEntries = static_cast<size_t>(readNonNegativeInteger(*val));
  }

  val = payload.find(tlv::nfd::NPitEntries);
  if (val != payload.elements_end()) {
    m_nPitEntries = static_cast<size_t>(readNonNegativeInteger(*val));
  }

  val = payload.find(tlv::nfd::NMeasurementsEntries);
  if (val != payload.elements_end()) {
    m_nMeasurementsEntries = static_cast<size_t>(readNonNegativeInteger(*val));
  }

  val = payload.find(tlv::nfd::NCsEntries);
  if (val != payload.elements_end()) {
    m_nCsEntries = static_cast<size_t>(readNonNegativeInteger(*val));
  }

  val = payload.find(tlv::nfd::NInInterests);
  if (val != payload.elements_end()) {
    m_nInInterests = static_cast<int>(readNonNegativeInteger(*val));
  }

  val = payload.find(tlv::nfd::NOutInterests);
  if (val != payload.elements_end()) {
    m_nOutInterests = static_cast<int>(readNonNegativeInteger(*val));
  }

  val = payload.find(tlv::nfd::NInDatas);
  if (val != payload.elements_end()) {
    m_nInDatas = static_cast<int>(readNonNegativeInteger(*val));
  }

  val = payload.find(tlv::nfd::NOutDatas);
  if (val != payload.elements_end()) {
    m_nOutDatas = static_cast<int>(readNonNegativeInteger(*val));
  }
}

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_STATUS_HPP
