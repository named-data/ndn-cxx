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

#include "util/io.hpp"
#include "security/key-chain.hpp"

#include "boost-test.hpp"

namespace ndn {

BOOST_AUTO_TEST_SUITE(UtilTestIo)

BOOST_AUTO_TEST_CASE(Basic)
{
  BOOST_REQUIRE_NO_THROW(KeyChain("sqlite3", "file"));
  KeyChain keyChain("sqlite3", "file");

  Name identity("/TestIO/Basic");
  identity.appendVersion();

  Name certName;
  BOOST_REQUIRE_NO_THROW(certName = keyChain.createIdentity(identity));
  shared_ptr<IdentityCertificate> idCert;
  BOOST_REQUIRE_NO_THROW(idCert = keyChain.getCertificate(certName));

  std::string file("/tmp/TestIO-Basic");
  io::save(*idCert, file);
  shared_ptr<IdentityCertificate> readCert = io::load<IdentityCertificate>(file);

  BOOST_CHECK(static_cast<bool>(readCert));
  BOOST_CHECK(idCert->getName() == readCert->getName());
  keyChain.deleteIdentity(identity);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
