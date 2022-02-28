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

#include "ndn-cxx/security/detail/certificate-bundle-decoder.hpp"

#include "tests/boost-test.hpp"
#include "tests/key-chain-fixture.hpp"

namespace ndn {
namespace security {
namespace detail {
namespace tests {

class CertificateBundleDecoderFixture : public ndn::tests::KeyChainFixture
{
protected:
  CertificateBundleDecoderFixture()
  {
    auto id1 = m_keyChain.createIdentity("/hello/world1");
    auto cert1 = id1.getDefaultKey().getDefaultCertificate();
    certBlock1 = cert1.wireEncode();
    m_certs.push_back(certBlock1);

    auto id2 = m_keyChain.createIdentity("/hello/world2");
    auto cert2 = id2.getDefaultKey().getDefaultCertificate();
    certBlock2 = cert2.wireEncode();
    m_certs.push_back(certBlock2);

    cbd.onCertDecoded.connect([this] (const Certificate& receivedCert) {
      BOOST_CHECK_EQUAL(receivedCert.wireEncode(), m_certs.at(nCertsCompleted));
      ++nCertsCompleted;
    });
  }

protected:
  CertificateBundleDecoder cbd;
  Block certBlock1;
  Block certBlock2;
  size_t nCertsCompleted = 0;

private:
  std::vector<Block> m_certs;
};

BOOST_AUTO_TEST_SUITE(Security)
BOOST_FIXTURE_TEST_SUITE(TestCertificateBundleDecoder, CertificateBundleDecoderFixture)

BOOST_AUTO_TEST_CASE(EmptySegment)
{
  BOOST_CHECK_EQUAL(cbd.hasError(), false);
  cbd.append(Block(tlv::Content));
  BOOST_CHECK_EQUAL(cbd.hasError(), false);
  BOOST_CHECK_EQUAL(nCertsCompleted, 0);
}

BOOST_AUTO_TEST_CASE(OneCertOneSegment)
{
  // Segment contains full certificate
  Data d;
  d.setContent(certBlock1);

  cbd.append(d.getContent());
  BOOST_CHECK_EQUAL(cbd.hasError(), false);
  BOOST_CHECK_EQUAL(nCertsCompleted, 1);
}

BOOST_AUTO_TEST_CASE(TwoCertsOneSegment)
{
  // Segment contains two full certificates
  auto buf = std::make_shared<Buffer>(certBlock1.begin(), certBlock1.end());
  buf->insert(buf->end(), certBlock2.begin(), certBlock2.end());
  Data d;
  d.setContent(std::move(buf));

  cbd.append(d.getContent());
  BOOST_CHECK_EQUAL(cbd.hasError(), false);
  BOOST_CHECK_EQUAL(nCertsCompleted, 2);
}

BOOST_AUTO_TEST_CASE(TwoCertsMultipleSegments)
{
  // First segment contains first 250 bytes of cert1
  Data d;
  d.setContent(make_span(certBlock1).first(250));

  // Second segment contains the rest of cert1 and the first 100 bytes of cert2
  auto buf = std::make_shared<Buffer>(certBlock1.begin() + 250, certBlock1.end());
  buf->insert(buf->end(), certBlock2.begin(), certBlock2.begin() + 100);
  Data d2;
  d2.setContent(std::move(buf));

  // Third segment contains the rest of cert2
  Data d3;
  d3.setContent(std::make_shared<Buffer>(certBlock2.begin() + 100, certBlock2.end()));

  cbd.append(d.getContent());
  BOOST_CHECK_EQUAL(cbd.hasError(), false);
  BOOST_CHECK_EQUAL(nCertsCompleted, 0);

  cbd.append(d2.getContent());
  BOOST_CHECK_EQUAL(cbd.hasError(), false);
  BOOST_CHECK_EQUAL(nCertsCompleted, 1);

  cbd.append(d3.getContent());
  BOOST_CHECK_EQUAL(cbd.hasError(), false);
  BOOST_CHECK_EQUAL(nCertsCompleted, 2);
}

BOOST_AUTO_TEST_CASE(InvalidCert)
{
  // First segment contains all of cert1
  Data d;
  d.setContent(certBlock1);

  const uint8_t buf[] = {
    0x06, 0x20, // Data
          0x07, 0x11, // Name
                0x08, 0x05, 0x68, 0x65, 0x6c, 0x6c, 0x6f, // GenericNameComponent 'hello'
                0x08, 0x01, 0x31, // GenericNameComponent '1'
                0x08, 0x05, 0x77, 0x6f, 0x72, 0x6c, 0x64, // GenericNameComponent 'world'
          0x14, 0x00, // MetaInfo empty
          0x15, 0x00, // Content empty
          0x16, 0x05, // SignatureInfo
                0x1b, 0x01, 0x01, // SignatureType RSA
                0x1c, 0x00, // KeyLocator empty
          0x17, 0x00 // SignatureValue empty
  };
  // Second segment contains non-Certificate data
  Data d2;
  d2.setContent(buf);

  // Third segment contains all of cert2
  Data d3;
  d3.setContent(certBlock2);

  cbd.append(d.getContent());
  BOOST_CHECK_EQUAL(cbd.hasError(), false);
  BOOST_CHECK_EQUAL(nCertsCompleted, 1);

  BOOST_CHECK_EXCEPTION(cbd.append(d2.getContent()), tlv::Error, [] (const auto& e) {
    return e.what() == "Name does not follow the naming convention for certificate"s;
  });
  BOOST_CHECK_EQUAL(cbd.hasError(), true);
  BOOST_CHECK_EQUAL(nCertsCompleted, 1);

  BOOST_CHECK_EXCEPTION(cbd.append(d3.getContent()), tlv::Error, [] (const auto& e) {
    return e.what() == "Unrecoverable decoding error"s;
  });
  BOOST_CHECK_EQUAL(cbd.hasError(), true);
  BOOST_CHECK_EQUAL(nCertsCompleted, 1);
}

BOOST_AUTO_TEST_CASE(UnrecognizedCritical)
{
  // First segment contains an unrecognized critical element
  Data d;
  d.setContent("050B07030102030A0404050607"_block);

  // Second segment contains cert1
  Data d2;
  d2.setContent(certBlock1);

  BOOST_CHECK_EXCEPTION(cbd.append(d.getContent()), tlv::Error, [] (const auto& e) {
    return e.what() == "Unrecognized element of critical type 5"s;
  });
  BOOST_CHECK_EQUAL(cbd.hasError(), true);
  BOOST_CHECK_EQUAL(nCertsCompleted, 0);

  BOOST_CHECK_EXCEPTION(cbd.append(d2.getContent()), tlv::Error, [] (const auto& e) {
    return e.what() == "Unrecoverable decoding error"s;
  });
  BOOST_CHECK_EQUAL(cbd.hasError(), true);
  BOOST_CHECK_EQUAL(nCertsCompleted, 0);
}

BOOST_AUTO_TEST_CASE(UnrecognizedNonCritical)
{
  // First segment contains an unrecognized non-critical element
  Data d;
  d.setContent("4202CAFE"_block);

  // Second segment contains cert1
  Data d2;
  d2.setContent(certBlock1);

  cbd.append(d.getContent());
  BOOST_CHECK_EQUAL(cbd.hasError(), false);
  BOOST_CHECK_EQUAL(nCertsCompleted, 0);

  cbd.append(d2.getContent());
  BOOST_CHECK_EQUAL(cbd.hasError(), false);
  BOOST_CHECK_EQUAL(nCertsCompleted, 1);
}

BOOST_AUTO_TEST_SUITE_END() // TestCertificateBundleEncoderDecoder
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace detail
} // namespace security
} // namespace ndn
