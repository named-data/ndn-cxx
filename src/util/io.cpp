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

#include "io.hpp"
#include "../encoding/buffer-stream.hpp"
#include "../security/transform.hpp"

namespace ndn {
namespace io {

optional<Block>
loadBlock(std::istream& is, IoEncoding encoding)
{
  namespace t = ndn::security::transform;

  OBufferStream os;
  try {
    switch (encoding) {
      case NO_ENCODING:
        t::streamSource(is) >> t::streamSink(os);
        break;
      case BASE64:
        t::streamSource(is) >> t::stripSpace("\n") >> t::base64Decode(false) >> t::streamSink(os);
        break;
      case HEX:
        t::streamSource(is) >> t::hexDecode() >> t::streamSink(os);
        break;
      default:
        return nullopt;
    }
  }
  catch (const t::Error&) {
    return nullopt;
  }

  try {
    return make_optional<Block>(os.buf());
  }
  catch (const tlv::Error&) {
    return nullopt;
  }
  catch (const std::invalid_argument&) {
    return nullopt;
  }
}

void
saveBlock(const Block& block, std::ostream& os, IoEncoding encoding)
{
  namespace t = ndn::security::transform;

  try {
    switch (encoding) {
      case NO_ENCODING:
        t::bufferSource(block.wire(), block.size()) >> t::streamSink(os);
        break;
      case BASE64:
        t::bufferSource(block.wire(), block.size()) >> t::base64Encode() >> t::streamSink(os);
        break;
      case HEX:
        t::bufferSource(block.wire(), block.size()) >> t::hexEncode(true) >> t::streamSink(os);
        break;
      default:
        BOOST_THROW_EXCEPTION(Error("unrecognized IoEncoding"));
    }
  }
  catch (const t::Error& e) {
    BOOST_THROW_EXCEPTION(Error(e.what()));
  }
}

} // namespace io
} // namespace ndn
