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

#include "security/transform.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace security {
namespace tests {

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(TestTransform)

BOOST_AUTO_TEST_CASE(SymbolVisibility)
{
  transform::BufferSource* bufferSource = nullptr;
  BOOST_CHECK(bufferSource == nullptr);

  transform::StreamSource* streamSource = nullptr;
  BOOST_CHECK(streamSource == nullptr);

  transform::StepSource* stepSource = nullptr;
  BOOST_CHECK(stepSource == nullptr);

  transform::BoolSink* boolSink = nullptr;
  BOOST_CHECK(boolSink == nullptr);

  transform::StreamSink* streamSink = nullptr;
  BOOST_CHECK(streamSink == nullptr);

  transform::HexEncode* hexEncode = nullptr;
  BOOST_CHECK(hexEncode == nullptr);

  transform::StripSpace* stripSpace = nullptr;
  BOOST_CHECK(stripSpace == nullptr);

  transform::HexDecode* hexDecode = nullptr;
  BOOST_CHECK(hexDecode == nullptr);

  transform::Base64Encode* base64Encode = nullptr;
  BOOST_CHECK(base64Encode == nullptr);

  transform::Base64Decode* base64Decode = nullptr;
  BOOST_CHECK(base64Decode == nullptr);

  transform::DigestFilter* digestFilter = nullptr;
  BOOST_CHECK(digestFilter == nullptr);

  transform::HmacFilter* hmacFilter = nullptr;
  BOOST_CHECK(hmacFilter == nullptr);

  transform::BlockCipher* blockCipher = nullptr;
  BOOST_CHECK(blockCipher == nullptr);

  transform::SignerFilter* signerFilter = nullptr;
  BOOST_CHECK(signerFilter == nullptr);

  transform::VerifierFilter* verifierFilter = nullptr;
  BOOST_CHECK(verifierFilter == nullptr);
}

BOOST_AUTO_TEST_SUITE_END() // TestTransform
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace security
} // namespace ndn
