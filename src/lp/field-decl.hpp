/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#ifndef NDN_CXX_LP_FIELD_DECL_HPP
#define NDN_CXX_LP_FIELD_DECL_HPP

#include "empty-value.hpp"
#include "field.hpp"
#include "sequence.hpp"
#include "tlv.hpp"

#include "../encoding/block-helpers.hpp"
#include "../util/concepts.hpp"
#include <boost/concept/requires.hpp>

namespace ndn {
namespace lp {

/** \brief Indicate a uint64_t field shall be decoded and encoded as a non-negative integer.
 */
struct NonNegativeIntegerTag;

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
struct DecodeHelper<TlvType, EmptyValue>
{
  static EmptyValue
  decode(const Block& wire)
  {
    if (wire.value_size() != 0) {
      BOOST_THROW_EXCEPTION(ndn::tlv::Error("NDNLP field of TLV-TYPE " + to_string(wire.type()) +
                                            " must be empty"));
    }

    return EmptyValue{};
  }
};

template<typename TlvType>
struct DecodeHelper<TlvType, NonNegativeIntegerTag>
{
  static uint64_t
  decode(const Block& wire)
  {
    return readNonNegativeInteger(wire);
  }
};

template<typename TlvType>
struct DecodeHelper<TlvType, uint64_t>
{
  static uint64_t
  decode(const Block& wire)
  {
    // NDNLPv2 spec defines sequence number fields to be encoded as a fixed-width unsigned integer,
    // but previous versions of ndn-cxx encode it as a NonNegativeInteger, so we decode it as such
    // for backwards compatibility. In a future version, the decoder will be changed to accept
    // 8-byte big endian only, to allow faster decoding.
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
      BOOST_THROW_EXCEPTION(ndn::tlv::Error("NDNLP field of TLV-TYPE " + to_string(wire.type()) +
                                            " cannot be empty"));
    }

    return std::make_pair(wire.value_begin(), wire.value_end());
  }
};

template<typename encoding::Tag TAG, typename TlvType, typename T>
struct EncodeHelper
{
  static
  BOOST_CONCEPT_REQUIRES(((WireEncodableWithEncodingBuffer<T>)), (size_t))
  encode(EncodingImpl<TAG>& encoder, const T& value)
  {
    return value.wireEncode(encoder);
  }
};

template<typename encoding::Tag TAG, typename TlvType>
struct EncodeHelper<TAG, TlvType, EmptyValue>
{
  static size_t
  encode(EncodingImpl<TAG>& encoder, const EmptyValue value)
  {
    size_t length = 0;
    length += encoder.prependVarNumber(0);
    length += encoder.prependVarNumber(TlvType::value);
    return length;
  }
};

template<typename encoding::Tag TAG, typename TlvType>
struct EncodeHelper<TAG, TlvType, NonNegativeIntegerTag>
{
  static size_t
  encode(EncodingImpl<TAG>& encoder, uint64_t value)
  {
    return prependNonNegativeIntegerBlock(encoder, TlvType::value, value);
  }
};

template<typename encoding::Tag TAG, typename TlvType>
struct EncodeHelper<TAG, TlvType, uint64_t>
{
  static size_t
  encode(EncodingImpl<TAG>& encoder, uint64_t value)
  {
    uint64_t be = htobe64(value);
    const uint8_t* buf = reinterpret_cast<const uint8_t*>(&be);
    return encoder.prependByteArrayBlock(TlvType::value, buf, sizeof(be));
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

/** \brief Declare a field.
 *  \tparam LOCATION a tag that indicates where the field is in an LpPacket.
 *  \tparam VALUE type of field value.
 *  \tparam TYPE TLV-TYPE number of the field.
 *  \tparam REPEATABLE whether the field is repeatable.
 *  \tparam DECODER_TAG selects a specialization of DecodeHelper.
 *  \tparam ENCODER_TAG selects a specialization of EncodeHelper.
 */
template<typename LOCATION, typename VALUE, uint64_t TYPE, bool REPEATABLE = false,
         typename DECODER_TAG = VALUE, typename ENCODER_TAG = VALUE>
class FieldDecl
{
public:
  typedef LOCATION FieldLocation;
  typedef VALUE ValueType;
  typedef std::integral_constant<uint64_t, TYPE> TlvType;
  typedef std::integral_constant<bool, REPEATABLE> IsRepeatable;

  /** \brief Decode a field.
   *  \param wire an element with top-level TLV-TYPE \c TlvType::value.
   *  \return value of the field.
   *  \throw ndn::tlv::Error decode failure.
   */
  static ValueType
  decode(const Block& wire)
  {
    if (wire.type() != TlvType::value) {
      BOOST_THROW_EXCEPTION(ndn::tlv::Error("Unexpected TLV-TYPE " + to_string(wire.type())));
    }

    return DecodeHelper<TlvType, DECODER_TAG>::decode(wire);
  }

  /** \brief Encode a field and prepend to \p encoder.
   *  \param encoder a buffer encoder or estimator.
   *  \param value value of the field.
   */
  template<typename encoding::Tag TAG>
  static size_t
  encode(EncodingImpl<TAG>& encoder, const ValueType& value)
  {
    return EncodeHelper<TAG, TlvType, ENCODER_TAG>::encode(encoder, value);
  }
};

} // namespace lp
} // namespace ndn

#endif // NDN_CXX_LP_FIELD_DECL_HPP
