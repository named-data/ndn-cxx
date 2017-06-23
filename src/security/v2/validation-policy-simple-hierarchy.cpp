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

#include "validation-policy-simple-hierarchy.hpp"

namespace ndn {
namespace security {
namespace v2 {

void
ValidationPolicySimpleHierarchy::checkPolicy(const Data& data, const shared_ptr<ValidationState>& state,
                                             const ValidationContinuation& continueValidation)
{
  Name klName = getKeyLocatorName(data, *state);
  if (!state->getOutcome()) { // already failed
    return;
  }

  if (klName.getPrefix(-2).isPrefixOf(data.getName())) {
    continueValidation(make_shared<CertificateRequest>(Interest(klName)), state);
  }
  else {
    state->fail({ValidationError::Code::INVALID_KEY_LOCATOR, "Data signing policy violation for " +
                 data.getName().toUri() + " by " + klName.toUri()});
  }
}

void
ValidationPolicySimpleHierarchy::checkPolicy(const Interest& interest, const shared_ptr<ValidationState>& state,
                                             const ValidationContinuation& continueValidation)
{
  Name klName = getKeyLocatorName(interest, *state);
  if (!state->getOutcome()) { // already failed
    return;
  }

  if (klName.getPrefix(-2).isPrefixOf(interest.getName())) {
    continueValidation(make_shared<CertificateRequest>(Interest(klName)), state);
  }
  else {
    state->fail({ValidationError::Code::INVALID_KEY_LOCATOR, "Interest signing policy violation for " +
                 interest.getName().toUri() + " by " + klName.toUri()});
  }
}

} // namespace v2
} // namespace security
} // namespace ndn
