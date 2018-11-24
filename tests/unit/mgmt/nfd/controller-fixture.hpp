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

#ifndef NDN_TESTS_UNIT_MGMT_NFD_CONTROLLER_FIXTURE_HPP
#define NDN_TESTS_UNIT_MGMT_NFD_CONTROLLER_FIXTURE_HPP

#include "ndn-cxx/mgmt/nfd/controller.hpp"
#include "ndn-cxx/security/v2/certificate-fetcher-offline.hpp"
#include "ndn-cxx/util/dummy-client-face.hpp"

#include "tests/unit/dummy-validator.hpp"
#include "tests/unit/identity-management-time-fixture.hpp"

namespace ndn {
namespace nfd {
namespace tests {

using namespace ndn::tests;

class ControllerFixture : public IdentityManagementTimeFixture
{
protected:
  ControllerFixture()
    : face(io, m_keyChain)
    , m_validator(true)
    , controller(face, m_keyChain, m_validator)
    , commandFailCallback(bind(&ControllerFixture::recordCommandFail, this, _1))
    , datasetFailCallback(bind(&ControllerFixture::recordDatasetFail, this, _1, _2))
  {
    Name identityName("/localhost/ControllerFixture");
    m_keyChain.setDefaultIdentity(this->addIdentity(identityName));
  }

  /** \brief controls whether Controller's validator should accept or reject validation requests
   *
   *  Initially, the validator accepts all requests.
   *  Setting \p false causes validator to reject all requests.
   */
  void
  setValidationResult(bool shouldAccept)
  {
    m_validator.getPolicy().setResult(shouldAccept);
  }

private:
  void
  recordCommandFail(const ControlResponse& response)
  {
    failCodes.push_back(response.getCode());
  }

  void
  recordDatasetFail(uint32_t code, const std::string& reason)
  {
    failCodes.push_back(code);
  }

protected:
  ndn::util::DummyClientFace face;
  DummyValidator m_validator;
  Controller controller;
  Controller::CommandFailCallback commandFailCallback;
  Controller::DatasetFailCallback datasetFailCallback;
  std::vector<uint32_t> failCodes;
};

} // namespace tests
} // namespace nfd
} // namespace ndn

#endif // NDN_TESTS_UNIT_MGMT_NFD_CONTROLLER_FIXTURE_HPP
