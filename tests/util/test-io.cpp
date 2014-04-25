/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 */

#include "util/io.hpp"
#include "security/key-chain.hpp"

#include "boost-test.hpp"

namespace ndn {

BOOST_AUTO_TEST_SUITE(UtilTestIo)

BOOST_AUTO_TEST_CASE(Basic)
{
  KeyChainImpl<SecPublicInfoSqlite3, SecTpmFile> keychain;

  Name identity("/TestIO/Basic");
  identity.appendVersion();

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
