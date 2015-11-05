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
#include "lp/nack.hpp"

namespace ndn {
namespace util {

inline shared_ptr<Interest>
makeInterest(const Name& name, uint32_t nonce = 0)
{
  auto interest = make_shared<Interest>(name);
  if (nonce != 0) {
    interest->setNonce(nonce);
  }
  return interest;
}

inline shared_ptr<Data>
signData(shared_ptr<Data> data)
{
  SignatureSha256WithRsa fakeSignature;
  fakeSignature.setValue(encoding::makeEmptyBlock(tlv::SignatureValue));
  data->setSignature(fakeSignature);
  data->wireEncode();
  return data;
}

inline shared_ptr<Data>
makeData(const Name& name)
{
  auto data = make_shared<Data>(name);
  return signData(data);
}

inline shared_ptr<Link>
makeLink(const Name& name, std::initializer_list<std::pair<uint32_t, Name>> delegations)
{
  auto link = make_shared<Link>(name, delegations);
  signData(link);
  return link;
}

inline lp::Nack
makeNack(const Interest& interest, lp::NackReason reason)
{
  lp::Nack nack(interest);
  nack.setReason(reason);
  return nack;
}

inline lp::Nack
makeNack(const Name& name, uint32_t nonce, lp::NackReason reason)
{
  Interest interest(name);
  interest.setNonce(nonce);
  return makeNack(interest, reason);
}

} // namespace util
} // namespace ndn

#endif // NDN_TESTS_UNIT_TESTS_MAKE_INTEREST_DATA_HPP
