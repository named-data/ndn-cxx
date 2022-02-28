/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#ifndef NDN_CXX_UTIL_IO_HPP
#define NDN_CXX_UTIL_IO_HPP

#include "ndn-cxx/encoding/block.hpp"
#include "ndn-cxx/util/concepts.hpp"
#include "ndn-cxx/util/optional.hpp"

#include <fstream>

namespace ndn {
namespace io {

class Error : public std::runtime_error
{
public:
  using std::runtime_error::runtime_error;
};

/** \brief Indicates how a file or stream of bytes is encoded.
 */
enum IoEncoding {
  /** \brief Raw binary, without encoding
   */
  NO_ENCODING,

  /** \brief Base64 encoding
   *
   *  `save()` inserts a newline after every 64 characters,
   *  `load()` can accept base64 text with or without newlines.
   */
  BASE64,

  /** \brief Hexadecimal encoding
   *
   *  `save()` uses uppercase letters A-F, `load()` can accept mixed-case.
   */
  HEX,
};

namespace detail {

template<typename T>
static void
checkNestedError(typename T::Error*)
{
  static_assert(std::is_convertible<typename T::Error*, tlv::Error*>::value,
                "T::Error, if defined, must be a subclass of ndn::tlv::Error");
}

template<typename T>
static void
checkNestedError(...)
{
  // T::Error is not defined
}

} // namespace detail

/**
 * \brief Reads bytes from a stream until EOF.
 * \return a Buffer containing the bytes read from the stream
 * \throw Error An error occurred, e.g., malformed input.
 * \throw std::invalid_argument The specified encoding is not supported.
 */
shared_ptr<Buffer>
loadBuffer(std::istream& is, IoEncoding encoding = BASE64);

/**
 * \brief Reads a TLV element of type `T` from a stream.
 * \tparam T Class type representing the TLV element; must be WireDecodable.
 * \return the parsed TLV element
 * \throw Error An error occurred, e.g., malformed input.
 * \throw std::invalid_argument The specified encoding is not supported.
 */
template<typename T>
T
loadTlv(std::istream& is, IoEncoding encoding = BASE64)
{
  BOOST_CONCEPT_ASSERT((WireDecodable<T>));

  auto buf = loadBuffer(is, encoding);
  try {
    return T(Block(buf));
  }
  catch (const std::exception& e) {
    NDN_THROW_NESTED(Error("Decode error during load: "s + e.what()));
  }
}

/**
 * \brief Reads a TLV element from a stream.
 * \tparam T Type of TLV element; `T` must be WireDecodable and the nested type
 *           `T::Error`, if defined, must be a subclass of ndn::tlv::Error.
 * \return the TLV element, or nullptr if an error occurs
 * \note This function has a peculiar error handling behavior. Consider using loadTlv() instead.
 */
template<typename T>
shared_ptr<T>
load(std::istream& is, IoEncoding encoding = BASE64)
{
  BOOST_CONCEPT_ASSERT((WireDecodable<T>));
  detail::checkNestedError<T>(nullptr);

  Block block;
  try {
    block = Block(loadBuffer(is, encoding));
  }
  catch (const std::invalid_argument&) {
    return nullptr;
  }
  catch (const std::runtime_error&) {
    return nullptr;
  }

  try {
    return make_shared<T>(block);
  }
  catch (const tlv::Error&) {
    return nullptr;
  }
}

/**
 * \brief Reads a TLV element from a file.
 * \tparam T Type of TLV element; `T` must be WireDecodable and the nested type
 *           `T::Error`, if defined, must be a subclass of ndn::tlv::Error.
 * \return the TLV element, or nullptr if an error occurs
 * \note This function has a peculiar error handling behavior. Consider using loadTlv() instead.
 */
template<typename T>
shared_ptr<T>
load(const std::string& filename, IoEncoding encoding = BASE64)
{
  std::ifstream is(filename);
  return load<T>(is, encoding);
}

/**
 * \brief Writes a sequence of bytes to a stream.
 * \throw Error error during saving
 * \throw std::invalid_argument the specified encoding is not supported
 */
void
saveBuffer(span<const uint8_t> buf, std::ostream& os, IoEncoding encoding = BASE64);

/** \brief Writes a TLV element to a stream.
 *  \tparam T type of TLV element; `T` must be WireEncodable and the nested type
 *            `T::Error`, if defined, must be a subclass of ndn::tlv::Error
 *  \throw Error error during encoding or saving
 *  \throw std::invalid_argument the specified encoding is not supported
 */
template<typename T>
void
save(const T& obj, std::ostream& os, IoEncoding encoding = BASE64)
{
  BOOST_CONCEPT_ASSERT((WireEncodable<T>));
  detail::checkNestedError<T>(nullptr);

  Block block;
  try {
    block = obj.wireEncode();
  }
  catch (const tlv::Error& e) {
    NDN_THROW_NESTED(Error("Encode error during save: "s + e.what()));
  }

  saveBuffer(block, os, encoding);
}

/** \brief Writes a TLV element to a file.
 *  \tparam T type of TLV element; `T` must be WireEncodable and the nested type
 *            `T::Error`, if defined, must be a subclass of ndn::tlv::Error
 *  \throw Error error during encoding or saving
 *  \throw std::invalid_argument the specified encoding is not supported
 */
template<typename T>
void
save(const T& obj, const std::string& filename, IoEncoding encoding = BASE64)
{
  std::ofstream os(filename);
  save(obj, os, encoding);
}

} // namespace io
} // namespace ndn

#endif // NDN_CXX_UTIL_IO_HPP
