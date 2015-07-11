/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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

#include "security/transform/block-cipher.hpp"
#include "security/transform/buffer-source.hpp"
#include "security/transform/stream-sink.hpp"
#include "encoding/buffer-stream.hpp"
#include <iostream>

#include "boost-test.hpp"

namespace ndn {
namespace security {
namespace transform {
namespace tests {

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(Transform)
BOOST_AUTO_TEST_SUITE(TestBlockCipher)

BOOST_AUTO_TEST_CASE(AesCbc)
{
  uint8_t key[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
  };

  uint8_t plainText[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
  };

  uint8_t iv[] = {
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
  };

  /*
   * Cipher text can be generated using:
   *
   * {
   *   using namespace CryptoPP;
   *   CBC_Mode<AES>::Encryption aes(key, sizeof(key), iv);
   *   StringSource(plainText, sizeof(plainText), true,
   *                new StreamTransformationFilter(aes,
   *                                               new HexEncoder(new FileSink(std::cerr), false)));
   * }
   */
  uint8_t cipherText[] = {
    0x07, 0x4d, 0x32, 0x68, 0xc3, 0x40, 0x64, 0x43,
    0x1e, 0x66, 0x4c, 0x25, 0x66, 0x42, 0x0f, 0x59,
    0x0a, 0x51, 0x19, 0x07, 0x67, 0x5c, 0x0e, 0xfa,
    0xa6, 0x8c, 0xbb, 0xaf, 0xfd, 0xea, 0x47, 0xd4,
    0xc7, 0x2c, 0x12, 0x34, 0x79, 0xde, 0xec, 0xc8,
    0x75, 0x33, 0x8f, 0x6b, 0xd6, 0x55, 0xf3, 0xfa
  };

  // encrypt
  OBufferStream os;
  bufferSource(plainText, sizeof(plainText)) >>
    blockCipher(BlockCipherAlgorithm::AES_CBC,
                CipherOperator::ENCRYPT,
                key, sizeof(key), iv, sizeof(iv)) >> streamSink(os);

  ConstBufferPtr buf = os.buf();
  BOOST_CHECK_EQUAL_COLLECTIONS(cipherText, cipherText + sizeof(cipherText),
                                buf->begin(), buf->end());

  // decrypt
  OBufferStream os2;
  bufferSource(cipherText, sizeof(cipherText)) >>
    blockCipher(BlockCipherAlgorithm::AES_CBC,
                CipherOperator::DECRYPT,
                key, sizeof(key), iv, sizeof(iv)) >> streamSink(os2);

  ConstBufferPtr buf2 = os2.buf();
  BOOST_CHECK_EQUAL_COLLECTIONS(plainText, plainText + sizeof(plainText),
                                buf2->begin(), buf2->end());
}

BOOST_AUTO_TEST_SUITE_END() // TestBlockCipher
BOOST_AUTO_TEST_SUITE_END() // Transform
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace transform
} // namespace security
} // namespace ndn
