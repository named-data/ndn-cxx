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

#ifndef NDN_UTIL_IO_HPP
#define NDN_UTIL_IO_HPP

#include "../encoding/block.hpp"
#include "../encoding/buffer-stream.hpp"
#include "../security/transform.hpp"

#include <iostream>
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
  NO_ENCODING, ///< binary without encoding
  BASE_64, ///< base64 encoding
  HEX ///< uppercase hexadecimal encoding
};

/** \brief loads a TLV element from a stream
 *  \tparam T type of TLV element; T must be WireDecodable,
 *            and must have a Error nested type
 *  \return the TLV element, or nullptr if an error occurs
 */
template<typename T>
shared_ptr<T>
load(std::istream& is, IoEncoding encoding = BASE_64)
{
  OBufferStream os;
  try {
    namespace t = ndn::security::transform;
    switch (encoding) {
      case NO_ENCODING:
        t::streamSource(is) >> t::streamSink(os);
        break;
      case BASE_64:
        t::streamSource(is) >> t::base64Decode() >> t::streamSink(os);
        break;
      case HEX:
        t::streamSource(is) >> t::hexDecode() >> t::streamSink(os);
        break;
      default:
        return nullptr;
    }
  }
  catch (const ndn::security::transform::Error&) {
    return nullptr;
  }

  try {
    auto obj = make_shared<T>();
    obj->wireDecode(Block(os.buf()));
    return obj;
  }
  catch (const typename T::Error& e) {
    return nullptr;
  }
  catch (const tlv::Error& e) {
    return nullptr;
  }
}

/** \brief loads a TLV element from a file
 */
template<typename T>
shared_ptr<T>
load(const std::string& filename, IoEncoding encoding = BASE_64)
{
  std::ifstream is(filename);
  return load<T>(is, encoding);
}

/** \brief saves a TLV element to a stream
 *  \tparam T type of TLV element; T must be WireEncodable,
 *            and must have a Error nested type
 *  \throw Error error during encoding or saving
 */
template<typename T>
void
save(const T& obj, std::ostream& os, IoEncoding encoding = BASE_64)
{
  Block block;
  try {
    block = obj.wireEncode();
  }
  catch (const typename T::Error& e) {
    BOOST_THROW_EXCEPTION(Error(e.what()));
  }
  catch (const tlv::Error& e) {
    BOOST_THROW_EXCEPTION(Error(e.what()));
  }

  try {
    namespace t = ndn::security::transform;
    switch (encoding) {
      case NO_ENCODING:
        t::bufferSource(block.wire(), block.size()) >> t::streamSink(os);
        break;
      case BASE_64:
        t::bufferSource(block.wire(), block.size()) >> t::base64Encode() >> t::streamSink(os);
        break;
      case HEX:
        t::bufferSource(block.wire(), block.size()) >> t::hexEncode(true) >> t::streamSink(os);
        break;
      default:
        BOOST_THROW_EXCEPTION(Error("unrecognized IoEncoding"));
    }
  }
  catch (const ndn::security::transform::Error& e) {
    BOOST_THROW_EXCEPTION(Error(e.what()));
  }
}

/** \brief saves a TLV element to a file
 */
template<typename T>
void
save(const T& obj, const std::string& filename, IoEncoding encoding = BASE_64)
{
  std::ofstream os(filename);
  save(obj, os, encoding);
}

} // namespace io
} // namespace ndn

#endif // NDN_UTIL_IO_HPP
