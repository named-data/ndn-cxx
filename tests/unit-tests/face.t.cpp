/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#include "face.hpp"
#include "util/scheduler.hpp"
#include "security/key-chain.hpp"
#include "util/dummy-client-face.hpp"
#include "transport/tcp-transport.hpp"

#include "boost-test.hpp"
#include "unit-test-time-fixture.hpp"
#include "make-interest-data.hpp"

namespace ndn {
namespace tests {

using ndn::util::DummyClientFace;

class FaceFixture : public UnitTestTimeFixture
{
public:
  explicit
  FaceFixture(bool enableRegistrationReply = true)
    : face(io, { true, enableRegistrationReply })
  {
  }

public:
  DummyClientFace face;
};

class FacesNoRegistrationReplyFixture : public FaceFixture
{
public:
  FacesNoRegistrationReplyFixture()
    : FaceFixture(false)
  {
  }
};

BOOST_FIXTURE_TEST_SUITE(TestFace, FaceFixture)

BOOST_AUTO_TEST_CASE(ExpressInterestData)
{
  size_t nData = 0;
  face.expressInterest(Interest("/Hello/World", time::milliseconds(50)),
                       [&] (const Interest& i, const Data& d) {
                         BOOST_CHECK(i.getName().isPrefixOf(d.getName()));
                         BOOST_CHECK_EQUAL(i.getName(), "/Hello/World");
                         ++nData;
                       },
                       bind([] {
                         BOOST_FAIL("Unexpected Nack");
                       }),
                       bind([] {
                         BOOST_FAIL("Unexpected timeout");
                       }));

  advanceClocks(time::milliseconds(1), 40);

  face.receive(*util::makeData("/Bye/World/a"));
  face.receive(*util::makeData("/Hello/World/a"));

  advanceClocks(time::milliseconds(1), 100);

  BOOST_CHECK_EQUAL(nData, 1);
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 1);
  BOOST_CHECK_EQUAL(face.sentData.size(), 0);

  size_t nTimeouts = 0;
  face.expressInterest(Interest("/Hello/World/a/2", time::milliseconds(50)),
                       bind([]{}),
                       bind([]{}),
                       bind([&nTimeouts] {
                           ++nTimeouts;
                       }));
  advanceClocks(time::milliseconds(10), 100);
  BOOST_CHECK_EQUAL(nTimeouts, 1);
}

// test case for deprecated expressInterest implementation
BOOST_AUTO_TEST_CASE(DeprecatedExpressInterestData)
{
  size_t nData = 0;
  face.expressInterest(Interest("/Hello/World", time::milliseconds(50)),
                       [&] (const Interest& i, const Data& d) {
                         BOOST_CHECK(i.getName().isPrefixOf(d.getName()));
                         ++nData;
                       },
                       bind([] {
                           BOOST_FAIL("Unexpected timeout");
                         }));

  advanceClocks(time::milliseconds(1), 40);

  face.receive(*util::makeData("/Bye/World/a"));
  face.receive(*util::makeData("/Hello/World/a"));

  advanceClocks(time::milliseconds(1), 100);

  BOOST_CHECK_EQUAL(nData, 1);
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 1);
  BOOST_CHECK_EQUAL(face.sentData.size(), 0);

  face.expressInterest(Interest("/Hello/World/a", time::milliseconds(50)),
                       [&] (const Interest& i, const Data& d) {
                         BOOST_CHECK(i.getName().isPrefixOf(d.getName()));
                         ++nData;
                       },
                       bind([] {
                           BOOST_FAIL("Unexpected timeout");
                         }));
  advanceClocks(time::milliseconds(1), 40);
  face.receive(*util::makeData("/Hello/World/a/1/xxxxx"));

  advanceClocks(time::milliseconds(1), 100);

  BOOST_CHECK_EQUAL(nData, 2);
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 2);
  BOOST_CHECK_EQUAL(face.sentData.size(), 0);

  size_t nTimeouts = 0;
  face.expressInterest(Interest("/Hello/World/a/2", time::milliseconds(50)),
                       bind([]{}),
                       bind([&nTimeouts] {
                           ++nTimeouts;
                         }));
  advanceClocks(time::milliseconds(10), 100);
  BOOST_CHECK_EQUAL(nTimeouts, 1);
}

