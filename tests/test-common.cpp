/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2020 Regents of the University of California.
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

#include "tests/test-common.hpp"

namespace ndn {
namespace tests {

shared_ptr<Interest>
makeInterest(const Name& name, bool canBePrefix, optional<time::milliseconds> lifetime,
             optional<Interest::Nonce> nonce)
{
  auto interest = std::make_shared<Interest>(name);
  interest->setCanBePrefix(canBePrefix);
  if (lifetime) {
    interest->setInterestLifetime(*lifetime);
  }
  interest->setNonce(nonce);
  return interest;
}

shared_ptr<Data>
makeData(const Name& name)
{
  auto data = std::make_shared<Data>(name);
  return signData(data);
}

Data&
signData(Data& data)
{
  data.setSignatureInfo(SignatureInfo(tlv::NullSignature));
  data.setSignatureValue(std::make_shared<Buffer>());
  data.wireEncode();
  return data;
}

lp::Nack
makeNack(Interest interest, lp::NackReason reason)
{
  lp::Nack nack(std::move(interest));
  nack.setReason(reason);
  return nack;
}

} // namespace tests
} // namespace ndn
