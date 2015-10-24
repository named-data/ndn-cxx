/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#ifndef NDN_TESTS_IDENTITY_MANAGEMENT_FIXTURE_HPP
#define NDN_TESTS_IDENTITY_MANAGEMENT_FIXTURE_HPP

#include "security/v1/key-chain.hpp"
#include "security/v2/key-chain.hpp"
#include "security/signing-helpers.hpp"
#include <vector>

#include "boost-test.hpp"
#include "test-home-fixture.hpp"

namespace ndn {
namespace tests {

class IdentityManagementBaseFixture : public TestHomeFixture<DefaultPibDir>
{
public:
  ~IdentityManagementBaseFixture();

  bool
  saveCertToFile(const Data& obj, const std::string& filename);

protected:
  std::set<Name> m_identities;
  std::set<std::string> m_certFiles;
};

/**
 * @brief A test suite level fixture to help with identity management
 *
 * Test cases in the suite can use this fixture to create identities.  Identities,
 * certificates, and saved certificates are automatically removed during test teardown.
 *
 * @deprecated Use IdentityManagementV2Fixture
 */
class IdentityManagementV1Fixture : public IdentityManagementBaseFixture
{
public:
  ~IdentityManagementV1Fixture();

  /**
   * @brief Add identity
   * @return name of the created self-signed certificate
   */
  Name
  addIdentity(const Name& identity, const KeyParams& params = security::v1::KeyChain::DEFAULT_KEY_PARAMS);

  /**
   *  @brief save identity certificate to a file
   *  @param identity certificate name
   *  @param filename file name, should be writable
   *  @return whether successful
   */
  bool
  saveIdentityCertificate(const Name& certName, const std::string& filename);

  /**
   * @brief issue a certificate for \p subIdentity signed by \p issuer
   *
   *  If identity does not exist, it is created.
   *  A new key is generated as the default key for identity.
   *  A default certificate for the key is signed by the issuer using its default certificate.
   *
   *  @return whether success
   */
  bool
  addSubCertificate(const Name& subIdentity, const Name& issuer,
                    const KeyParams& params = security::v1::KeyChain::DEFAULT_KEY_PARAMS);

protected:
  security::v1::KeyChain m_keyChain;
};

/**
 * @brief A test suite level fixture to help with identity management
 *
 * Test cases in the suite can use this fixture to create identities.  Identities,
 * certificates, and saved certificates are automatically removed during test teardown.
 */
class IdentityManagementV2Fixture : public IdentityManagementBaseFixture
{
public:
  IdentityManagementV2Fixture();

  /**
   * @brief Add identity @p identityName
   * @return name of the created self-signed certificate
   */
  security::Identity
  addIdentity(const Name& identityName, const KeyParams& params = security::v2::KeyChain::getDefaultKeyParams());

  /**
   *  @brief Save identity certificate to a file
   *  @param identity identity
   *  @param filename file name, should be writable
   *  @return whether successful
   */
  bool
  saveIdentityCertificate(const security::Identity& identity, const std::string& filename);

  /**
   * @brief Issue a certificate for \p subIdentityName signed by \p issuer
   *
   *  If identity does not exist, it is created.
   *  A new key is generated as the default key for identity.
   *  A default certificate for the key is signed by the issuer using its default certificate.
   *
   *  @return the sub identity
   */
  security::Identity
  addSubCertificate(const Name& subIdentityName, const security::Identity& issuer,
                    const KeyParams& params = security::v2::KeyChain::getDefaultKeyParams());

  /**
   * @brief Add a self-signed certificate to @p key with issuer ID @p issuer
   */
  security::v2::Certificate
  addCertificate(const security::Key& key, const std::string& issuer);

protected:
  security::v2::KeyChain m_keyChain;
};

using IdentityManagementFixture = IdentityManagementV2Fixture;

} // namespace tests
} // namespace ndn

#endif // NDN_TESTS_IDENTITY_MANAGEMENT_FIXTURE_HPP