BOOST_AUTO_TEST_CASE(ExpressInterestTimeout)
{
  size_t nTimeouts = 0;
  face.expressInterest(Interest("/Hello/World", time::milliseconds(50)),
                       bind([] {
                         BOOST_FAIL("Unexpected ata");
                       }),
                       bind([] {
                         BOOST_FAIL("Unexpected Nack");
                       }),
                       [&nTimeouts] (const Interest& i) {
                         BOOST_CHECK_EQUAL(i.getName(), "/Hello/World");
                         ++nTimeouts;
                       });

  advanceClocks(time::milliseconds(10), 100);

  BOOST_CHECK_EQUAL(nTimeouts, 1);
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 1);
  BOOST_CHECK_EQUAL(face.sentData.size(), 0);
  BOOST_CHECK_EQUAL(face.sentNacks.size(), 0);
}

// test case for deprecated expressInterest implementation
BOOST_AUTO_TEST_CASE(DeprecatedExpressInterestTimeout)
{
  size_t nTimeouts = 0;
  face.expressInterest(Interest("/Hello/World", time::milliseconds(50)),
                       bind([] {
                           BOOST_FAIL("Unexpected data");
                         }),
                       bind([&nTimeouts] {
                           ++nTimeouts;
                         }));

  advanceClocks(time::milliseconds(10), 100);

  BOOST_CHECK_EQUAL(nTimeouts, 1);
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 1);
  BOOST_CHECK_EQUAL(face.sentData.size(), 0);
}

BOOST_AUTO_TEST_CASE(ExpressInterestNack)
{
  size_t nNacks = 0;

  Interest interest("/Hello/World", time::milliseconds(50));

  face.expressInterest(interest,
                       bind([] {
                         BOOST_FAIL("Unexpected Data");
                       }),
                       [&] (const Interest& i, const lp::Nack& n) {
                         BOOST_CHECK(i.getName().isPrefixOf(n.getInterest().getName()));
                         BOOST_CHECK_EQUAL(i.getName(), "/Hello/World");
                         BOOST_CHECK_EQUAL(n.getReason(), lp::NackReason::DUPLICATE);
                         ++nNacks;
                       },
                       bind([] {
                         BOOST_FAIL("Unexpected timeout");
                       }));

  advanceClocks(time::milliseconds(1), 40);

  lp::Nack nack(face.sentInterests[0]);
  nack.setReason(lp::NackReason::DUPLICATE);

  BOOST_CHECK_EQUAL(face.sentNacks.size(), 0);

  face.receive(nack);

  advanceClocks(time::milliseconds(1), 100);

  BOOST_CHECK_EQUAL(nNacks, 1);
  BOOST_CHECK_EQUAL(face.sentInterests.size(), 1);
}

BOOST_AUTO_TEST_CASE(RemovePendingInterest)
{
  const PendingInterestId* interestId =
    face.expressInterest(Interest("/Hello/World", time::milliseconds(50)),
                         bind([] {
                             BOOST_FAIL("Unexpected data");
                           }),
                         bind([] {
                             BOOST_FAIL("Unexpected timeout");
                           }));
  advanceClocks(time::milliseconds(10));

  face.removePendingInterest(interestId);
  advanceClocks(time::milliseconds(10));

  face.receive(*util::makeData("/Hello/World/!"));
  advanceClocks(time::milliseconds(10), 100);
}

BOOST_AUTO_TEST_CASE(removeAllPendingInterests)
{
  face.expressInterest(Interest("/Hello/World/0", time::milliseconds(50)),
                       bind([] { BOOST_FAIL("Unexpected data"); }),
                       bind([] { BOOST_FAIL("Unexpected nack"); }),
                       bind([] { BOOST_FAIL("Unexpected timeout"); }));

  face.expressInterest(Interest("/Hello/World/1", time::milliseconds(50)),
                       bind([] { BOOST_FAIL("Unexpected data"); }),
                       bind([] { BOOST_FAIL("Unexpected nack"); }),
                       bind([] { BOOST_FAIL("Unexpected timeout"); }));

  advanceClocks(time::milliseconds(10));

  face.removeAllPendingInterests();
  advanceClocks(time::milliseconds(10));

  BOOST_CHECK_EQUAL(face.getNPendingInterests(), 0);

  face.receive(*util::makeData("/Hello/World/0"));
  face.receive(*util::makeData("/Hello/World/1"));
  advanceClocks(time::milliseconds(10), 100);
}


