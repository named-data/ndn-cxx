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

#ifndef NDN_TESTS_IDENTITY_MANAGEMENT_FIXTURE_HPP
#define NDN_TESTS_IDENTITY_MANAGEMENT_FIXTURE_HPP

#include "security/key-chain.hpp"
#include <vector>

#include "boost-test.hpp"

namespace ndn {
namespace tests {

/**
 * @brief IdentityManagementFixture is a test suite level fixture.
 * Test cases in the suite can use this fixture to create identities.
 * Identities added via addIdentity method are automatically deleted
 * during test teardown.
 */
class IdentityManagementFixture
{
public:
  IdentityManagementFixture();

  ~IdentityManagementFixture();

  /// @brief add identity, return true if succeed.
  bool
  addIdentity(const Name& identity, const KeyParams& params = KeyChain::DEFAULT_KEY_PARAMS);

  /**
   *  @brief save identity certificate to a file
   *  @param identity identity name
   *  @param filename file name, should be writable
   *  @param wantAdd if true, add new identity when necessary
   *  @return whether successful
   */
  bool
  saveIdentityCertificate(const Name& identity, const std::string& filename, bool wantAdd = false);

protected:
  KeyChain m_keyChain;

private:
  std::vector<Name> m_identities;
  std::vector<std::string> m_certFiles;
};

} // namespace tests
} // namespace ndn

#endif // NDN_TESTS_IDENTITY_MANAGEMENT_FIXTURE_HPP
