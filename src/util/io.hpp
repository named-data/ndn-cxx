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

#ifndef NDN_UTIL_IO_HPP
#define NDN_UTIL_IO_HPP

#include "concepts.hpp"
#include "../encoding/block.hpp"

#include <fstream>

namespace ndn {
namespace io {

class Error : public std::runtime_error
{
public:
  explicit
  Error(const std::string& what)
    : std::runtime_error(what)
  {
  }
};

/** \brief indicates how a file or stream is encoded
 */
enum IoEncoding {
  /** \brief binary without encoding
   */
  NO_ENCODING,

  /** \brief base64 encoding
   *
   *  \p save() inserts a newline after every 64 characters,
   *  \p load() can accept base64 text with or without newlines
   */
  BASE64,

  /** \brief hexadecimal encoding
   *
   *  \p save() uses uppercase letters A-F, \p load() can accept mixed-case
   */
  HEX
};

namespace detail {

template<typename T>
static void
checkInnerError(typename T::Error*)
{
  static_assert(std::is_base_of<tlv::Error, typename T::Error>::value,
                "T::Error, if declared, must inherit from ndn::tlv::Error");
}

template<typename T>
static void
checkInnerError(...)
{
  // T::Error is not declared
}

} // namespace detail

/** \brief loads a TLV block from a stream
 *  \return if success, the Block and true
 *          otherwise, a default-constructed Block and false
 */
optional<Block>
loadBlock(std::istream& is, IoEncoding encoding = BASE64);

/** \brief loads a TLV element from a stream
 *  \tparam T type of TLV element; T must be WireDecodable,
 *            and must have a Error nested type
 *  \return the TLV element, or nullptr if an error occurs
 */
template<typename T>
shared_ptr<T>
load(std::istream& is, IoEncoding encoding = BASE64)
{
  BOOST_CONCEPT_ASSERT((WireDecodable<T>));
  detail::checkInnerError<T>(nullptr);

  optional<Block> block = loadBlock(is, encoding);
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

/** \brief loads a TLV element from a file
 */
template<typename T>
shared_ptr<T>
load(const std::string& filename, IoEncoding encoding = BASE64)
{
  std::ifstream is(filename);
  return load<T>(is, encoding);
}

/** \brief saves a TLV block to a stream
 *  \throw Error error during saving
 */
void
saveBlock(const Block& block, std::ostream& os, IoEncoding encoding = BASE64);

/** \brief saves a TLV element to a stream
 *  \tparam T type of TLV element; T must be WireEncodable,
 *            and must have a Error nested type
 *  \throw Error error during encoding or saving
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
  catch (const tlv::Error& e) {
    BOOST_THROW_EXCEPTION(Error(e.what()));
  }

  saveBlock(block, os, encoding);
}

/** \brief saves a TLV element to a file
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