BOOST_AUTO_TEST_CASE(SetUnsetInterestFilter)
{
  size_t nInterests = 0;
  size_t nRegs = 0;
  const RegisteredPrefixId* regPrefixId =
    face.setInterestFilter("/Hello/World",
                           bind([&nInterests] { ++nInterests; }),
                           bind([&nRegs] { ++nRegs; }),
                           bind([] {
                               BOOST_FAIL("Unexpected setInterestFilter failure");
                             }));
  advanceClocks(time::milliseconds(10), 10);
  BOOST_CHECK_EQUAL(nRegs, 1);
  BOOST_CHECK_EQUAL(nInterests, 0);

  face.receive(Interest("/Hello/World/!"));
  advanceClocks(time::milliseconds(10), 10);

  BOOST_CHECK_EQUAL(nRegs, 1);
  BOOST_CHECK_EQUAL(nInterests, 1);

  face.receive(Interest("/Bye/World/!"));
  advanceClocks(time::milliseconds(10000), 10);
  BOOST_CHECK_EQUAL(nInterests, 1);

  face.receive(Interest("/Hello/World/!/2"));
  advanceClocks(time::milliseconds(10), 10);
  BOOST_CHECK_EQUAL(nInterests, 2);

  // removing filter
  face.unsetInterestFilter(regPrefixId);
  advanceClocks(time::milliseconds(10), 10);

  face.receive(Interest("/Hello/World/!/3"));
  BOOST_CHECK_EQUAL(nInterests, 2);

  face.unsetInterestFilter(static_cast<const RegisteredPrefixId*>(0));
  advanceClocks(time::milliseconds(10), 10);

  face.unsetInterestFilter(static_cast<const InterestFilterId*>(0));
  advanceClocks(time::milliseconds(10), 10);
}

BOOST_AUTO_TEST_CASE(SetUnsetInterestFilterWithoutSucessCallback)
{
  size_t nInterests = 0;
  const RegisteredPrefixId* regPrefixId =
    face.setInterestFilter("/Hello/World",
                           bind([&nInterests] { ++nInterests; }),
                           bind([] {
                               BOOST_FAIL("Unexpected setInterestFilter failure");
                             }));
  advanceClocks(time::milliseconds(10), 10);
  BOOST_CHECK_EQUAL(nInterests, 0);

  face.receive(Interest("/Hello/World/!"));
  advanceClocks(time::milliseconds(10), 10);

  BOOST_CHECK_EQUAL(nInterests, 1);

  face.receive(Interest("/Bye/World/!"));
  advanceClocks(time::milliseconds(10000), 10);
  BOOST_CHECK_EQUAL(nInterests, 1);

  face.receive(Interest("/Hello/World/!/2"));
  advanceClocks(time::milliseconds(10), 10);
  BOOST_CHECK_EQUAL(nInterests, 2);

  // removing filter
  face.unsetInterestFilter(regPrefixId);
  advanceClocks(time::milliseconds(10), 10);

  face.receive(Interest("/Hello/World/!/3"));
  BOOST_CHECK_EQUAL(nInterests, 2);

  face.unsetInterestFilter(static_cast<const RegisteredPrefixId*>(0));
  advanceClocks(time::milliseconds(10), 10);

  face.unsetInterestFilter(static_cast<const InterestFilterId*>(0));
  advanceClocks(time::milliseconds(10), 10);
}

BOOST_FIXTURE_TEST_CASE(SetInterestFilterFail, FacesNoRegistrationReplyFixture)
{
  // don't enable registration reply
  size_t nRegFailed = 0;
  face.setInterestFilter("/Hello/World",
                         bind([] {
                             BOOST_FAIL("Unexpected Interest");
                           }),
                         bind([] {
                             BOOST_FAIL("Unexpected success of setInterestFilter");
                           }),
                         bind([&nRegFailed] {
                             ++nRegFailed;
                           }));

  advanceClocks(time::milliseconds(10), 10);
  BOOST_CHECK_EQUAL(nRegFailed, 0);

  advanceClocks(time::milliseconds(1000), 10);
  BOOST_CHECK_EQUAL(nRegFailed, 1);
}

BOOST_FIXTURE_TEST_CASE(SetInterestFilterFailWithoutSuccessCallback, FacesNoRegistrationReplyFixture)
{
  // don't enable registration reply
  size_t nRegFailed = 0;
  face.setInterestFilter("/Hello/World",
                         bind([] {
                             BOOST_FAIL("Unexpected Interest");
                           }),
                         bind([&nRegFailed] {
                             ++nRegFailed;
                           }));

  advanceClocks(time::milliseconds(10), 10);
  BOOST_CHECK_EQUAL(nRegFailed, 0);

  advanceClocks(time::milliseconds(1000), 10);
  BOOST_CHECK_EQUAL(nRegFailed, 1);
}

