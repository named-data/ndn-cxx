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
  if (!data.getSignature().hasKeyLocator()) {
    return state->fail({ValidationError::Code::INVALID_KEY_LOCATOR, "Required key locator is missing"});
  }
  const KeyLocator& locator = data.getSignature().getKeyLocator();
  if (locator.getType() != KeyLocator::KeyLocator_Name) {
    return state->fail({ValidationError::Code::INVALID_KEY_LOCATOR, "Key locator not Name"});
  }
  if (locator.getName().getPrefix(-2).isPrefixOf(data.getName())) {
    continueValidation(make_shared<CertificateRequest>(Interest(locator.getName())), state);
  }
  else {
    state->fail({ValidationError::Code::INVALID_KEY_LOCATOR, "Data signing policy violation for " +
                 data.getName().toUri() + " by " + locator.getName().toUri()});
  }
}

void
ValidationPolicySimpleHierarchy::checkPolicy(const Interest& interest, const shared_ptr<ValidationState>& state,
                                             const ValidationContinuation& continueValidation)
{
  SignatureInfo info;
  try {
    info.wireDecode(interest.getName().at(signed_interest::POS_SIG_INFO).blockFromValue());
  }
  catch (const tlv::Error& e) {
    return state->fail({ValidationError::Code::INVALID_KEY_LOCATOR, "Invalid signed interest (" +
                        std::string(e.what()) + ")"});
  }
  if (!info.hasKeyLocator()) {
    return state->fail({ValidationError::Code::INVALID_KEY_LOCATOR, "Required key locator is missing"});
  }
  const KeyLocator& locator = info.getKeyLocator();
  if (locator.getType() != KeyLocator::KeyLocator_Name) {
    return state->fail({ValidationError::Code::INVALID_KEY_LOCATOR, "Key locator not Name"});
  }
  if (locator.getName().getPrefix(-2).isPrefixOf(interest.getName())) {
    continueValidation(make_shared<CertificateRequest>(Interest(locator.getName())), state);
  }
  else {
    state->fail({ValidationError::Code::INVALID_KEY_LOCATOR, "Interest signing policy violation for " +
                 interest.getName().toUri() + " by " + locator.getName().toUri()});
  }
}

} // namespace v2
} // namespace security
} // namespace ndn
