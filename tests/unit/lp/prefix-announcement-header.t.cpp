/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#include "ndn-cxx/lp/prefix-announcement-header.hpp"
#include "ndn-cxx/lp/tlv.hpp"
#include "ndn-cxx/security/signature-sha256-with-rsa.hpp"

#include "tests/boost-test.hpp"
#include "tests/identity-management-fixture.hpp"

namespace ndn {
namespace lp {
namespace tests {

BOOST_AUTO_TEST_SUITE(Lp)
BOOST_FIXTURE_TEST_SUITE(TestPrefixAnnouncementHeader, ndn::tests::IdentityManagementFixture)

BOOST_AUTO_TEST_CASE(EncodeDecode)
{
  EncodingEstimator estimator;
  PrefixAnnouncementHeader header;
  BOOST_CHECK_THROW(header.wireEncode(estimator), PrefixAnnouncementHeader::Error);
  BOOST_CHECK_THROW(PrefixAnnouncementHeader{PrefixAnnouncement()}, PrefixAnnouncementHeader::Error);

  PrefixAnnouncement pa;
  pa.setAnnouncedName("/net/example");
  pa.setExpiration(1_h);
  const Data& data = pa.toData(m_keyChain, signingWithSha256(), 1);
  Block encodedData = data.wireEncode();

  Block expectedBlock(tlv::PrefixAnnouncement);
  expectedBlock.push_back(encodedData);
  expectedBlock.encode();

  PrefixAnnouncementHeader pah0(pa);
  size_t estimatedSize = pah0.wireEncode(estimator);
  EncodingBuffer buffer(estimatedSize, 0);
  pah0.wireEncode(buffer);
  Block wire = buffer.block();
  BOOST_CHECK_EQUAL(expectedBlock, wire);

  PrefixAnnouncementHeader pah1;
  pah1.wireDecode(wire);
  BOOST_CHECK_EQUAL(*pah0.getPrefixAnn(), *pah1.getPrefixAnn());
}

BOOST_AUTO_TEST_SUITE_END() // TestPrefixAnnouncementHeader
BOOST_AUTO_TEST_SUITE_END() // Lp

} // namespace tests
} // namespace lp
} // namespace ndn
