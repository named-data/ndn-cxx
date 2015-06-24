/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California,
 *                         Arizona Board of Regents,
 *                         Colorado State University,
 *                         University Pierre & Marie Curie, Sorbonne University,
 *                         Washington University in St. Louis,
 *                         Beijing Institute of Technology
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

#ifndef NDN_TESTS_UNIT_TESTS_MAKE_INTEREST_DATA_HPP
#define NDN_TESTS_UNIT_TESTS_MAKE_INTEREST_DATA_HPP

#include "boost-test.hpp"

#include "security/key-chain.hpp"

namespace ndn {
namespace util {

inline shared_ptr<Interest>
makeInterest(const Name& name)
{
  return make_shared<Interest>(name);
}

inline shared_ptr<Data>
signData(const shared_ptr<Data>& data)
{
  ndn::SignatureSha256WithRsa fakeSignature;
  fakeSignature.setValue(makeEmptyBlock(tlv::SignatureValue));
  data->setSignature(fakeSignature);
  data->wireEncode();

  return data;
}

inline shared_ptr<Data>
makeData(const Name& name)
{
  shared_ptr<Data> data = make_shared<Data>(name);

  return signData(data);
}

} // namespace util
} // namespace ndn

#endif // NDN_TESTS_UNIT_TESTS_MAKE_INTEREST_DATA_HPP
