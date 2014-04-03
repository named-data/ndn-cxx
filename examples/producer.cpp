/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 * See COPYING for copyright and distribution information.
 */

// correct way to include NDN-CPP headers
// #include <ndn-cpp-dev/face.hpp>
// #include <ndn-cpp-dev/security/key-chain.hpp>

#include "face.hpp"
#include "security/key-chain.hpp"

using namespace ndn;

class Producer
{
public:
  Producer()
  {}

  void
  onInterest(const Name& name, const Interest& interest)
  {
    std::cout << "<< I: " << interest << std::endl;

    ndn::Data data(ndn::Name(interest.getName()).append("testApp").appendVersion());
    data.setFreshnessPeriod(ndn::time::seconds(10));

    data.setContent((const uint8_t*)"HELLO KITTY", sizeof("HELLO KITTY"));

    keyChain_.sign(data);

    std::cout << ">> D: " << data << std::endl;
    face_.put(data);
  }


  void
  onRegisterFailed (const ndn::Name& prefix, const std::string& reason)
  {
    std::cerr << "ERROR: Failed to register prefix in local hub's daemon (" << reason << ")" << std::endl;
    face_.shutdown ();
  }

  void
  listen()
  {
    face_.setInterestFilter("/localhost/testApp",
                            func_lib::bind(&Producer::onInterest, this, _1, _2),
                            func_lib::bind(&Producer::onRegisterFailed, this, _1, _2));
    face_.processEvents();
  }

private:
  ndn::Face face_;
  KeyChain keyChain_;

  Buffer ndndId_;
};

int main()
{
  try {
    Producer producer;
    producer.listen();
  }
  catch(std::exception &e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
  return 0;
}
