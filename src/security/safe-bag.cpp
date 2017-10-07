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
 *
 * @author Zhiyi Zhang <dreamerbarrychang@gmail.com>
 */

#include "safe-bag.hpp"
#include "encoding/encoding-buffer.hpp"
#include "encoding/tlv-security.hpp"
#include "util/concepts.hpp"

namespace ndn {
namespace security {

BOOST_CONCEPT_ASSERT((WireEncodable<SafeBag>));
BOOST_CONCEPT_ASSERT((WireDecodable<SafeBag>));

SafeBag::SafeBag() = default;

SafeBag::SafeBag(const Block& wire)
{
  this->wireDecode(wire);
}

SafeBag::SafeBag(const Data& certificate,
                 const Buffer& encryptedKeyBag)
  : m_certificate(certificate)
  , m_encryptedKeyBag(encryptedKeyBag)
{
}

SafeBag::SafeBag(const Data& certificate,
                 const uint8_t* encryptedKey,
                 size_t encryptedKeyLen)
  : m_certificate(certificate)
  , m_encryptedKeyBag(encryptedKey, encryptedKeyLen)
{
}

///////////////////////////////////////////////////// encode & decode

template<encoding::Tag TAG>
size_t
SafeBag::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;

  // EncryptedKeyBag
  totalLength += encoder.prependByteArrayBlock(tlv::security::EncryptedKeyBag,
                                               m_encryptedKeyBag.data(),
                                               m_encryptedKeyBag.size());

  // Certificate
  totalLength += this->m_certificate.wireEncode(encoder);

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::security::SafeBag);

  return totalLength;
}

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(SafeBag);

const Block&
SafeBag::wireEncode() const
{
  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  this->m_wire = buffer.block();
  return m_wire;
}

void
SafeBag::wireDecode(const Block& wire)
{
  if (wire.type() != tlv::security::SafeBag)
    BOOST_THROW_EXCEPTION(tlv::Error("Unexpected TLV type when decoding safebag"));

  this->m_wire = wire;
  m_wire.parse();

  Block::element_const_iterator it = m_wire.elements_begin();

  // Certificate must be the first part
  if (it != m_wire.elements_end()) {
    this->m_certificate.wireDecode(*it);
    it++;
  }
  else
    BOOST_THROW_EXCEPTION(tlv::Error("Unexpected TLV structure when decoding certificate"));

  // EncryptedKeyBag
  if (it != m_wire.elements_end() && it->type() == tlv::security::EncryptedKeyBag) {
    this->m_encryptedKeyBag = Buffer(it->value(), it->value_size());
    it++;
  }
  else
    BOOST_THROW_EXCEPTION(tlv::Error("Unexpected TLV structure when decoding encryptedkeybag"));

  // Check if end
  if (it != m_wire.elements_end())
    BOOST_THROW_EXCEPTION(tlv::Error("Unexpected TLV structure after decoding the block"));
}

} // namespace security
} // namespace ndn
