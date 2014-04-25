/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
 * @author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 * BSD License, see COPYING for copyright and distribution information.
 */

// correct way to include ndn-cxx headers
// #include <ndn-cxx/face.hpp>
#include "face.hpp"

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions
namespace examples {

void
onData(Face& face,
       const Interest& interest, Data& data)
{
  std::cout << "I: " << interest.toUri() << std::endl;
  std::cout << "D: " << data.getName().toUri() << std::endl;
}

void
onTimeout(Face& face,
          const Interest& interest)
{
  std::cout << "Timeout" << std::endl;
}

int
main(int argc, char** argv)
{
  try {
    Interest i(Name("/localhost/testApp/randomData"));
    i.setScope(1);
    i.setInterestLifetime(time::milliseconds(1000));
    i.setMustBeFresh(true);

    Face face;
    face.expressInterest(i,
                         bind(onData, boost::ref(face), _1, _2),
                         bind(onTimeout, boost::ref(face), _1));

    // processEvents will block until the requested data received or timeout occurs
    face.processEvents();
  }
  catch(std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}

} // namespace examples
} // namespace ndn

int
main(int argc, char** argv)
{
  return ndn::examples::main(argc, argv);
}
