/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_COMMAND_INTEREST_GENERATOR_HPP
#define NDN_COMMAND_INTEREST_GENERATOR_HPP

#include "../interest.hpp"
#include "../security/key-chain.hpp"


namespace ndn
{

class CommandInterestGenerator
{
public:
  static const Name DEFAULT_CERTIFICATE_NAME;

  CommandInterestGenerator();

  virtual
  ~CommandInterestGenerator() {}

  void
  generate(Interest& interest, const Name& certificateName = DEFAULT_CERTIFICATE_NAME);
  
  void
  generateWithIdentity(Interest& interest, const Name& identity);
  
private:
  int64_t m_lastTimestamp;
  KeyChain m_keyChain;
};

}//ndn

#endif
