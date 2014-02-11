/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 * See COPYING for copyright and distribution information.
 */

// correct way to include NDN-CPP headers
// #include <ndn-cpp-dev/face.hpp>
#include "face.hpp"

void
onData(ndn::Face &face,
       const ndn::Interest& interest, ndn::Data& data)
{
  std::cout << "I: " << interest.toUri() << std::endl;
  std::cout << "D: " << data.getName().toUri() << std::endl;
}

void
onTimeout(ndn::Face &face,
          const ndn::Interest& interest)
{
  std::cout << "Timeout" << std::endl;
}

int main()
{
  try {
    ndn::Interest i(ndn::Name("/localhost/testApp/randomData"));
    i.setScope(1);
    i.setInterestLifetime(1000);
    i.setMustBeFresh(true);

    ndn::Face face;
    face.expressInterest(i,
                          ndn::bind(onData, boost::ref(face), _1, _2),
                          ndn::bind(onTimeout, boost::ref(face), _1));

    // processEvents will block until the requested data received or timeout occurs
    face.processEvents();
  }
  catch(std::exception &e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
  return 0;
}
