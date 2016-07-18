/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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

#ifndef NDN_CXX_LP_DETAIL_FIELD_DECL_HPP
#define NDN_CXX_LP_DETAIL_FIELD_DECL_HPP

#include "../field.hpp"
#include "../tlv.hpp"

#include "../../encoding/block-helpers.hpp"
#include "../../util/concepts.hpp"
#include <boost/concept/requires.hpp>

namespace ndn {
namespace lp {
namespace detail {

template<typename TlvType, typename T>
struct DecodeHelper
{
  static
  BOOST_CONCEPT_REQUIRES(((WireDecodable<T>)), (T))
  decode(const Block& wire)
  {
    T type;
    type.wireDecode(wire);
    return type;
  }
};

template<typename TlvType>
struct DecodeHelper<TlvType, uint64_t>
{
  static uint64_t
  decode(const Block& wire)
  {
    return readNonNegativeInteger(wire);
  }
};

template<typename TlvType>
struct DecodeHelper<TlvType, std::pair<Buffer::const_iterator, Buffer::const_iterator>>
{
  static std::pair<Buffer::const_iterator, Buffer::const_iterator>
  decode(const Block& wire)
  {
    if (wire.value_size() == 0) {
      BOOST_THROW_EXCEPTION(ndn::tlv::Error(to_string(wire.type()) + " must not be empty"));
    }

    return std::make_pair(wire.value_begin(), wire.value_end());
  }
};

template<typename encoding::Tag TAG, typename TlvType, typename T>
struct EncodeHelper
{
  static
  BOOST_CONCEPT_REQUIRES(((WireEncodable<T>)), (size_t))
  encode(EncodingImpl<TAG>& encoder, const T& value)
  {
    return value.wireEncode(encoder);
  }
};

template<typename encoding::Tag TAG, typename TlvType>
struct EncodeHelper<TAG, TlvType, uint64_t>
{
  static size_t
  encode(EncodingImpl<TAG>& encoder, const uint64_t value)
  {
    return prependNonNegativeIntegerBlock(encoder, TlvType::value, value);
  }
};

template<typename encoding::Tag TAG, typename TlvType>
struct EncodeHelper<TAG, TlvType, std::pair<Buffer::const_iterator, Buffer::const_iterator>>
{
  static size_t
  encode(EncodingImpl<TAG>& encoder, const std::pair<Buffer::const_iterator, Buffer::const_iterator>& value)
  {
    size_t length = 0;
    length += encoder.prependRange(value.first, value.second);
    length += encoder.prependVarNumber(length);
    length += encoder.prependVarNumber(TlvType::value);
    return length;
  }
};

template<typename LOCATION, typename VALUE, uint64_t TYPE, bool REPEATABLE = false>
class FieldDecl
{
public:
  typedef LOCATION FieldLocation;
  typedef VALUE ValueType;
  typedef std::integral_constant<uint64_t, TYPE> TlvType;
  typedef std::integral_constant<bool, REPEATABLE> IsRepeatable;

  /** \brief decodes a field
   *  \param wire a Block with top-level type \p TYPE
   *  \return value of the field
   */
  static ValueType
  decode(const Block& wire)
  {
    if (wire.type() != TlvType::value) {
      BOOST_THROW_EXCEPTION(ndn::tlv::Error("Unexpected TLV type " + to_string(wire.type())));
    }

    return DecodeHelper<TlvType, ValueType>::decode(wire);
  }

  /** \brief encodes a field and prepends to \p encoder its Block with top-level type \p TYPE
   *  \param encoder Instance of the buffer encoder or buffer estimator
   *  \param value value of the field
   */
  template<typename encoding::Tag TAG, typename T>
  static size_t
  encode(EncodingImpl<TAG>& encoder, const T& value)
  {
    return EncodeHelper<TAG, TlvType, T>::encode(encoder, value);
  }
};

} // namespace detail
} // namespace lp
} // namespace ndn

#endif // NDN_CXX_LP_DETAIL_FIELD_DECL_HPP
