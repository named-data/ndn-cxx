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

#ifndef NDN_CXX_TESTS_UNIT_SECURITY_PIB_PIB_DATA_FIXTURE_HPP
#define NDN_CXX_TESTS_UNIT_SECURITY_PIB_PIB_DATA_FIXTURE_HPP

#include "ndn-cxx/security/certificate.hpp"
#include "ndn-cxx/security/pib/pib-impl.hpp"

namespace ndn::tests {

using ndn::security::Certificate;

class PibDataFixture
{
public:
  PibDataFixture();

  [[nodiscard]] static shared_ptr<security::pib::PibImpl>
  makePibWithIdentity(const Name& idName);

  [[nodiscard]] static shared_ptr<security::pib::PibImpl>
  makePibWithKey(const Name& keyName, span<const uint8_t> key);

public:
  Certificate id1Key1Cert1;
  Certificate id1Key1Cert2;
  Certificate id1Key2Cert1;
  Certificate id1Key2Cert2;
  Certificate id2Key1Cert1;
  Certificate id2Key1Cert2;
  Certificate id2Key2Cert1;
  Certificate id2Key2Cert2;

  Name id1;
  Name id2;

  Name id1Key1Name;
  Name id1Key2Name;
  Name id2Key1Name;
  Name id2Key2Name;

  Buffer id1Key1;
  Buffer id1Key2;
  Buffer id2Key1;
  Buffer id2Key2;
};

} // namespace ndn::tests

#endif // NDN_CXX_TESTS_UNIT_SECURITY_PIB_PIB_DATA_FIXTURE_HPP
