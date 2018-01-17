/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 */

#define BOOST_TEST_MAIN 1
#define BOOST_TEST_DYN_LINK 1
#define BOOST_TEST_MODULE ndn-cxx Integrated Tests (Face)

#include "face.hpp"
#include "transport/tcp-transport.hpp"
#include "transport/unix-transport.hpp"
#include "util/scheduler.hpp"

#include "boost-test.hpp"
#include "identity-management-fixture.hpp"
#include "make-interest-data.hpp"

#include <stdio.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <boost/mpl/vector.hpp>

namespace ndn {
namespace tests {

static Name
makeVeryLongName(Name prefix = Name())
{
  for (size_t i = 0; i <= MAX_NDN_PACKET_SIZE / 10; i++) {
    prefix.append("0123456789");
  }
  return prefix;
}

static std::string
executeCommand(const std::string& cmd)
{
  std::string output;
  char buf[256];
  FILE* pipe = popen(cmd.data(), "r");
  BOOST_REQUIRE_MESSAGE(pipe != nullptr, "cannot execute '" << cmd << "'");
  while (fgets(buf, sizeof(buf), pipe) != nullptr) {
    output += buf;
  }
  pclose(pipe);
  return output;
}

template<typename TransportType>
class FaceFixture : public IdentityManagementFixture
{
protected:
  FaceFixture()
    : face(TransportType::create(""), m_keyChain)
    , sched(face.getIoService())
  {
  }

  /** \brief Send an Interest from a secondary face
   *  \param delay scheduling delay before sending Interest
   *  \param interest the Interest
   *  \param[out] outcome the response, initially '?', 'D' for Data, 'N' for Nack, 'T' for timeout
   *  \return scheduled event id
   */
  EventId
  sendInterest(time::nanoseconds delay, const Interest& interest, char& outcome)
  {
    if (face2 == nullptr) {
      face2 = make_unique<Face>(TransportType::create(""), face.getIoService(), m_keyChain);
    }

    outcome = '?';
    return sched.scheduleEvent(delay, [this, interest, &outcome] {
      face2->expressInterest(interest,
        [&] (const Interest&, const Data&) { outcome = 'D'; },
        [&] (const Interest&, const lp::Nack&) { outcome = 'N'; },
        [&] (const Interest&) { outcome = 'T'; });
    });
  }

  EventId
  sendInterest(time::nanoseconds delay, const Interest& interest)
  {
    static char ignoredOutcome;
    return sendInterest(delay, interest, ignoredOutcome);
  }

  /** \brief Stop io_service after a delay
   *  \return scheduled event id
   */
  EventId
  terminateAfter(time::nanoseconds delay)
  {
    return sched.scheduleEvent(delay, [this] { face.getIoService().stop(); });
  }

protected:
  Face face;
  Scheduler sched;

