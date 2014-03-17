/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 * See COPYING for copyright and distribution information.
 */

// correct way to include NDN-CPP headers
// #include <ndn-cpp-dev/face.hpp>
#include "face.hpp"
#include "util/scheduler.hpp"

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

void
delayedInterest(ndn::Face &face)
{
  std::cout << "One more Interest, delayed by the scheduler" << std::endl;
  
  ndn::Interest i(ndn::Name("/localhost/testApp/randomData"));
  i.setScope(1);
  i.setInterestLifetime(ndn::time::milliseconds(1000));
  i.setMustBeFresh(true);

  face.expressInterest(i,
                       ndn::bind(&onData, boost::ref(face), _1, _2),
                       ndn::bind(&onTimeout, boost::ref(face), _1));
}

int main()
{
  try {
    // Explicitly create io_service object, which can be shared between
    // Face and Scheduler
    ndn::shared_ptr<boost::asio::io_service> io =
      ndn::make_shared<boost::asio::io_service>();
    
    ndn::Interest i(ndn::Name("/localhost/testApp/randomData"));
    i.setScope(1);
    i.setInterestLifetime(ndn::time::seconds(1));
    i.setMustBeFresh(true);

    ndn::Face face(io);
    face.expressInterest(i,
                          ndn::bind(&onData, boost::ref(face), _1, _2),
                          ndn::bind(&onTimeout, boost::ref(face), _1));


    ndn::Scheduler scheduler(*io);
    scheduler.scheduleEvent(ndn::time::seconds(2),
                            ndn::bind(&delayedInterest, boost::ref(face)));
    
    io->run();
    
    // Alternatively, a helper face.processEvents() also can be called
    // processEvents will block until the requested data received or timeout occurs
    // face.processEvents();
  }
  catch(std::exception &e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
  return 0;
}
