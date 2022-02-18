/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#ifndef NDN_CXX_TESTS_UNIT_SECURITY_VALIDATOR_FIXTURE_HPP
#define NDN_CXX_TESTS_UNIT_SECURITY_VALIDATOR_FIXTURE_HPP

#include "ndn-cxx/security/validator.hpp"
#include "ndn-cxx/security/certificate-fetcher-from-network.hpp"
#include "ndn-cxx/util/dummy-client-face.hpp"

#include "tests/boost-test.hpp"
#include "tests/unit/io-key-chain-fixture.hpp"

#include <boost/lexical_cast.hpp>

namespace ndn {
namespace security {
inline namespace v2 {
namespace tests {

class ValidatorFixtureBase : public ndn::tests::IoKeyChainFixture
{
protected:
  ValidatorFixtureBase();

  void
  mockNetworkOperations();

  /** \brief undo clock advancement of mockNetworkOperations()
   */
  void
  rewindClockAfterValidation()
  {
    m_systemClock->advance(s_mockPeriod * s_mockTimes * -1);
  }

  /**
   * @brief Issues a certificate for @p subIdentityName signed by @p issuer
   *
   * If the identity does not exist, it is created.
   * A new key is generated as the default key for the identity.
   * A default certificate for the key is signed by the issuer using its default certificate.
   *
   * @return The sub-identity
   */
  Identity
  addSubCertificate(const Name& subIdentityName, const Identity& issuer);

protected:
  util::DummyClientFace face{m_io, {true, true}};
  std::function<void(const Interest&)> processInterest;
  CertificateCache cache{100_days};
  ValidationError lastError{ValidationError::Code::NO_ERROR};

private:
  const static time::milliseconds s_mockPeriod;
  const static int s_mockTimes;
};

template<class ValidationPolicyT, class CertificateFetcherT = CertificateFetcherFromNetwork>
class ValidatorFixture : public ValidatorFixtureBase
{
protected:
  ValidatorFixture()
    : validator(make_unique<ValidationPolicyT>(), make_unique<CertificateFetcherT>(face))
    , policy(static_cast<ValidationPolicyT&>(validator.getPolicy()))
  {
  }

  template<class Packet>
  void
  validate(const Packet& packet, const std::string& msg, bool expectSuccess, int line)
  {
    std::string detailedInfo = msg + " on line " + to_string(line);
    size_t nCallbacks = 0;
    this->validator.validate(packet,
      [&] (const Packet&) {
        ++nCallbacks;
        BOOST_CHECK_MESSAGE(expectSuccess,
                            (expectSuccess ? "OK: " : "FAILED: ") + detailedInfo);
      },
      [&] (const Packet&, const ValidationError& error) {
        lastError = error;
        ++nCallbacks;
        BOOST_CHECK_MESSAGE(!expectSuccess,
                            (!expectSuccess ? "OK: " : "FAILED: ") + detailedInfo +
                            (expectSuccess ? " (" + boost::lexical_cast<std::string>(error) + ")" : ""));
      });

    mockNetworkOperations();
    BOOST_CHECK_EQUAL(nCallbacks, 1);
  }

protected:
  Validator validator;
  ValidationPolicyT& policy;
};

template<class ValidationPolicyT, class CertificateFetcherT = CertificateFetcherFromNetwork>
class HierarchicalValidatorFixture : public ValidatorFixture<ValidationPolicyT, CertificateFetcherT>
{
protected:
  HierarchicalValidatorFixture()
  {
    identity = this->m_keyChain.createIdentity("/Security/ValidatorFixture");
    subIdentity = this->addSubCertificate("/Security/ValidatorFixture/Sub1", identity);
    subSelfSignedIdentity = this->m_keyChain.createIdentity("/Security/ValidatorFixture/Sub1/Sub2");
    otherIdentity = this->m_keyChain.createIdentity("/Security/OtherIdentity");

    this->validator.loadAnchor("", Certificate(identity.getDefaultKey().getDefaultCertificate()));

    this->cache.insert(identity.getDefaultKey().getDefaultCertificate());
    this->cache.insert(subIdentity.getDefaultKey().getDefaultCertificate());
    this->cache.insert(subSelfSignedIdentity.getDefaultKey().getDefaultCertificate());
    this->cache.insert(otherIdentity.getDefaultKey().getDefaultCertificate());
  }

protected:
  Identity identity;
  Identity subIdentity;
  Identity subSelfSignedIdentity;
  Identity otherIdentity;
};

#define VALIDATE_SUCCESS(packet, message) this->validate(packet, message, true, __LINE__)
#define VALIDATE_FAILURE(packet, message) this->validate(packet, message, false, __LINE__)

class DummyValidationState : public ValidationState
{
public:
  ~DummyValidationState() override
  {
    m_outcome = false;
  }

  void
  fail(const ValidationError&) override
  {
    m_outcome = false;
  }

private:
  void
  verifyOriginalPacket(const optional<Certificate>&) override
  {
    // do nothing
  }

  void
  bypassValidation() override
  {
    // do nothing
  }
};

struct DataPkt
{
  static constexpr uint32_t
  getType()
  {
    return tlv::Data;
  }

  static Name
  makeName(Name name, KeyChain&)
  {
    return name;
  }

  static Data
  makePacket(const Name& name)
  {
    return Data(name);
  }

  static shared_ptr<ValidationState>
  makeState()
  {
    return make_shared<DummyValidationState>();
  }
};

struct InterestV02Pkt
{
  static constexpr uint32_t
  getType()
  {
    return tlv::Interest;
  }

  static Name
  makeName(Name name, KeyChain& keyChain);

  static Interest
  makePacket(const Name& name)
  {
    return Interest(name);
  }

  static shared_ptr<ValidationState>
  makeState()
  {
    auto state = make_shared<DummyValidationState>();
    state->setTag(make_shared<SignedInterestFormatTag>(SignedInterestFormat::V02));
    return state;
  }
};

struct InterestV03Pkt
{
  static constexpr uint32_t
  getType()
  {
    return tlv::Interest;
  }

  static Name
  makeName(Name name, KeyChain& keyChain);

  static Interest
  makePacket(const Name& name)
  {
    return Interest(name);
  }

  static shared_ptr<ValidationState>
  makeState()
  {
    auto state = make_shared<DummyValidationState>();
    state->setTag(make_shared<SignedInterestFormatTag>(SignedInterestFormat::V03));
    return state;
  }
};

} // namespace tests
} // inline namespace v2
} // namespace security
} // namespace ndn

#endif // NDN_CXX_TESTS_UNIT_SECURITY_VALIDATOR_FIXTURE_HPP
