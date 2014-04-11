/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
 * @author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 * BSD License, see COPYING for copyright and distribution information.
 */

// correct way to include NDN-CPP headers
// #include <ndn-cpp-dev/face.hpp>
// #include <ndn-cpp-dev/util/scheduler.hpp>
#include "face.hpp"
#include "util/scheduler.hpp"

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

void
delayedInterest(Face& face)
{
  std::cout << "One more Interest, delayed by the scheduler" << std::endl;

  Interest i(Name("/localhost/testApp/randomData"));
  i.setScope(1);
  i.setInterestLifetime(time::milliseconds(1000));
  i.setMustBeFresh(true);

  face.expressInterest(i,
                       bind(&onData, boost::ref(face), _1, _2),
                       bind(&onTimeout, boost::ref(face), _1));
}

int
main(int argc, char** argv)
{
  try {
    // Explicitly create io_service object, which can be shared between Face and Scheduler
    shared_ptr<boost::asio::io_service> io = make_shared<boost::asio::io_service>();

    Interest i(Name("/localhost/testApp/randomData"));
    i.setScope(1);
    i.setInterestLifetime(time::seconds(1));
    i.setMustBeFresh(true);

    // Create face with io_service object
    Face face(io);
    face.expressInterest(i,
                         bind(&onData, boost::ref(face), _1, _2),
                         bind(&onTimeout, boost::ref(face), _1));


    // Create scheduler object
    Scheduler scheduler(*io);

    // Schedule a new event
    scheduler.scheduleEvent(time::seconds(2),
                            bind(&delayedInterest, boost::ref(face)));

    // io->run() will block until all events finished or io->stop() is called
    io->run();

    // Alternatively, a helper face.processEvents() also can be called
    // processEvents will block until the requested data received or timeout occurs
    // face.processEvents();
  }
  catch(std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
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
