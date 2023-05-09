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

#include "ndn-cxx/security/validation-state.hpp"
#include "ndn-cxx/security/validator.hpp"
#include "ndn-cxx/security/verification-helpers.hpp"
#include "ndn-cxx/util/logger.hpp"

namespace ndn::security {

NDN_LOG_INIT(ndn.security.ValidationState);

#define NDN_LOG_DEBUG_DEPTH(x) NDN_LOG_DEBUG(std::string(this->getDepth() + 1, '>') << " " << x)
#define NDN_LOG_TRACE_DEPTH(x) NDN_LOG_TRACE(std::string(this->getDepth() + 1, '>') << " " << x)

ValidationState::~ValidationState()
{
  NDN_LOG_TRACE(__func__);
  BOOST_ASSERT(!boost::logic::indeterminate(m_outcome));
}

bool
ValidationState::hasSeenCertificateName(const Name& certName)
{
  return !m_seenCertificateNames.insert(certName).second;
}

void
ValidationState::addCertificate(const Certificate& cert)
{
  m_certificateChain.push_front(cert);
}

const Certificate*
ValidationState::verifyCertificateChain(const Certificate& trustedCert)
{
  const Certificate* validatedCert = &trustedCert;
  for (auto it = m_certificateChain.begin(); it != m_certificateChain.end(); ++it) {
    const auto& certToValidate = *it;

    if (!verifySignature(certToValidate, *validatedCert)) {
      this->fail({ValidationError::INVALID_SIGNATURE, "Certificate " + certToValidate.getName().toUri()});
      m_certificateChain.erase(it, m_certificateChain.end());
      return nullptr;
    }

    NDN_LOG_TRACE_DEPTH("OK signature for certificate `" << certToValidate.getName() << "`");
    validatedCert = &certToValidate;
  }
  return validatedCert;
}

/////// DataValidationState

DataValidationState::DataValidationState(const Data& data,
                                         const DataValidationSuccessCallback& successCb,
                                         const DataValidationFailureCallback& failureCb)
  : m_data(data)
  , m_successCb(successCb)
  , m_failureCb(failureCb)
{
  BOOST_ASSERT(m_successCb != nullptr);
  BOOST_ASSERT(m_failureCb != nullptr);
}

DataValidationState::~DataValidationState()
{
  if (boost::logic::indeterminate(m_outcome)) {
    this->fail({ValidationError::IMPLEMENTATION_ERROR,
                "Validator/policy did not invoke success or failure callback"});
  }
}

void
DataValidationState::verifyOriginalPacket(const std::optional<Certificate>& trustedCert)
{
  if (verifySignature(m_data, trustedCert)) {
    NDN_LOG_TRACE_DEPTH("OK signature for data `" << m_data.getName() << "`");
    m_successCb(m_data);
    BOOST_ASSERT(boost::logic::indeterminate(m_outcome));
    m_outcome = true;
  }
  else {
    this->fail({ValidationError::INVALID_SIGNATURE, "Data " + m_data.getName().toUri()});
  }
}

void
DataValidationState::bypassValidation()
{
  NDN_LOG_TRACE_DEPTH("Signature verification bypassed for data `" << m_data.getName() << "`");
  m_successCb(m_data);
  BOOST_ASSERT(boost::logic::indeterminate(m_outcome));
  m_outcome = true;
}

void
DataValidationState::fail(const ValidationError& error)
{
  NDN_LOG_DEBUG_DEPTH(error);
  m_failureCb(m_data, error);
  BOOST_ASSERT(boost::logic::indeterminate(m_outcome));
  m_outcome = false;
}

/////// InterestValidationState

InterestValidationState::InterestValidationState(const Interest& interest,
                                                 const InterestValidationSuccessCallback& successCb,
                                                 const InterestValidationFailureCallback& failureCb)
  : m_interest(interest)
  , m_failureCb(failureCb)
{
  afterSuccess.connect(successCb);
  BOOST_ASSERT(successCb != nullptr);
  BOOST_ASSERT(m_failureCb != nullptr);
}

InterestValidationState::~InterestValidationState()
{
  if (boost::logic::indeterminate(m_outcome)) {
    this->fail({ValidationError::IMPLEMENTATION_ERROR,
                "Validator/policy did not invoke success or failure callback"});
  }
}

void
InterestValidationState::verifyOriginalPacket(const std::optional<Certificate>& trustedCert)
{
  if (verifySignature(m_interest, trustedCert)) {
    NDN_LOG_TRACE_DEPTH("OK signature for interest `" << m_interest.getName() << "`");
    this->afterSuccess(m_interest);
    BOOST_ASSERT(boost::logic::indeterminate(m_outcome));
    m_outcome = true;
  }
  else {
    this->fail({ValidationError::INVALID_SIGNATURE, "Interest " + m_interest.getName().toUri()});
  }
}

void
InterestValidationState::bypassValidation()
{
  NDN_LOG_TRACE_DEPTH("Signature verification bypassed for interest `" << m_interest.getName() << "`");
  this->afterSuccess(m_interest);
  BOOST_ASSERT(boost::logic::indeterminate(m_outcome));
  m_outcome = true;
}

void
InterestValidationState::fail(const ValidationError& error)
{
  NDN_LOG_DEBUG_DEPTH(error);
  m_failureCb(m_interest, error);
  BOOST_ASSERT(boost::logic::indeterminate(m_outcome));
  m_outcome = false;
}

} // namespace ndn::security
