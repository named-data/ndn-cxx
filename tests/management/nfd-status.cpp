/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include "management/nfd-status.hpp"
#include "data.hpp"

#include <boost/test/unit_test.hpp>

namespace ndn {
namespace nfd {

BOOST_AUTO_TEST_SUITE(NfdStatus)

BOOST_AUTO_TEST_CASE(StatusEncode)
{
  Status status1;
  status1.setNfdVersion(1014210635);
  status1.setStartTimestamp(Status::Timestamp(boost::chrono::seconds(375193249)));
  status1.setCurrentTimestamp(Status::Timestamp(boost::chrono::seconds(1886109034)));
  status1.setNNameTreeEntries(1849943160);
  status1.setNFibEntries(621739748);
  status1.setNPitEntries(482129741);
  status1.setNMeasurementsEntries(1771725298);
  status1.setNCsEntries(1264968688);
  status1.setNInInterests(612811615);
  status1.setNOutInterests(952144445);
  status1.setNInDatas(1843576050);
  status1.setNOutDatas(138198826);

  EncodingBuffer buffer;
  status1.wireEncode(buffer);

  Data data;
  data.setContent(buffer.buf(), buffer.size());

  Status status2(data.getContent());
  BOOST_CHECK_EQUAL(status1.getNfdVersion(), status2.getNfdVersion());
  BOOST_CHECK_EQUAL(status1.getStartTimestamp(), status2.getStartTimestamp());
  BOOST_CHECK_EQUAL(status1.getCurrentTimestamp(), status2.getCurrentTimestamp());
  BOOST_CHECK_EQUAL(status1.getNNameTreeEntries(), status2.getNNameTreeEntries());
  BOOST_CHECK_EQUAL(status1.getNFibEntries(), status2.getNFibEntries());
  BOOST_CHECK_EQUAL(status1.getNPitEntries(), status2.getNPitEntries());
  BOOST_CHECK_EQUAL(status1.getNMeasurementsEntries(), status2.getNMeasurementsEntries());
  BOOST_CHECK_EQUAL(status1.getNCsEntries(), status2.getNCsEntries());
  BOOST_CHECK_EQUAL(status1.getNInInterests(), status2.getNInInterests());
  BOOST_CHECK_EQUAL(status1.getNOutInterests(), status2.getNOutInterests());
  BOOST_CHECK_EQUAL(status1.getNInDatas(), status2.getNInDatas());
  BOOST_CHECK_EQUAL(status1.getNOutDatas(), status2.getNOutDatas());
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace nfd
} // namespace ndn
