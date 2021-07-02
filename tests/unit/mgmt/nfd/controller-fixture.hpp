/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2021 Regents of the University of California.
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

#ifndef NDN_CXX_TESTS_UNIT_MGMT_NFD_CONTROLLER_FIXTURE_HPP
#define NDN_CXX_TESTS_UNIT_MGMT_NFD_CONTROLLER_FIXTURE_HPP

#include "ndn-cxx/mgmt/nfd/controller.hpp"
#include "ndn-cxx/security/certificate-fetcher-offline.hpp"
#include "ndn-cxx/util/dummy-client-face.hpp"

#include "tests/unit/dummy-validator.hpp"
#include "tests/unit/io-key-chain-fixture.hpp"

namespace ndn {
namespace nfd {
namespace tests {

using namespace ndn::tests;

class ControllerFixture : public IoKeyChainFixture
{
protected:
  ControllerFixture()
    : face(m_io, m_keyChain)
    , m_validator(true)
    , controller(face, m_keyChain, m_validator)
    , commandFailCallback([this] (const auto& resp) { failCodes.push_back(resp.getCode()); })
    , datasetFailCallback([this] (auto code, const auto&) { failCodes.push_back(code); })
  {
    m_keyChain.setDefaultIdentity(m_keyChain.createIdentity("/localhost/ControllerFixture"));
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

#endif // NDN_CXX_TESTS_UNIT_MGMT_NFD_CONTROLLER_FIXTURE_HPP