BOOST_AUTO_TEST_CASE(RegisterUnregisterPrefix)
{
  size_t nRegSuccesses = 0;
  const RegisteredPrefixId* regPrefixId =
    face.registerPrefix("/Hello/World",
                        bind([&nRegSuccesses] { ++nRegSuccesses; }),
                        bind([] {
                            BOOST_FAIL("Unexpected registerPrefix failure");
                          }));

  advanceClocks(time::milliseconds(10), 10);
  BOOST_CHECK_EQUAL(nRegSuccesses, 1);

  size_t nUnregSuccesses = 0;
  face.unregisterPrefix(regPrefixId,
                        bind([&nUnregSuccesses] { ++nUnregSuccesses; }),
                        bind([] {
                            BOOST_FAIL("Unexpected unregisterPrefix failure");
                          }));

  advanceClocks(time::milliseconds(10), 10);
  BOOST_CHECK_EQUAL(nUnregSuccesses, 1);
}

BOOST_FIXTURE_TEST_CASE(RegisterUnregisterPrefixFail, FacesNoRegistrationReplyFixture)
{
  size_t nRegFailures = 0;
  face.registerPrefix("/Hello/World",
                      bind([] {
                          BOOST_FAIL("Unexpected registerPrefix success");
                        }),
                      bind([&nRegFailures] { ++nRegFailures; }));

  advanceClocks(time::milliseconds(1000), 100);
  BOOST_CHECK_EQUAL(nRegFailures, 1);
}

BOOST_AUTO_TEST_CASE(SimilarFilters)
{
  size_t nInInterests1 = 0;
  face.setInterestFilter("/Hello/World",
                         bind([&nInInterests1] { ++nInInterests1; }),
                         RegisterPrefixSuccessCallback(),
                         bind([] {
                             BOOST_FAIL("Unexpected setInterestFilter failure");
                           }));

  size_t nInInterests2 = 0;
  face.setInterestFilter("/Hello",
                         bind([&nInInterests2] { ++nInInterests2; }),
                         RegisterPrefixSuccessCallback(),
                         bind([] {
                             BOOST_FAIL("Unexpected setInterestFilter failure");
                           }));

  size_t nInInterests3 = 0;
  face.setInterestFilter("/Los/Angeles/Lakers",
                         bind([&nInInterests3] { ++nInInterests3; }),
                         RegisterPrefixSuccessCallback(),
                         bind([] {
                             BOOST_FAIL("Unexpected setInterestFilter failure");
                           }));

  advanceClocks(time::milliseconds(10), 10);

  face.receive(Interest("/Hello/World/!"));
  advanceClocks(time::milliseconds(10), 10);

  BOOST_CHECK_EQUAL(nInInterests1, 1);
  BOOST_CHECK_EQUAL(nInInterests2, 1);
  BOOST_CHECK_EQUAL(nInInterests3, 0);
}

BOOST_AUTO_TEST_CASE(SetRegexFilterError)
{
  face.setInterestFilter(InterestFilter("/Hello/World", "<><b><c>?"),
                         [] (const Name&, const Interest&) {
                           BOOST_FAIL("InterestFilter::Error should have been triggered");
                         },
                         RegisterPrefixSuccessCallback(),
                         bind([] {
                             BOOST_FAIL("Unexpected setInterestFilter failure");
                           }));

  advanceClocks(time::milliseconds(10), 10);

  BOOST_REQUIRE_THROW(face.receive(Interest("/Hello/World/XXX/b/c")), InterestFilter::Error);
}

BOOST_AUTO_TEST_CASE(SetRegexFilter)
{
  size_t nInInterests = 0;
  face.setInterestFilter(InterestFilter("/Hello/World", "<><b><c>?"),
                         bind([&nInInterests] { ++nInInterests; }),
                         RegisterPrefixSuccessCallback(),
                         bind([] {
                             BOOST_FAIL("Unexpected setInterestFilter failure");
                           }));

  advanceClocks(time::milliseconds(10), 10);

  face.receive(Interest("/Hello/World/a"));     // shouldn't match
  BOOST_CHECK_EQUAL(nInInterests, 0);

  face.receive(Interest("/Hello/World/a/b"));   // should match
  BOOST_CHECK_EQUAL(nInInterests, 1);

  face.receive(Interest("/Hello/World/a/b/c")); // should match
  BOOST_CHECK_EQUAL(nInInterests, 2);

  face.receive(Interest("/Hello/World/a/b/d")); // should not match
  BOOST_CHECK_EQUAL(nInInterests, 2);
}

