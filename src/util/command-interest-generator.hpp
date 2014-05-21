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

#ifndef NDN_HELPERS_COMMAND_INTEREST_GENERATOR_HPP
#define NDN_HELPERS_COMMAND_INTEREST_GENERATOR_HPP

#include "../interest.hpp"
#include "../security/key-chain.hpp"
#include "../util/time.hpp"
#include "../util/random.hpp"

namespace ndn {

/**
 * @brief Helper class to generate CommandInterests
 *
 * @see http://redmine.named-data.net/projects/nfd/wiki/Command_Interests
 */
class CommandInterestGenerator
{
public:
  static const Name DEFAULT_CERTIFICATE_NAME;

  CommandInterestGenerator()
    : m_lastTimestamp(time::toUnixTimestamp(time::system_clock::now()))
  {
  }

  virtual
  ~CommandInterestGenerator()
  {
  }

  void
  generate(Interest& interest, const Name& certificateName = Name());

  void
  generateWithIdentity(Interest& interest, const Name& identity);

private:
  time::milliseconds m_lastTimestamp;
  KeyChain m_keyChain;
};


inline void
CommandInterestGenerator::generate(Interest& interest,
                                   const Name& certificateName /*= Name()*/)
{
  time::milliseconds timestamp = time::toUnixTimestamp(time::system_clock::now());
  while (timestamp <= m_lastTimestamp)
    {
      timestamp += time::milliseconds(1);
    }

  Name commandInterestName = interest.getName();
  commandInterestName
    .append(name::Component::fromNumber(timestamp.count()))
    .append(name::Component::fromNumber(random::generateWord64()));
  interest.setName(commandInterestName);

  if (certificateName.empty())
    m_keyChain.sign(interest);
  else
    m_keyChain.sign(interest, certificateName);

  m_lastTimestamp = timestamp;
}

inline void
CommandInterestGenerator::generateWithIdentity(Interest& interest, const Name& identity)
{
  time::milliseconds timestamp = time::toUnixTimestamp(time::system_clock::now());
  while (timestamp <= m_lastTimestamp)
    {
      timestamp += time::milliseconds(1);
    }

  Name commandInterestName = interest.getName();
  commandInterestName
    .append(name::Component::fromNumber(timestamp.count()))
    .append(name::Component::fromNumber(random::generateWord64()));
  interest.setName(commandInterestName);

  m_keyChain.signByIdentity(interest, identity);

  m_lastTimestamp = timestamp;
}


} // namespace ndn

#endif // NDN_HELPERS_COMMAND_INTEREST_GENERATOR_HPP
