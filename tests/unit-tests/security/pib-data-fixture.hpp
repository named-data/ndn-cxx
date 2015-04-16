/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#ifndef NDN_TESTS_PIB_DATA_FIXTURE_HPP
#define NDN_TESTS_PIB_DATA_FIXTURE_HPP

#include "security/identity-certificate.hpp"

#include "boost-test.hpp"

namespace ndn {
namespace security {

class PibDataFixture
{
public:
  PibDataFixture();

public:
  Name id1;
  Name id2;

  Name id1Key1Name;
  Name id1Key2Name;
  Name id2Key1Name;
  Name id2Key2Name;

  PublicKey id1Key1;
  PublicKey id1Key2;
  PublicKey id2Key1;
  PublicKey id2Key2;

  IdentityCertificate id1Key1Cert1;
  IdentityCertificate id1Key1Cert2;
  IdentityCertificate id1Key2Cert1;
  IdentityCertificate id1Key2Cert2;
  IdentityCertificate id2Key1Cert1;
  IdentityCertificate id2Key1Cert2;
  IdentityCertificate id2Key2Cert1;
  IdentityCertificate id2Key2Cert2;
};

} // namespace security
} // namespace ndn

#endif // NDN_TESTS_PIB_DATA_FIXTURE_HPP
