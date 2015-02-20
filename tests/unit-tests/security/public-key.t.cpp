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

#include "security/public-key.hpp"
#include "security/cryptopp.hpp"
#include "encoding/buffer-stream.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(SecurityPublicKey)

const std::string RSA_DER("MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuFoDcNtffwbfFix64fw0\
hI2tKMkFrc6Ex7yw0YLMK9vGE8lXOyBl/qXabow6RCz+GldmFN6E2Qhm1+AX3Zm5\
sj3H53/HPtzMefvMQ9X7U+lK8eNMWawpRzvBh4/36VrK/awlkNIVIQ9aXj6q6BVe\
zL+zWT/WYemLq/8A1/hHWiwCtfOH1xQhGqWHJzeSgwIgOOrzxTbRaCjhAb1u2TeV\
yx/I9H/DV+AqSHCaYbB92HDcDN0kqwSnUf5H1+osE9MR5DLBLhXdSiULSgxT3Or/\
y2QgsgUK59WrjhlVMPEiHHRs15NZJbL1uQFXjgScdEarohcY3dilqotineFZCeN8\
DwIDAQAB");

const uint8_t RSA_DER_KEY_DIGEST[] = {
  0x1d, 0x20,
    0x58, 0x72, 0x4c, 0xf7, 0x36, 0x3d, 0xee, 0x4a,
    0x5c, 0x5b, 0x39, 0x44, 0x2d, 0xf6, 0x1a, 0x24,
    0xda, 0x13, 0xac, 0xab, 0x70, 0xf7, 0x74, 0x40,
    0x5a, 0x44, 0xfe, 0xc0, 0xc9, 0x26, 0x58, 0x74
};

const std::string ECDSA_DER("MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAENZpqkPJDj8uhSpffOiCbvSYMLsGB\
1Eo/WU6mrexjGvduQXjqwon/eSHFI6EgHZk8L9KfiV5XVtVsk2g5wIpJVg==");

const uint8_t ECDSA_DER_KEY_DIGEST[] = {
  0x1d, 0x20,
    0xaf, 0x82, 0x3f, 0xfc, 0xdc, 0x85, 0xb2, 0xa4,
    0xc8, 0xf5, 0x3b, 0x1a, 0xf8, 0xec, 0x4a, 0x55,
    0x97, 0x55, 0x19, 0x3f, 0x54, 0xdd, 0xd0, 0xfd,
    0xb5, 0x9d, 0x80, 0x65, 0x80, 0x6b, 0x4b, 0x63
};

BOOST_AUTO_TEST_CASE(RSA)
{
  using namespace CryptoPP;

  OBufferStream os;
  StringSource ss(reinterpret_cast<const uint8_t*>(RSA_DER.c_str()), RSA_DER.size(),
                  true, new Base64Decoder(new FileSink(os)));

  shared_ptr<PublicKey> publicKey;
  BOOST_REQUIRE_NO_THROW(publicKey = shared_ptr<PublicKey>(new PublicKey(os.buf()->buf(),
                                                                         os.buf()->size())));

  BOOST_CHECK_EQUAL(publicKey->getKeyType(), KEY_TYPE_RSA);

  Block digestBlock(RSA_DER_KEY_DIGEST, sizeof(RSA_DER_KEY_DIGEST));
  const Block& digest = publicKey->computeDigest();
  BOOST_CHECK_EQUAL_COLLECTIONS(digestBlock.wire(),
                                digestBlock.wire() + digestBlock.size(),
                                digest.wire(),
                                digest.wire() + digest.size());
}

BOOST_AUTO_TEST_CASE(ECDSA)
{
  using namespace CryptoPP;

  OBufferStream os;
  StringSource ss(reinterpret_cast<const uint8_t*>(ECDSA_DER.c_str()), ECDSA_DER.size(),
                  true, new Base64Decoder(new FileSink(os)));

  shared_ptr<PublicKey> publicKey;
  BOOST_REQUIRE_NO_THROW(publicKey = shared_ptr<PublicKey>(new PublicKey(os.buf()->buf(),
                                                                         os.buf()->size())));

  BOOST_CHECK_EQUAL(publicKey->getKeyType(), KEY_TYPE_ECDSA);

  Block digestBlock(ECDSA_DER_KEY_DIGEST, sizeof(ECDSA_DER_KEY_DIGEST));
  const Block& digest = publicKey->computeDigest();
  BOOST_CHECK_EQUAL_COLLECTIONS(digestBlock.wire(),
                                digestBlock.wire() + digestBlock.size(),
                                digest.wire(),
                                digest.wire() + digest.size());
}


BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace ndn
