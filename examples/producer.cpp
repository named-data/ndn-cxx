/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
 * @author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 * BSD License, see COPYING for copyright and distribution information.
 */

// correct way to include NDN-CPP headers
// #include <ndn-cpp-dev/face.hpp>
// #include <ndn-cpp-dev/security/key-chain.hpp>

#include "face.hpp"
#include "security/key-chain.hpp"

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions
namespace examples {

class Producer
{
public:
  void
  onInterest(const Name& name, const Interest& interest)
  {
    std::cout << "<< I: " << interest << std::endl;

    // Create new name, based on Interest's name
    Name dataName(interest.getName());
    dataName
      .append("testApp") // add "testApp" component to Interest name
      .appendVersion();  // add "version" component (current UNIX timestamp in milliseconds)

    static const std::string content = "HELLO KITTY";

    // Create Data packet
    Data data;
    data.setName(dataName);
    data.setFreshnessPeriod(time::seconds(10));
    data.setContent(Block(content.c_str(), content.size()));

    // Sign Data packet with default identity
    m_keyChain.sign(data);
    // m_keyChain.sign(data, <identityName>);
    // m_keyChain.sign(data, <certificate>);

    // Return Data packet to the requester
    std::cout << ">> D: " << data << std::endl;
    m_face.put(data);
  }


  void
  onRegisterFailed(const Name& prefix, const std::string& reason)
  {
    std::cerr << "ERROR: Failed to register prefix in local hub's daemon (" << reason << ")" << std::endl;
    m_face.shutdown();
  }

  void
  run()
  {
    m_face.setInterestFilter("/localhost/testApp",
                             bind(&Producer::onInterest, this, _1, _2),
                             bind(&Producer::onRegisterFailed, this, _1, _2));
    m_face.processEvents();
  }

private:
  Face m_face;
  KeyChain m_keyChain;
};

} // namespace examples
} // namespace ndn

int
main(int argc, char** argv)
{
  try {
    ndn::examples::Producer producer;
    producer.run();
  }
  catch (std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
  return 0;
}
