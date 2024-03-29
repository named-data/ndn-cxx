/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2023 Regents of the University of California.
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

#include "tests/unit/security/validator-fixture.hpp"

#include "ndn-cxx/security/additional-description.hpp"
#include "ndn-cxx/util/signal/scoped-connection.hpp"

#include <boost/asio/post.hpp>

namespace ndn::tests {

using namespace ndn::security;

ValidatorFixtureBase::ValidatorFixtureBase()
{
  processInterest = [this] (const Interest& interest) {
    auto cert = cache.find(interest);
    if (cert != nullptr) {
      BOOST_TEST_MESSAGE("ValidatorFixture processInterest " << interest << " reply " << cert->getName());
      face.receive(*cert);
    }
    else {
      BOOST_TEST_MESSAGE("ValidatorFixture processInterest " << interest << " no reply");
    }
  };
}

void
ValidatorFixtureBase::mockNetworkOperations()
{
  signal::ScopedConnection conn = face.onSendInterest.connect([this] (const Interest& interest) {
    if (processInterest) {
      boost::asio::post(m_io, [=] { processInterest(interest); });
    }
  });
  advanceClocks(s_mockPeriod, s_mockTimes);
}

void
ValidatorFixtureBase::rewindClockAfterValidation()
{
  m_systemClock->advance(s_mockPeriod * s_mockTimes * -1);
}

Identity
ValidatorFixtureBase::addSubCertificate(const Name& subIdentityName, const Identity& issuer)
{
  auto subId = m_keyChain.createIdentity(subIdentityName);
  auto cert = subId.getDefaultKey().getDefaultCertificate();
  cert.setName(cert.getKeyName()
               .append("parent")
               .appendVersion());

  SignatureInfo info;
  info.setValidityPeriod(ValidityPeriod::makeRelative(0_s, 90_days));

  AdditionalDescription description;
  description.set("type", "sub-certificate");
  info.addCustomTlv(description.wireEncode());

  m_keyChain.sign(cert, signingByIdentity(issuer).setSignatureInfo(info));
  m_keyChain.setDefaultCertificate(subId.getDefaultKey(), cert);

  return subId;
}

Name
InterestV02Pkt::makeName(Name name, KeyChain& keyChain)
{
  Interest interest(name);
  SigningInfo params;
  params.setSignedInterestFormat(SignedInterestFormat::V02);
  keyChain.sign(interest, params);
  return interest.getName();
}

Name
InterestV03Pkt::makeName(Name name, KeyChain& keyChain)
{
  Interest interest(name);
  SigningInfo params;
  params.setSignedInterestFormat(SignedInterestFormat::V03);
  keyChain.sign(interest, params);
  return interest.getName();
}

} // namespace ndn::tests
