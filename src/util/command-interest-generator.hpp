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
  while(timestamp <= m_lastTimestamp)
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
  while(timestamp <= m_lastTimestamp)
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
