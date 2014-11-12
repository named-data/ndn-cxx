/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#include "boost-test.hpp"
#include "util/dummy-client-face.hpp"

namespace ndn {
namespace tests {

using ndn::util::DummyClientFace;
using ndn::util::makeDummyClientFace;

class FacesFixture
{
public:
  FacesFixture(bool enableRegistrationReply = true)
    : nData(0)
    , nTimeouts(0)
    , nInInterests(0)
    , nInInterests2(0)
    , nRegSuccesses(0)
    , nRegFailures(0)
    , nUnregSuccesses(0)
    , nUnregFailures(0)
  {
    DummyClientFace::Options options { true, enableRegistrationReply };
    this->face = makeDummyClientFace(io, options);
  }

  void
  onData()
  {
    ++nData;
  }

  void
  onTimeout()
  {
    ++nTimeouts;
  }

  void
  onInterest(Face& face,
             const Name&, const Interest&)
  {
    ++nInInterests;
  }

  void
  onInterest2(Face& face,
              const Name&, const Interest&)
  {
    ++nInInterests2;
  }

  void
  onInterestRegex(Face& face,
                  const InterestFilter&, const Interest&)
  {
    ++nInInterests;
  }

  void
  onInterestRegexError(Face& face,
                       const Name&, const Interest&)
  {
    BOOST_FAIL("InterestFilter::Error should have been triggered");
  }

  void
  onRegSucceeded()
  {
    ++nRegSuccesses;
  }

  void
  onRegFailed()
  {
    ++nRegFailures;
  }

  void
  onUnregSucceeded()
  {
    ++nUnregSuccesses;
  }

  void
  onUnregFailed()
  {
    ++nUnregFailures;
  }

  shared_ptr<Data>
  makeData(const Name& name)
  {
    shared_ptr<Data> data = make_shared<Data>("/Hello/World/!");
    static KeyChain keyChain;
    keyChain.signWithSha256(*data);
    return data;
  }

  boost::asio::io_service io;
  shared_ptr<DummyClientFace> face;

  uint32_t nData;
  uint32_t nTimeouts;

