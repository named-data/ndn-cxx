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

#ifndef NDN_TESTS_SECURITY_PIB_DATA_FIXTURE_HPP
#define NDN_TESTS_SECURITY_PIB_DATA_FIXTURE_HPP

#include "security/v2/certificate.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace security {
namespace tests {

class PibDataFixture
{
public:
  PibDataFixture();

public:
  v2::Certificate id1Key1Cert1;
  v2::Certificate id1Key1Cert2;
  v2::Certificate id1Key2Cert1;
  v2::Certificate id1Key2Cert2;
  v2::Certificate id2Key1Cert1;
  v2::Certificate id2Key1Cert2;
  v2::Certificate id2Key2Cert1;
  v2::Certificate id2Key2Cert2;

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

} // namespace tests
} // namespace security
} // namespace ndn

#endif // NDN_TESTS_SECURITY_PIB_DATA_FIXTURE_HPP
