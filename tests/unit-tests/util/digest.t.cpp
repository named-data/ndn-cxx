/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#include "util/digest.hpp"
#include "util/crypto.hpp"
#include "util/string-helper.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace util {
namespace test {

BOOST_AUTO_TEST_SUITE(UtilDigest)

BOOST_AUTO_TEST_CASE(Sha256Digest)
{
  uint8_t origin[4] = {0x01, 0x02, 0x03, 0x04};
  ConstBufferPtr digest1 = crypto::sha256(origin, 4);

  Sha256 statefulSha256;
  statefulSha256.update(origin, 1);
  statefulSha256.update(origin + 1, 1);
  statefulSha256.update(origin + 2, 1);
  statefulSha256.update(origin + 3, 1);
  ConstBufferPtr digest2 = statefulSha256.computeDigest();

  BOOST_CHECK_EQUAL_COLLECTIONS(digest1->buf(),
                                digest1->buf() + digest1->size(),
                                digest2->buf(),
                                digest2->buf() + digest2->size());
}

BOOST_AUTO_TEST_CASE(Compute)
{
  std::string input = "Hello, World!";
  ConstBufferPtr digest1 = crypto::sha256(reinterpret_cast<const uint8_t*>(input.data()),
                                          input.size());

  Sha256 hashObject;
  hashObject << input;
  BOOST_CHECK_EQUAL(hashObject.toString(), "DFFD6021BB2BD5B0AF676290809EC3A53191DD81C7F70A4B28688A362182986F");
  ConstBufferPtr digest2 = hashObject.computeDigest();
  BOOST_CHECK_EQUAL_COLLECTIONS(digest1->buf(),
                                digest1->buf() + digest1->size(),
                                digest2->buf(),
                                digest2->buf() + digest2->size());

}

BOOST_AUTO_TEST_CASE(ConstructFromStream)
{
  std::string input = "Hello, World!";
  ConstBufferPtr digest1 = crypto::sha256(reinterpret_cast<const uint8_t*>(input.data()),
                                          input.size());

  std::istringstream is(input);
  Sha256 hashObject(is);
  BOOST_CHECK_EQUAL(hashObject.toString(), "DFFD6021BB2BD5B0AF676290809EC3A53191DD81C7F70A4B28688A362182986F");
  ConstBufferPtr digest2 = hashObject.computeDigest();
  BOOST_CHECK_EQUAL_COLLECTIONS(digest1->buf(),
                                digest1->buf() + digest1->size(),
                                digest2->buf(),
                                digest2->buf() + digest2->size());
}

BOOST_AUTO_TEST_CASE(Compare)
{
  uint8_t origin[4] = {0x01, 0x02, 0x03, 0x04};

  Sha256 digest;
  digest.update(origin, 4);
  digest.computeDigest();

  Sha256 digest2;
  digest2.update(origin, 1);
  digest2.update(origin + 1, 1);
  digest2.update(origin + 2, 1);
  digest2.update(origin + 3, 1);
  digest2.computeDigest();

  BOOST_CHECK(digest == digest2);
  BOOST_CHECK_EQUAL(digest != digest2, false);
}

BOOST_AUTO_TEST_CASE(OperatorDigest)
{
  uint8_t origin[32] = {0x94, 0xEE, 0x05, 0x93, 0x35, 0xE5, 0x87, 0xE5,
                        0x01, 0xCC, 0x4B, 0xF9, 0x06, 0x13, 0xE0, 0x81,
                        0x4F, 0x00, 0xA7, 0xB0, 0x8B, 0xC7, 0xC6, 0x48,
                        0xFD, 0x86, 0x5A, 0x2A, 0xF6, 0xA2, 0x2C, 0xC2};
  ConstBufferPtr digest1 = crypto::sha256(origin, 32);

  std::string str("TEST");
  Sha256 metaDigest;
  metaDigest << str;

  Sha256 statefulSha256;
  statefulSha256 << metaDigest;
  ConstBufferPtr digest2 = statefulSha256.computeDigest();

  BOOST_CHECK_EQUAL_COLLECTIONS(digest1->buf(),
                                digest1->buf() + digest1->size(),
                                digest2->buf(),
                                digest2->buf() + digest2->size());
}

BOOST_AUTO_TEST_CASE(OperatorString)
{
  uint8_t origin[4] = {0x54, 0x45, 0x53, 0x54};
  ConstBufferPtr digest1 = crypto::sha256(origin, 4);

  std::string str("TEST");
  Sha256 statefulSha256;
  statefulSha256 << str;
  ConstBufferPtr digest2 = statefulSha256.computeDigest();

  BOOST_CHECK_EQUAL_COLLECTIONS(digest1->buf(),
                                digest1->buf() + digest1->size(),
                                digest2->buf(),
                                digest2->buf() + digest2->size());
}

BOOST_AUTO_TEST_CASE(OperatorBlock)
{
  uint8_t origin[] = {
    0x16, 0x1b, // SignatureInfo
      0x1b, 0x01, // SignatureType
        0x01, // Sha256WithRsa
      0x1c, 0x16, // KeyLocator
        0x07, 0x14, // Name
          0x08, 0x04,
            0x74, 0x65, 0x73, 0x74,
          0x08, 0x03,
            0x6b, 0x65, 0x79,
          0x08, 0x07,
            0x6c, 0x6f, 0x63, 0x61, 0x74, 0x6f, 0x72
  };
  ConstBufferPtr digest1 = crypto::sha256(origin, sizeof(origin));

  Sha256 statefulSha256;
  Block block(origin, sizeof(origin));
  statefulSha256 << block;
  ConstBufferPtr digest2 = statefulSha256.computeDigest();

  BOOST_CHECK_EQUAL_COLLECTIONS(digest1->buf(),
                                digest1->buf() + digest1->size(),
                                digest2->buf(),
                                digest2->buf() + digest2->size());
}

BOOST_AUTO_TEST_CASE(OperatorUint64t)
{
  uint64_t origin[4] = {1, 2, 3, 4};
  ConstBufferPtr digest1 = crypto::sha256(reinterpret_cast<uint8_t*>(origin), 32);

  Sha256 statefulSha256;
  statefulSha256 << origin[0];
  statefulSha256 << origin[1];
  statefulSha256 << origin[2];
  statefulSha256 << origin[3];
  ConstBufferPtr digest2 = statefulSha256.computeDigest();

  BOOST_CHECK_EQUAL_COLLECTIONS(digest1->buf(),
                                digest1->buf() + digest1->size(),
                                digest2->buf(),
                                digest2->buf() + digest2->size());
}


BOOST_AUTO_TEST_CASE(Error)
{
  uint64_t origin = 256;

  Sha256 digest;
  BOOST_CHECK(digest.empty());

  digest << origin;

  BOOST_CHECK_NO_THROW(digest.computeDigest());
  BOOST_CHECK_THROW(digest << origin, Sha256::Error);

  digest.reset();
}

BOOST_AUTO_TEST_CASE(ComputeDigest)
{
  uint8_t origin[4] = {0x01, 0x02, 0x03, 0x04};
  ConstBufferPtr digest1 = crypto::sha256(origin, 4);

  ConstBufferPtr digest2 = Sha256::computeDigest(origin, 4);

  BOOST_CHECK_EQUAL_COLLECTIONS(digest1->buf(),
                                digest1->buf() + digest1->size(),
                                digest2->buf(),
                                digest2->buf() + digest2->size());
}

BOOST_AUTO_TEST_CASE(Print)
{
  uint8_t origin[32] = {0x94, 0xEE, 0x05, 0x93, 0x35, 0xE5, 0x87, 0xE5,
                        0x01, 0xCC, 0x4B, 0xF9, 0x06, 0x13, 0xE0, 0x81,
                        0x4F, 0x00, 0xA7, 0xB0, 0x8B, 0xC7, 0xC6, 0x48,
                        0xFD, 0x86, 0x5A, 0x2A, 0xF6, 0xA2, 0x2C, 0xC2};

  std::string hexString = toHex(origin, 32);

  std::string str("TEST");
  Sha256 digest;
  digest << str;

  std::ostringstream os;
  os << digest;

  BOOST_CHECK_EQUAL(os.str(), hexString);
  BOOST_CHECK_EQUAL(digest.toString(), hexString);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace test
} // namespace util
} // namespace ndn