  uint32_t nInInterests;
  uint32_t nInInterests2;
  uint32_t nRegSuccesses;
  uint32_t nRegFailures;
  uint32_t nUnregSuccesses;
  uint32_t nUnregFailures;
};

class FacesNoRegistrationReplyFixture : public FacesFixture
{
public:
  FacesNoRegistrationReplyFixture()
    : FacesFixture(false)
  {
  }
};

BOOST_FIXTURE_TEST_SUITE(TestFaces, FacesFixture)

BOOST_AUTO_TEST_CASE(ExpressInterestData)
{
  face->expressInterest(Interest("/Hello/World", time::milliseconds(50)),
                        bind(&FacesFixture::onData, this),
                        bind(&FacesFixture::onTimeout, this));

  BOOST_REQUIRE_NO_THROW(face->processEvents(time::milliseconds(-100)));

  face->receive(*makeData("/Hello/World/!"));
  BOOST_REQUIRE_NO_THROW(face->processEvents(time::milliseconds(200)));

  BOOST_CHECK_EQUAL(nData, 1);
  BOOST_CHECK_EQUAL(nTimeouts, 0);
}

BOOST_AUTO_TEST_CASE(ExpressInterestTimeout)
{
  face->expressInterest(Interest("/Hello/World", time::milliseconds(50)),
                        bind(&FacesFixture::onData, this),
                        bind(&FacesFixture::onTimeout, this));

  BOOST_REQUIRE_NO_THROW(face->processEvents(time::milliseconds(200)));

  BOOST_CHECK_EQUAL(nData, 0);
  BOOST_CHECK_EQUAL(nTimeouts, 1);
}

BOOST_AUTO_TEST_CASE(SetFilter)
{
  face->setInterestFilter("/Hello/World",
                          bind(&FacesFixture::onInterest, this, ref(*face), _1, _2),
                          RegisterPrefixSuccessCallback(),
                          bind(&FacesFixture::onRegFailed, this));

  BOOST_REQUIRE_NO_THROW(face->processEvents(time::milliseconds(-100)));

  face->receive(Interest("/Hello/World/!"));
  BOOST_REQUIRE_NO_THROW(face->processEvents(time::milliseconds(-100)));

  BOOST_CHECK_EQUAL(nRegFailures, 0);
  BOOST_CHECK_EQUAL(nInInterests, 1);
}

BOOST_FIXTURE_TEST_CASE(SetFilterFail, FacesNoRegistrationReplyFixture)
{
  // don't enable registration reply

  face->setInterestFilter("/Hello/World",
                          bind(&FacesFixture::onInterest, this, ref(*face), _1, _2),
                          RegisterPrefixSuccessCallback(),
                          bind(&FacesFixture::onRegFailed, this));

  BOOST_REQUIRE_NO_THROW(face->processEvents(time::milliseconds(11000)));

  BOOST_CHECK_EQUAL(nRegFailures, 1);
}

BOOST_AUTO_TEST_CASE(SetUnsetInterestFilter)
{
  const RegisteredPrefixId* regPrefixId =
    face->setInterestFilter(InterestFilter("/Hello/World"),
                            bind(&FacesFixture::onInterest, this,
                                 ref(*face), _1, _2),
                            RegisterPrefixSuccessCallback(),
                            bind(&FacesFixture::onRegFailed, this));
  BOOST_REQUIRE_NO_THROW(face->processEvents(time::milliseconds(100)));

  face->receive(Interest("/Hello/World/!"));
  BOOST_CHECK_EQUAL(nInInterests, 1);

  BOOST_REQUIRE_NO_THROW(face->processEvents(time::milliseconds(100)));

  face->receive(Interest("/Hello/World/!"));
  BOOST_CHECK_EQUAL(nInInterests, 2);

  face->unsetInterestFilter(regPrefixId);

  BOOST_REQUIRE_NO_THROW(face->processEvents(time::milliseconds(100)));

  face->receive(Interest("/Hello/World/!"));
  BOOST_CHECK_EQUAL(nInInterests, 2);

  BOOST_CHECK_NO_THROW(face->unsetInterestFilter(static_cast<const RegisteredPrefixId*>(0)));
  BOOST_REQUIRE_NO_THROW(face->processEvents(time::milliseconds(100)));

  BOOST_CHECK_NO_THROW(face->unsetInterestFilter(static_cast<const InterestFilterId*>(0)));
  BOOST_REQUIRE_NO_THROW(face->processEvents(time::milliseconds(100)));
}

BOOST_AUTO_TEST_CASE(RegisterUnregisterPrefix)
{
  const RegisteredPrefixId* regPrefixId =
    face->registerPrefix("/Hello/World",
                         bind(&FacesFixture::onRegSucceeded, this),
                         bind(&FacesFixture::onRegFailed, this));

  BOOST_REQUIRE_NO_THROW(face->processEvents(time::milliseconds(100)));
  BOOST_CHECK_EQUAL(nRegFailures, 0);
  BOOST_CHECK_EQUAL(nRegSuccesses, 1);

  face->unregisterPrefix(regPrefixId,
                         bind(&FacesFixture::onUnregSucceeded, this),
                         bind(&FacesFixture::onUnregFailed, this));

  BOOST_REQUIRE_NO_THROW(face->processEvents(time::milliseconds(100)));
  BOOST_CHECK_EQUAL(nUnregFailures, 0);
  BOOST_CHECK_EQUAL(nUnregSuccesses, 1);

}

BOOST_AUTO_TEST_CASE(SeTwoSimilarFilters)
{
  face->setInterestFilter("/Hello/World",
                          bind(&FacesFixture::onInterest, this, ref(*face), _1, _2),
                          RegisterPrefixSuccessCallback(),
                          bind(&FacesFixture::onRegFailed, this));

  face->setInterestFilter("/Hello",
                          bind(&FacesFixture::onInterest2, this, ref(*face), _1, _2),
                          RegisterPrefixSuccessCallback(),
                          bind(&FacesFixture::onRegFailed, this));

  BOOST_REQUIRE_NO_THROW(face->processEvents(time::milliseconds(-100)));

  face->receive(Interest("/Hello/World/!"));

  BOOST_REQUIRE_NO_THROW(face->processEvents(time::milliseconds(-100)));

  BOOST_CHECK_EQUAL(nRegFailures, 0);
  BOOST_CHECK_EQUAL(nInInterests, 1);
  BOOST_CHECK_EQUAL(nInInterests2, 1);
}

BOOST_AUTO_TEST_CASE(SetTwoDifferentFilters)
{
  face->setInterestFilter("/Hello/World",
                          bind(&FacesFixture::onInterest, this, ref(*face), _1, _2),
                          RegisterPrefixSuccessCallback(),
                          bind(&FacesFixture::onRegFailed, this));

  face->setInterestFilter("/Los/Angeles/Lakers",
                          bind(&FacesFixture::onInterest2, this, ref(*face), _1, _2),
                          RegisterPrefixSuccessCallback(),
                          bind(&FacesFixture::onRegFailed, this));

  BOOST_REQUIRE_NO_THROW(face->processEvents(time::milliseconds(-100)));

  face->receive(Interest("/Hello/World/!"));

  BOOST_REQUIRE_NO_THROW(face->processEvents(time::milliseconds(-100)));

  BOOST_CHECK_EQUAL(nRegFailures, 0);
  BOOST_CHECK_EQUAL(nInInterests, 1);
  BOOST_CHECK_EQUAL(nInInterests2, 0);
}

BOOST_AUTO_TEST_CASE(SetRegexFilterError)
{
  face->setInterestFilter(InterestFilter("/Hello/World", "<><b><c>?"),
                          bind(&FacesFixture::onInterestRegexError, this,
                               ref(*face), _1, _2),
                          RegisterPrefixSuccessCallback(),
                          bind(&FacesFixture::onRegFailed, this));

  BOOST_REQUIRE_NO_THROW(face->processEvents(time::milliseconds(-100)));

  BOOST_REQUIRE_THROW(face->receive(Interest("/Hello/World/XXX/b/c")), InterestFilter::Error);
}

BOOST_AUTO_TEST_CASE(SetRegexFilter)
{
  face->setInterestFilter(InterestFilter("/Hello/World", "<><b><c>?"),
                          bind(&FacesFixture::onInterestRegex, this,
                               ref(*face), _1, _2),
                          RegisterPrefixSuccessCallback(),
                          bind(&FacesFixture::onRegFailed, this));

  BOOST_REQUIRE_NO_THROW(face->processEvents(time::milliseconds(-100)));

  face->receive(Interest("/Hello/World/a"));     // shouldn't match
  BOOST_CHECK_EQUAL(nInInterests, 0);

  face->receive(Interest("/Hello/World/a/b"));   // should match
  BOOST_CHECK_EQUAL(nInInterests, 1);

  face->receive(Interest("/Hello/World/a/b/c")); // should match
  BOOST_CHECK_EQUAL(nInInterests, 2);

  face->receive(Interest("/Hello/World/a/b/d")); // should not match
  BOOST_CHECK_EQUAL(nInInterests, 2);
}


BOOST_AUTO_TEST_CASE(SetRegexFilterAndRegister)
{
  face->setInterestFilter(InterestFilter("/Hello/World", "<><b><c>?"),
                          bind(&FacesFixture::onInterestRegex, this,
                               ref(*face), _1, _2));

  face->registerPrefix("/Hello/World",
                       bind(&FacesFixture::onRegSucceeded, this),
                       bind(&FacesFixture::onRegFailed, this));

  BOOST_REQUIRE_NO_THROW(face->processEvents(time::milliseconds(100)));
  BOOST_CHECK_EQUAL(nRegFailures, 0);
  BOOST_CHECK_EQUAL(nRegSuccesses, 1);

  face->receive(Interest("/Hello/World/a")); // shouldn't match
  BOOST_CHECK_EQUAL(nInInterests, 0);

  face->receive(Interest("/Hello/World/a/b")); // should match
  BOOST_CHECK_EQUAL(nInInterests, 1);

  face->receive(Interest("/Hello/World/a/b/c")); // should match
  BOOST_CHECK_EQUAL(nInInterests, 2);

  face->receive(Interest("/Hello/World/a/b/d")); // should not match
  BOOST_CHECK_EQUAL(nInInterests, 2);
}

BOOST_AUTO_TEST_SUITE_END()

} // tests
} // namespace ndn
