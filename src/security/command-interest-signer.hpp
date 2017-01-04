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

#ifndef NDN_SECURITY_COMMAND_INTEREST_SIGNER_HPP
#define NDN_SECURITY_COMMAND_INTEREST_SIGNER_HPP

#include "v2/key-chain.hpp"

namespace ndn {
namespace security {

/**
 * @brief Helper class to prepare command interest name
 *
 * The preparer adds timestamp and nonce name components to the supplied name.
 *
 * This class is primarily designed to be used as part of CommandInterestSigner, but can also
 * be using in an application that defines custom signing methods not support by the KeyChain
 * (such as HMAC-SHA1).
 *
 * @sa https://redmine.named-data.net/projects/ndn-cxx/wiki/CommandInterest
 */
class CommandInterestPreparer : noncopyable
{
public:
  CommandInterestPreparer();

  /**
   * @brief Prepare name of the CommandInterest
   *
   * This method appends the timestamp and nonce name components to the supplied name.
   */
  Name
  prepareCommandInterestName(Name name);

private:
  time::milliseconds m_lastUsedTimestamp;
};

/**
 * @brief Helper class to create command interests
 *
 * The signer adds timestamp and nonce name components to the supplied name, creates an
 * Interest, and signs it with the KeyChain.
 *
 * @sa https://redmine.named-data.net/projects/ndn-cxx/wiki/CommandInterest
 */
class CommandInterestSigner : private CommandInterestPreparer
{
public:
  explicit
  CommandInterestSigner(KeyChain& keyChain);

  /**
   * @brief Create CommandInterest
   *
   * This method appends the timestamp and nonce name components to the supplied name, create
   * an Interest object and signs it with the keychain.
   *
   * Note that signature of the command interest covers only Name of the interest.  Therefore,
   * other fields in the returned interest can be changed without breaking validity of the
   * signature, because s
   *
   * @sa https://redmine.named-data.net/projects/ndn-cxx/wiki/CommandInterest
   */
  Interest
  makeCommandInterest(const Name& name, const SigningInfo& params = KeyChain::getDefaultSigningInfo());

private:
  KeyChain& m_keyChain;
};

} // namespace security
} // namespace ndn


#endif // NDN_SECURITY_COMMAND_INTEREST_SIGNER_HPP
