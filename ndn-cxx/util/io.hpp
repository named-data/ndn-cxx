/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#ifndef NDN_UTIL_IO_HPP
#define NDN_UTIL_IO_HPP

#include "ndn-cxx/encoding/block.hpp"
#include "ndn-cxx/util/concepts.hpp"

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
checkInnerError(typename T::Error*)
{
  static_assert(std::is_convertible<typename T::Error*, tlv::Error*>::value,
                "T::Error, if defined, must be a subclass of ndn::tlv::Error");
}

template<typename T>
static void
checkInnerError(...)
{
  // T::Error is not defined
}

} // namespace detail

/** \brief Reads bytes from a stream until EOF.
 *  \return a Buffer containing the bytes read from the stream
 *  \throw Error error during loading
 *  \throw std::invalid_argument the specified encoding is not supported
 */
shared_ptr<Buffer>
loadBuffer(std::istream& is, IoEncoding encoding = BASE64);

/** \brief Reads a TLV block from a stream.
 *  \return a Block, or nullopt if an error occurs
 */
optional<Block>
loadBlock(std::istream& is, IoEncoding encoding = BASE64);

/** \brief Reads a TLV element from a stream.
 *  \tparam T type of TLV element; `T` must be WireDecodable and the nested type
 *            `T::Error`, if defined, must be a subclass of ndn::tlv::Error
 *  \return the TLV element, or nullptr if an error occurs
 */
template<typename T>
shared_ptr<T>
load(std::istream& is, IoEncoding encoding = BASE64)
{
  BOOST_CONCEPT_ASSERT((WireDecodable<T>));
  detail::checkInnerError<T>(nullptr);

  auto block = loadBlock(is, encoding);
  if (!block) {
    return nullptr;
  }

  try {
    return make_shared<T>(*block);
  }
  catch (const tlv::Error&) {
    return nullptr;
  }
}

/** \brief Reads a TLV element from a file.
 *  \tparam T type of TLV element; `T` must be WireDecodable and the nested type
 *            `T::Error`, if defined, must be a subclass of ndn::tlv::Error
 *  \return the TLV element, or nullptr if an error occurs
 */
template<typename T>
shared_ptr<T>
load(const std::string& filename, IoEncoding encoding = BASE64)
{
  std::ifstream is(filename);
  return load<T>(is, encoding);
}

/** \brief Writes a byte buffer to a stream.
 *  \throw Error error during saving
 *  \throw std::invalid_argument the specified encoding is not supported
 */
void
saveBuffer(const uint8_t* buf, size_t size, std::ostream& os, IoEncoding encoding = BASE64);

/** \brief Writes a TLV block to a stream.
 *  \throw Error error during saving
 *  \throw std::invalid_argument the specified encoding is not supported
 */
void
saveBlock(const Block& block, std::ostream& os, IoEncoding encoding = BASE64);

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
  detail::checkInnerError<T>(nullptr);

  Block block;
  try {
    block = obj.wireEncode();
  }
  catch (const tlv::Error&) {
    NDN_THROW_NESTED(Error("Encode error during save"));
  }

  saveBlock(block, os, encoding);
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

#endif // NDN_UTIL_IO_HPP
