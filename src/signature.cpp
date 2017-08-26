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

#include "signature.hpp"

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Signature>));
static_assert(std::is_base_of<tlv::Error, Signature::Error>::value,
              "Signature::Error must inherit from tlv::Error");

Signature::Signature(const Block& info, const Block& value)
  : m_info(info)
  , m_value(value)
{
}


Signature::Signature(const SignatureInfo& info, const Block& value)
  : m_info(info)
  , m_value(value)
{
}

tlv::SignatureTypeValue
Signature::getType() const
{
  if (!*this) {
    BOOST_THROW_EXCEPTION(Error("Signature is invalid"));
  }
  return static_cast<tlv::SignatureTypeValue>(m_info.getSignatureType());
}

void
Signature::setInfo(const Block& info)
{
  m_info = SignatureInfo(info);
}

void
Signature::setValue(const Block& value)
{
  if (value.type() != tlv::SignatureValue) {
    BOOST_THROW_EXCEPTION(Error("Expecting SignatureValue, but TLV-TYPE is " + to_string(value.type())));
  }
  m_value = value;
}

bool
operator==(const Signature& lhs, const Signature& rhs)
{
  return lhs.getSignatureInfo() == rhs.getSignatureInfo() && lhs.getValue() == rhs.getValue();
}

} // namespace ndn
