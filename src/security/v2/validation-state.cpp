/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2017 Regents of the University of California.
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

#include "validation-state.hpp"
#include "validator.hpp"
#include "../verification-helpers.hpp"
#include "util/logger.hpp"

namespace ndn {
namespace security {
namespace v2 {

NDN_LOG_INIT(ndn.security.v2.ValidationState);

#define NDN_LOG_DEBUG_DEPTH(x) NDN_LOG_DEBUG(std::string(this->getDepth() + 1, '>') << " " << x)
#define NDN_LOG_TRACE_DEPTH(x) NDN_LOG_TRACE(std::string(this->getDepth() + 1, '>') << " " << x)

ValidationState::ValidationState()
  : m_outcome(boost::logic::indeterminate)
{
}

ValidationState::~ValidationState()
{
  NDN_LOG_TRACE(__func__);
  BOOST_ASSERT(!boost::logic::indeterminate(m_outcome));
}

size_t
ValidationState::getDepth() const
{
  return m_certificateChain.size();
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
      this->fail({ValidationError::Code::INVALID_SIGNATURE, "Invalid signature of certificate `" +
                  certToValidate.getName().toUri() + "`"});
      m_certificateChain.erase(it, m_certificateChain.end());
      return nullptr;
    }
    else {
      NDN_LOG_TRACE_DEPTH("OK signature for certificate `" << certToValidate.getName() << "`");
      validatedCert = &certToValidate;
    }
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
    this->fail({ValidationError::Code::IMPLEMENTATION_ERROR,
                "Validator/policy did not invoke success or failure callback"});
  }
}

void
DataValidationState::verifyOriginalPacket(const Certificate& trustedCert)
{
  if (verifySignature(m_data, trustedCert)) {
    NDN_LOG_TRACE_DEPTH("OK signature for data `" << m_data.getName() << "`");
    m_successCb(m_data);
    BOOST_ASSERT(boost::logic::indeterminate(m_outcome));
    m_outcome = true;
  }
  else {
    this->fail({ValidationError::Code::INVALID_SIGNATURE, "Invalid signature of data `" +
                m_data.getName().toUri() + "`"});
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

const Data&
DataValidationState::getOriginalData() const
{
  return m_data;
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
    this->fail({ValidationError::Code::IMPLEMENTATION_ERROR,
                "Validator/policy did not invoke success or failure callback"});
  }
}

void
InterestValidationState::verifyOriginalPacket(const Certificate& trustedCert)
{
  if (verifySignature(m_interest, trustedCert)) {
    NDN_LOG_TRACE_DEPTH("OK signature for interest `" << m_interest.getName() << "`");
    this->afterSuccess(m_interest);
    BOOST_ASSERT(boost::logic::indeterminate(m_outcome));
    m_outcome = true;
  }
  else {
    this->fail({ValidationError::Code::INVALID_SIGNATURE, "Invalid signature of interest `" +
                m_interest.getName().toUri() + "`"});
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

const Interest&
InterestValidationState::getOriginalInterest() const
{
  return m_interest;
}

} // namespace v2
} // namespace security
} // namespace ndn
