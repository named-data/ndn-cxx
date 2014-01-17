/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <ndn-cpp/face.hpp>

#include <ndn-cpp/security/key-chain.hpp>

#if NDN_CPP_HAVE_CXX11
// In the std library, the placeholders are in a different namespace than boost.
using namespace ndn::func_lib::placeholders;
#endif

using namespace ndn;

class Producer
{
public:
  ////////////////////////////////////////////////////////////////////////////////////////
  // CREATE TEST KEYCHAIN (THIS CODE IS ONLY FOR DEBUGGING, NOT TO BE USED IN REAL APPS //
  ////////////////////////////////////////////////////////////////////////////////////////
  Producer()
    : keyChain_()
  {
  }
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////

  void
  onInterest(const ptr_lib::shared_ptr<const Name> &name, const ptr_lib::shared_ptr<const Interest> &interest)
  {
    std::cout << "<< I: " << *interest << std::endl;
    
    ndn::Data data(ndn::Name(interest->getName()).append("testApp").appendVersion());
    data.setFreshnessPeriod(1000); // 10 sec

    data.setContent((const uint8_t*)"HELLO KITTY", sizeof("HELLO KITTY"));

    keyChain_.sign(data);

    std::cout << ">> D: " << data << std::endl;
    face_.put(data);
  }

  void
  onRegisterFailed(const ptr_lib::shared_ptr<const Name>&)
  {
    std::cerr << "ERROR: Failed to register prefix in local hub's daemon" << std::endl;
    face_.shutdown();
  }
  
  void
  listen()
  {
    face_.setInterestFilter("/localhost/testApp",
                            func_lib::bind(&Producer::onInterest, this, _1, _2),
                            func_lib::bind(&Producer::onRegisterFailed, this, _1));
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
