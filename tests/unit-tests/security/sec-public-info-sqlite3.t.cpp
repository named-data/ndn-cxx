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

#include "security/sec-public-info-sqlite3.hpp"
#include "security/key-chain.hpp"
#include "security/cryptopp.hpp"
#include "encoding/buffer-stream.hpp"
#include "util/time.hpp"

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include "boost-test.hpp"

namespace ndn {
namespace tests {

class PibTmpPathFixture
{
public:
  PibTmpPathFixture()
  {
    boost::system::error_code error;
    tmpPath = boost::filesystem::temp_directory_path(error);
    BOOST_REQUIRE(boost::system::errc::success == error.value());
    tmpPath /= boost::lexical_cast<std::string>(random::generateWord32());
  }

  ~PibTmpPathFixture()
  {
    boost::filesystem::remove_all(tmpPath);
  }

public:
  boost::filesystem::path tmpPath;
};

BOOST_AUTO_TEST_SUITE(SecuritySecPublicInfoSqlite3)

const std::string RSA_DER("MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuFoDcNtffwbfFix64fw0\
hI2tKMkFrc6Ex7yw0YLMK9vGE8lXOyBl/qXabow6RCz+GldmFN6E2Qhm1+AX3Zm5\
sj3H53/HPtzMefvMQ9X7U+lK8eNMWawpRzvBh4/36VrK/awlkNIVIQ9aXj6q6BVe\
zL+zWT/WYemLq/8A1/hHWiwCtfOH1xQhGqWHJzeSgwIgOOrzxTbRaCjhAb1u2TeV\
yx/I9H/DV+AqSHCaYbB92HDcDN0kqwSnUf5H1+osE9MR5DLBLhXdSiULSgxT3Or/\
y2QgsgUK59WrjhlVMPEiHHRs15NZJbL1uQFXjgScdEarohcY3dilqotineFZCeN8\
DwIDAQAB");
const std::string ECDSA_DER("MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAENZpqkPJDj8uhSpffOiCbvSYMLsGB\
1Eo/WU6mrexjGvduQXjqwon/eSHFI6EgHZk8L9KfiV5XVtVsk2g5wIpJVg==");

BOOST_FIXTURE_TEST_CASE(Basic, PibTmpPathFixture)
{
  SecPublicInfoSqlite3 pib(tmpPath.generic_string());

  BOOST_CHECK(pib.doesTableExist("Identity"));
  BOOST_CHECK(pib.doesTableExist("Key"));
  BOOST_CHECK(pib.doesTableExist("Certificate"));
}

BOOST_FIXTURE_TEST_CASE(TpmLocatorTest, PibTmpPathFixture)
{
  SecPublicInfoSqlite3 pib(tmpPath.generic_string());

  BOOST_REQUIRE_THROW(pib.getTpmLocator(), SecPublicInfo::Error);
  pib.addIdentity("/test/id1");
  BOOST_CHECK(pib.doesIdentityExist("/test/id1"));

  // Pib does not have tpmInfo set yet, setTpmInfo simply set the tpmInfo.
  std::string tpmLocator("tpm-file:");
  tpmLocator.append((tmpPath / "tpm").generic_string());
  pib.setTpmLocator(tpmLocator);
  BOOST_CHECK(pib.doesIdentityExist("/test/id1"));

  BOOST_REQUIRE_NO_THROW(pib.getTpmLocator());
  BOOST_CHECK_EQUAL(tpmLocator, pib.getTpmLocator());

  // Pib has tpmInfo set, set a different tpmInfo will reset Pib content.
  std::string tpmLocator3("tpm-osxkeychain:");
  pib.setTpmLocator(tpmLocator3);
  BOOST_CHECK(!pib.doesIdentityExist("/test/id1"));
}

BOOST_AUTO_TEST_CASE(KeyTypeRsa)
{
  using namespace CryptoPP;

  OBufferStream os;
  StringSource ss(reinterpret_cast<const uint8_t*>(RSA_DER.c_str()), RSA_DER.size(),
                  true, new Base64Decoder(new FileSink(os)));

  shared_ptr<PublicKey> rsaKey;
  BOOST_REQUIRE_NO_THROW(rsaKey = shared_ptr<PublicKey>(new PublicKey(os.buf()->buf(),
                                                                      os.buf()->size())));
  Name rsaKeyName("/TestSecPublicInfoSqlite3/KeyType/RSA/ksk-123");
  SecPublicInfoSqlite3 pib;
  pib.addKey(rsaKeyName, *rsaKey);

  BOOST_CHECK_EQUAL(KEY_TYPE_RSA, pib.getPublicKeyType(rsaKeyName));

  pib.deleteIdentityInfo(Name("/TestSecPublicInfoSqlite3/KeyType/RSA"));
}

BOOST_AUTO_TEST_CASE(KeyTypeEcdsa)
{
  using namespace CryptoPP;

  OBufferStream os;
  StringSource ss(reinterpret_cast<const uint8_t*>(ECDSA_DER.c_str()), ECDSA_DER.size(),
                  true, new Base64Decoder(new FileSink(os)));

  shared_ptr<PublicKey> ecdsaKey;
  BOOST_REQUIRE_NO_THROW(ecdsaKey = shared_ptr<PublicKey>(new PublicKey(os.buf()->buf(),
                                                                        os.buf()->size())));
  Name ecdsaKeyName("/TestSecPublicInfoSqlite3/KeyType/ECDSA/ksk-123");
  SecPublicInfoSqlite3 pib;
  pib.addKey(ecdsaKeyName, *ecdsaKey);

  BOOST_CHECK_EQUAL(KEY_TYPE_ECDSA, pib.getPublicKeyType(ecdsaKeyName));
  pib.deleteIdentityInfo(Name("/TestSecPublicInfoSqlite3/KeyType/ECDSA"));
}

BOOST_AUTO_TEST_CASE(KeyTypeNonExist)
{
  Name nullKeyName("/TestSecPublicInfoSqlite3/KeyType/Null");
  SecPublicInfoSqlite3 pib;

  BOOST_CHECK_EQUAL(KEY_TYPE_NULL, pib.getPublicKeyType(nullKeyName));

}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace ndn