  unique_ptr<Face> face2;
};

using Transports = boost::mpl::vector<UnixTransport, TcpTransport>;

BOOST_FIXTURE_TEST_SUITE(TestFace, FaceFixture<UnixTransport>)

BOOST_AUTO_TEST_SUITE(Consumer)

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ExpressInterestData, TransportType, Transports, FaceFixture<TransportType>)
{
  int nData = 0;
  this->face.expressInterest(Interest("/"),
    [&] (const Interest&, const Data&) { ++nData; },
    [] (const Interest&, const lp::Nack&) { BOOST_ERROR("unexpected Nack"); },
    [] (const Interest&) { BOOST_ERROR("unexpected timeout"); });

  this->face.processEvents();
  BOOST_CHECK_EQUAL(nData, 1);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ExpressInterestNack, TransportType, Transports, FaceFixture<TransportType>)
{
  int nNacks = 0;
  this->face.expressInterest(Interest("/localhost/non-existent-should-nack"),
    [] (const Interest&, const Data&) { BOOST_ERROR("unexpected Data"); },
    [&] (const Interest&, const lp::Nack&) { ++nNacks; },
    [] (const Interest&) { BOOST_ERROR("unexpected timeout"); });

  this->face.processEvents();
  BOOST_CHECK_EQUAL(nNacks, 1);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(ExpressInterestTimeout, TransportType, Transports, FaceFixture<TransportType>)
{
  // add route toward null face so Interest would timeout instead of getting Nacked
  executeCommand("nfdc route add /localhost/non-existent-should-timeout null://");
  std::this_thread::sleep_for(std::chrono::milliseconds(200)); // wait for FIB update to take effect

  int nTimeouts = 0;
  this->face.expressInterest(Interest("/localhost/non-existent-should-timeout", 1_s),
    [] (const Interest&, const Data&) { BOOST_ERROR("unexpected Data"); },
    [] (const Interest&, const lp::Nack&) { BOOST_ERROR("unexpected Nack"); },
    [&] (const Interest&) { ++nTimeouts; });

  this->face.processEvents();
  BOOST_CHECK_EQUAL(nTimeouts, 1);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(OversizedInterest, TransportType, Transports, FaceFixture<TransportType>)
{
  BOOST_CHECK_THROW(do {
    this->face.expressInterest(Interest(makeVeryLongName()), nullptr, nullptr, nullptr);
    this->face.processEvents();
  } while (false), Face::OversizedPacketError);
}

BOOST_AUTO_TEST_SUITE_END() // Consumer

BOOST_AUTO_TEST_SUITE(Producer)

BOOST_FIXTURE_TEST_CASE_TEMPLATE(RegisterUnregisterPrefix, TransportType, Transports, FaceFixture<TransportType>)
{
  this->terminateAfter(4_s);

  int nRegSuccess = 0, nUnregSuccess = 0;
  auto id = this->face.registerPrefix("/Hello/World",
    [&] (const Name&) { ++nRegSuccess; },
    [] (const Name&, const std::string& msg) { BOOST_ERROR("unexpected register prefix failure: " << msg); });

  this->sched.scheduleEvent(1_s, [&nRegSuccess] {
    BOOST_CHECK_EQUAL(nRegSuccess, 1);
    std::string output = executeCommand("nfdc route list | grep /Hello/World");
    BOOST_CHECK(!output.empty());
  });

  this->sched.scheduleEvent(2_s, [this, id, &nUnregSuccess] {
    this->face.unregisterPrefix(id,
      [&] { ++nUnregSuccess; },
      [] (const std::string& msg) { BOOST_ERROR("unexpected unregister prefix failure: " << msg); });
  });

  this->sched.scheduleEvent(3_s, [&nUnregSuccess] {
    BOOST_CHECK_EQUAL(nUnregSuccess, 1);

    // Boost.Test would fail if a child process exits with non-zero. http://stackoverflow.com/q/5325202
    std::string output = executeCommand("nfdc route list | grep /Hello/World || true");
    BOOST_CHECK(output.empty());
  });

  this->face.processEvents();
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(RegularFilter, TransportType, Transports, FaceFixture<TransportType>)
{
  this->terminateAfter(2_s);

  int nInterests1 = 0, nRegSuccess1 = 0, nRegSuccess2 = 0;
  this->face.setInterestFilter("/Hello/World",
    [&] (const InterestFilter&, const Interest&) { ++nInterests1; },
    [&] (const Name&) { ++nRegSuccess1; },
    [] (const Name&, const std::string& msg) { BOOST_ERROR("unexpected register prefix failure: " << msg); });
  this->face.setInterestFilter("/Los/Angeles/Lakers",
    [&] (const InterestFilter&, const Interest&) { BOOST_ERROR("unexpected Interest"); },
    [&] (const Name&) { ++nRegSuccess2; },
    [] (const Name&, const std::string& msg) { BOOST_ERROR("unexpected register prefix failure: " << msg); });

  this->sched.scheduleEvent(500_ms, [] {
    std::string output = executeCommand("nfdc route list | grep /Hello/World");
    BOOST_CHECK(!output.empty());
  });

  char interestOutcome;
  this->sendInterest(1_s, Interest("/Hello/World/regular", 50_ms), interestOutcome);

  this->face.processEvents();
  BOOST_CHECK_EQUAL(interestOutcome, 'T');
  BOOST_CHECK_EQUAL(nInterests1, 1);
  BOOST_CHECK_EQUAL(nRegSuccess1, 1);
  BOOST_CHECK_EQUAL(nRegSuccess2, 1);
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(RegexFilter, TransportType, Transports, FaceFixture<TransportType>)
{
  this->terminateAfter(2_s);

  int nRegSuccess = 0;
  std::set<Name> receivedInterests;
  this->face.setInterestFilter(InterestFilter("/Hello/World", "<><b><c>?"),
    [&] (const InterestFilter&, const Interest& interest) { receivedInterests.insert(interest.getName()); },
    [&] (const Name&) { ++nRegSuccess; },
    [] (const Name&, const std::string& msg) { BOOST_ERROR("unexpected register prefix failure: " << msg); });

  this->sched.scheduleEvent(700_ms, [] {
    std::string output = executeCommand("nfdc route list | grep /Hello/World");
    BOOST_CHECK(!output.empty());
  });

  this->sendInterest(200_ms, Interest("/Hello/World/a", 50_ms));
  this->sendInterest(300_ms, Interest("/Hello/World/a/b", 50_ms));
  this->sendInterest(400_ms, Interest("/Hello/World/a/b/c", 50_ms));
  this->sendInterest(500_ms, Interest("/Hello/World/a/b/d", 50_ms));

  this->face.processEvents();
  BOOST_CHECK_EQUAL(nRegSuccess, 1);
  std::set<Name> expectedInterests{"/Hello/World/a/b", "/Hello/World/a/b/c"};
  BOOST_CHECK_EQUAL_COLLECTIONS(receivedInterests.begin(), receivedInterests.end(),
                                expectedInterests.begin(), expectedInterests.end());
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(RegexFilterNoRegister, TransportType, Transports, FaceFixture<TransportType>)
{
  this->terminateAfter(2_s);

  // no Interest shall arrive because prefix isn't registered in forwarder
  this->face.setInterestFilter(InterestFilter("/Hello/World", "<><b><c>?"),
    [&] (const InterestFilter&, const Interest& interest) { BOOST_ERROR("unexpected Interest"); });

  this->sched.scheduleEvent(700_ms, [] {
    // Boost.Test would fail if a child process exits with non-zero. http://stackoverflow.com/q/5325202
    std::string output = executeCommand("nfdc route list | grep /Hello/World || true");
    BOOST_CHECK(output.empty());
  });

  this->sendInterest(200_ms, Interest("/Hello/World/a", 50_ms));
  this->sendInterest(300_ms, Interest("/Hello/World/a/b", 50_ms));
  this->sendInterest(400_ms, Interest("/Hello/World/a/b/c", 50_ms));
  this->sendInterest(500_ms, Interest("/Hello/World/a/b/d", 50_ms));

  this->face.processEvents();
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(PutDataNack, TransportType, Transports, FaceFixture<TransportType>)
{
  this->terminateAfter(2_s);

  this->face.setInterestFilter("/Hello/World",
    [&] (const InterestFilter&, const Interest& interest) {
      if (interest.getName().at(2) == name::Component("nack")) {
        this->face.put(makeNack(interest, lp::NackReason::NO_ROUTE));
      }
      else {
        this->face.put(*makeData(interest.getName()));
      }
    },
    nullptr,
    [] (const Name&, const std::string& msg) { BOOST_ERROR("unexpected register prefix failure: " << msg); });

  char outcome1, outcome2;
  this->sendInterest(700_ms, Interest("/Hello/World/data", 50_ms), outcome1);
  this->sendInterest(800_ms, Interest("/Hello/World/nack", 50_ms), outcome2);

  this->face.processEvents();
  BOOST_CHECK_EQUAL(outcome1, 'D');
  BOOST_CHECK_EQUAL(outcome2, 'N');
}

BOOST_FIXTURE_TEST_CASE_TEMPLATE(OversizedData, TransportType, Transports, FaceFixture<TransportType>)
{
  this->terminateAfter(2_s);

  this->face.setInterestFilter("/Hello/World",
    [&] (const InterestFilter&, const Interest& interest) {
      this->face.put(*makeData(makeVeryLongName(interest.getName())));
    },
    nullptr,
    [] (const Name&, const std::string& msg) { BOOST_ERROR("unexpected register prefix failure: " << msg); });

  this->sendInterest(1_s, Interest("/Hello/World/oversized", 50_ms));

  BOOST_CHECK_THROW(this->face.processEvents(), Face::OversizedPacketError);
}

BOOST_AUTO_TEST_SUITE_END() // Producer

BOOST_AUTO_TEST_SUITE(IoRoutine)

BOOST_AUTO_TEST_CASE(ShutdownWhileSendInProgress) // Bug #3136
{
  this->face.expressInterest(Interest("/Hello/World"), nullptr, nullptr, nullptr);
  this->face.processEvents(1_s);

  this->face.expressInterest(Interest("/Bye/World/1"), nullptr, nullptr, nullptr);
  this->face.expressInterest(Interest("/Bye/World/2"), nullptr, nullptr, nullptr);
  this->face.expressInterest(Interest("/Bye/World/3"), nullptr, nullptr, nullptr);
  this->face.shutdown();

  this->face.processEvents(1_s); // should not segfault
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(LargeDelayBetweenFaceConstructorAndProcessEvents) // Bug #2742
{
  std::this_thread::sleep_for(std::chrono::seconds(5)); // simulate setup workload
  this->face.processEvents(1_s); // should not throw
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(ProcessEventsBlocksForeverWhenNothingScheduled) // Bug #3957
{
  std::mutex m;
  std::condition_variable cv;
  bool processEventsFinished = false;

  std::thread faceThread([&] {
    this->face.processEvents();

    processEventsFinished = true;
    std::lock_guard<std::mutex> lk(m);
    cv.notify_one();
  });

  {
    std::unique_lock<std::mutex> lk(m);
    cv.wait_for(lk, std::chrono::seconds(5), [&] { return processEventsFinished; });
  }

  BOOST_CHECK_EQUAL(processEventsFinished, true);
  if (!processEventsFinished) {
    this->face.shutdown();
  }
  faceThread.join();
}

BOOST_AUTO_TEST_SUITE_END() // IoRoutine

BOOST_AUTO_TEST_SUITE_END() // TestFace

} // namespace tests
} // namespace ndn
