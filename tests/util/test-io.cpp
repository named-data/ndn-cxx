/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include <boost/test/unit_test.hpp>

#include "util/io.hpp"
#include "security/key-chain.hpp"

namespace ndn {

BOOST_AUTO_TEST_SUITE(TestIO)

BOOST_AUTO_TEST_CASE (Basic)
{
  KeyChainImpl<SecPublicInfoSqlite3, SecTpmFile> keychain;

  Name identity("/TestIO/Basic/" + boost::lexical_cast<std::string>(time::now()));
  Name certName;
  BOOST_REQUIRE_NO_THROW(certName = keychain.createIdentity(identity));
  shared_ptr<IdentityCertificate> idCert;
  BOOST_REQUIRE_NO_THROW(idCert = keychain.getCertificate(certName));

  std::string file("/tmp/TestIO-Basic");
  io::save(*idCert, file);
  shared_ptr<IdentityCertificate> readCert = io::load<IdentityCertificate>(file);

  BOOST_CHECK(static_cast<bool>(readCert));
  BOOST_CHECK(idCert->getName() == readCert->getName());
  keychain.deleteIdentity(identity);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
