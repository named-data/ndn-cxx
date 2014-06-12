/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

BOOST_AUTO_TEST_SUITE(SecurityTestPublicKey)

const std::string RSA_DER("MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuFoDcNtffwbfFix64fw0\
hI2tKMkFrc6Ex7yw0YLMK9vGE8lXOyBl/qXabow6RCz+GldmFN6E2Qhm1+AX3Zm5\
sj3H53/HPtzMefvMQ9X7U+lK8eNMWawpRzvBh4/36VrK/awlkNIVIQ9aXj6q6BVe\
zL+zWT/WYemLq/8A1/hHWiwCtfOH1xQhGqWHJzeSgwIgOOrzxTbRaCjhAb1u2TeV\
yx/I9H/DV+AqSHCaYbB92HDcDN0kqwSnUf5H1+osE9MR5DLBLhXdSiULSgxT3Or/\
y2QgsgUK59WrjhlVMPEiHHRs15NZJbL1uQFXjgScdEarohcY3dilqotineFZCeN8\
DwIDAQAB");
const std::string ECDSA_DER("MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAENZpqkPJDj8uhSpffOiCbvSYMLsGB\
1Eo/WU6mrexjGvduQXjqwon/eSHFI6EgHZk8L9KfiV5XVtVsk2g5wIpJVg==");

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
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
