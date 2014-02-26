/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
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
    : m_lastTimestamp(time::now() / 1000000)
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
  int64_t m_lastTimestamp;
  KeyChain m_keyChain;
};


inline void
CommandInterestGenerator::generate(Interest& interest, 
				   const Name& certificateName /*= Name()*/)
{
  int64_t timestamp = time::now() / 1000000;
  while(timestamp == m_lastTimestamp)
    {
      usleep(1000); //Guarantee unqiueness of timestamp
      timestamp = time::now();
    }

  Name commandInterestName = interest.getName();
  commandInterestName
    .append(name::Component::fromNumber(timestamp))
    .append(name::Component::fromNumber(random::generateWord64()));
  interest.setName(commandInterestName);

  if(certificateName.empty())
    m_keyChain.sign(interest);
  else
    m_keyChain.sign(interest, certificateName);

  m_lastTimestamp = timestamp;
}
  
inline void
CommandInterestGenerator::generateWithIdentity(Interest& interest, const Name& identity)
{
  int64_t timestamp = time::now() / 1000000;
  if(timestamp <= m_lastTimestamp)
    timestamp = m_lastTimestamp + 1;

  Name commandInterestName = interest.getName();
  commandInterestName
    .append(name::Component::fromNumber(timestamp))
    .append(name::Component::fromNumber(random::generateWord64()));
  interest.setName(commandInterestName);

  m_keyChain.signByIdentity(interest, identity);

  m_lastTimestamp = timestamp;
}


} // namespace ndn

#endif // NDN_HELPERS_COMMAND_INTEREST_GENERATOR_HPP