BOOST_AUTO_TEST_CASE(SetRegexFilterAndRegister)
{
  size_t nInInterests = 0;
  face.setInterestFilter(InterestFilter("/Hello/World", "<><b><c>?"),
                         bind([&nInInterests] { ++nInInterests; }));

  size_t nRegSuccesses = 0;
  face.registerPrefix("/Hello/World",
                      bind([&nRegSuccesses] { ++nRegSuccesses; }),
                      bind([] {
                          BOOST_FAIL("Unexpected setInterestFilter failure");
                        }));

  advanceClocks(time::milliseconds(10), 10);
  BOOST_CHECK_EQUAL(nRegSuccesses, 1);

  face.receive(Interest("/Hello/World/a")); // shouldn't match
  BOOST_CHECK_EQUAL(nInInterests, 0);

  face.receive(Interest("/Hello/World/a/b")); // should match
  BOOST_CHECK_EQUAL(nInInterests, 1);

  face.receive(Interest("/Hello/World/a/b/c")); // should match
  BOOST_CHECK_EQUAL(nInInterests, 2);

  face.receive(Interest("/Hello/World/a/b/d")); // should not match
  BOOST_CHECK_EQUAL(nInInterests, 2);
}

BOOST_FIXTURE_TEST_CASE(SetInterestFilterNoReg, FacesNoRegistrationReplyFixture) // Bug 2318
{
  // This behavior is specific to DummyClientFace.
  // Regular Face won't accept incoming packets until something is sent.

  int hit = 0;
  face.setInterestFilter(Name("/"), bind([&hit] { ++hit; }));
  face.processEvents(time::milliseconds(-1));

  auto interest = make_shared<Interest>("/A");
  face.receive(*interest);
  face.processEvents(time::milliseconds(-1));

  BOOST_CHECK_EQUAL(hit, 1);
}

BOOST_AUTO_TEST_CASE(ProcessEvents)
{
  face.processEvents(time::milliseconds(-1)); // io_service::reset()/poll() inside

  size_t nRegSuccesses = 0;
  face.registerPrefix("/Hello/World",
                      bind([&nRegSuccesses] { ++nRegSuccesses; }),
                      bind([] {
                          BOOST_FAIL("Unexpected setInterestFilter failure");
                        }));

  // io_service::poll() without reset
  face.getIoService().poll();
  BOOST_CHECK_EQUAL(nRegSuccesses, 0);

  face.processEvents(time::milliseconds(-1)); // io_service::reset()/poll() inside
  BOOST_CHECK_EQUAL(nRegSuccesses, 1);
}

BOOST_AUTO_TEST_CASE(PutNack)
{
  lp::Nack nack(Interest("/Hello/World", time::milliseconds(50)));
  nack.setReason(lp::NackReason::NO_ROUTE);

  BOOST_CHECK_EQUAL(face.sentNacks.size(), 0);

  face.put(nack);

  advanceClocks(time::milliseconds(10));

  BOOST_CHECK_EQUAL(face.sentNacks.size(), 1);
}

BOOST_AUTO_TEST_CASE(DestructionWithoutCancellingPendingInterests) // Bug #2518
{
  {
    DummyClientFace face2(io);
    face2.expressInterest(Interest("/Hello/World", time::milliseconds(50)),
                          bind([]{}), bind([]{}));
    advanceClocks(time::milliseconds(10), 10);
  }

  advanceClocks(time::milliseconds(10), 10);
  // should not segfault
}

BOOST_AUTO_TEST_CASE(FaceTransport)
{
  KeyChain keyChain;

  BOOST_CHECK(Face().getTransport() != nullptr);

  BOOST_CHECK(Face(shared_ptr<Transport>()).getTransport() != nullptr);
  BOOST_CHECK(Face(shared_ptr<Transport>(), io).getTransport() != nullptr);
  BOOST_CHECK(Face(shared_ptr<Transport>(), io, keyChain).getTransport() != nullptr);

  auto transport = make_shared<TcpTransport>("localhost", "6363"); // no real io operations will be scheduled
  BOOST_CHECK(Face(transport).getTransport() == transport);
  BOOST_CHECK(Face(transport, io).getTransport() == transport);
  BOOST_CHECK(Face(transport, io, keyChain).getTransport() == transport);
}

BOOST_AUTO_TEST_SUITE_END()

} // tests
} // namespace ndn
