/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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

#include "make-interest-data.hpp"
#include "security/signature-sha256-with-rsa.hpp"

namespace ndn {
namespace tests {

shared_ptr<Interest>
makeInterest(const Name& name, uint32_t nonce)
{
  auto interest = make_shared<Interest>(name);
  if (nonce != 0) {
    interest->setNonce(nonce);
  }
  return interest;
}

shared_ptr<Data>
makeData(const Name& name)
{
  auto data = make_shared<Data>(name);
  return signData(data);
}

Data&
signData(Data& data)
{
  ndn::SignatureSha256WithRsa fakeSignature;
  fakeSignature.setValue(ndn::encoding::makeEmptyBlock(tlv::SignatureValue));
  data.setSignature(fakeSignature);
  data.wireEncode();
  return data;
}

lp::Nack
makeNack(const Interest& interest, lp::NackReason reason)
{
  lp::Nack nack(interest);
  nack.setReason(reason);
  return nack;
}

lp::Nack
makeNack(const Name& name, uint32_t nonce, lp::NackReason reason)
{
  Interest interest(name);
  interest.setNonce(nonce);
  return makeNack(interest, reason);
}

} // namespace tests
} // namespace ndn
