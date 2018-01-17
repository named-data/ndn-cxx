/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#include "command-interest-signer.hpp"
#include "../util/random.hpp"

namespace ndn {
namespace security {

CommandInterestPreparer::CommandInterestPreparer()
  : m_lastUsedTimestamp(0)
{
}

Name
CommandInterestPreparer::prepareCommandInterestName(Name name)
{
  time::milliseconds timestamp = time::toUnixTimestamp(time::system_clock::now());
  if (timestamp <= m_lastUsedTimestamp) {
    timestamp = m_lastUsedTimestamp + 1_ms;
  }
  m_lastUsedTimestamp = timestamp;

  name
    .append(name::Component::fromNumber(timestamp.count()))
    .append(name::Component::fromNumber(random::generateWord64())) // nonce
    ;

  return name;
}

CommandInterestSigner::CommandInterestSigner(KeyChain& keyChain)
  : m_keyChain(keyChain)
{
}

Interest
CommandInterestSigner::makeCommandInterest(const Name& name, const SigningInfo& params)
{
  Interest commandInterest(prepareCommandInterestName(name));
  m_keyChain.sign(commandInterest, params);
  return commandInterest;
}

} // namespace security
} // namespace ndn
