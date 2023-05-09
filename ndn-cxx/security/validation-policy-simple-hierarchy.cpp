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

#include "ndn-cxx/security/validation-policy-simple-hierarchy.hpp"

namespace ndn::security {

void
ValidationPolicySimpleHierarchy::checkPolicy(const Data& data, const shared_ptr<ValidationState>& state,
                                             const ValidationContinuation& continueValidation)
{
  Name klName = getKeyLocatorName(data.getSignatureInfo(), *state);
  if (!state->getOutcome()) { // already failed
    return;
  }

  Name identity;
  try {
    identity = extractIdentityNameFromKeyLocator(klName);
  }
  catch (const KeyLocator::Error& e) {
    state->fail({ValidationError::INVALID_KEY_LOCATOR, e.what()});
    return;
  }

  if (!identity.isPrefixOf(data.getName())) {
    state->fail({ValidationError::POLICY_ERROR,
                 "Data " + data.getName().toUri() + " signed by " + klName.toUri()});
    return;
  }

  continueValidation(make_shared<CertificateRequest>(klName), state);
}

void
ValidationPolicySimpleHierarchy::checkPolicy(const Interest& interest, const shared_ptr<ValidationState>& state,
                                             const ValidationContinuation& continueValidation)
{
  auto sigInfo = getSignatureInfo(interest, *state);
  if (!state->getOutcome()) { // already failed
    return;
  }
  Name klName = getKeyLocatorName(sigInfo, *state);
  if (!state->getOutcome()) { // already failed
    return;
  }

  Name identity;
  try {
    identity = extractIdentityNameFromKeyLocator(klName);
  }
  catch (const KeyLocator::Error& e) {
    state->fail({ValidationError::INVALID_KEY_LOCATOR, e.what()});
    return;
  }

  if (!identity.isPrefixOf(interest.getName())) {
    state->fail({ValidationError::POLICY_ERROR,
                 "Interest " + interest.getName().toUri() + " signed by " + klName.toUri()});
    return;
  }

  continueValidation(make_shared<CertificateRequest>(klName), state);
}

} // namespace ndn::security
