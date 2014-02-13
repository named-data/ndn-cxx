/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include "command-interest-generator.hpp"
#include "../util/time.hpp"
#include "../util/random.hpp"

#include <unistd.h>

namespace ndn
{
const Name CommandInterestGenerator::DEFAULT_CERTIFICATE_NAME = Name();

CommandInterestGenerator::CommandInterestGenerator()
  : m_lastTimestamp(time::now() / 1000000)
{}

void
CommandInterestGenerator::generate(Interest& interest, 
				   const Name& certificateName /*= DEFAULT_CERTIFICATE_NAME*/)
{
  int64_t timestamp = time::now() / 1000000;
  while(timestamp == m_lastTimestamp)
    {
      usleep(1000); //Guarantee unqiueness of timestamp
      timestamp = time::now();
    }
  
  interest.getName().append(name::Component::fromNumber(timestamp)).append(name::Component::fromNumber(random::generateWord64()));

  if(certificateName == DEFAULT_CERTIFICATE_NAME)
    m_keyChain.sign(interest);
  else
    m_keyChain.sign(interest, certificateName);

  m_lastTimestamp = timestamp;
}
  
void
CommandInterestGenerator::generateWithIdentity(Interest& interest, const Name& identity)
{
  int64_t timestamp = time::now() / 1000000;
  if(timestamp <= m_lastTimestamp)
    timestamp = m_lastTimestamp + 1;
  
  interest.getName().append(name::Component::fromNumber(timestamp)).append(name::Component::fromNumber(random::generateWord64()));

  m_keyChain.signByIdentity(interest, identity);

  m_lastTimestamp = timestamp;
}

}//ndn
