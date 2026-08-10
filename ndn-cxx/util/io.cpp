/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2026 Regents of the University of California.
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

#include "ndn-cxx/util/io.hpp"
#include "ndn-cxx/encoding/buffer-stream.hpp"
#include "ndn-cxx/security/transform/base64-decode.hpp"
#include "ndn-cxx/security/transform/base64-encode.hpp"
#include "ndn-cxx/security/transform/buffer-source.hpp"
#include "ndn-cxx/security/transform/hex-decode.hpp"
#include "ndn-cxx/security/transform/hex-encode.hpp"
#include "ndn-cxx/security/transform/stream-sink.hpp"
#include "ndn-cxx/security/transform/stream-source.hpp"
#include "ndn-cxx/security/transform/strip-space.hpp"

namespace ndn::io {

shared_ptr<Buffer>
loadBuffer(std::istream& is, IoEncoding encoding)
{
  namespace t = ndn::security::transform;

  if (!is)
    NDN_THROW(Error("Cannot read from input stream"));

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
        NDN_THROW(std::invalid_argument("Unknown IoEncoding " + std::to_string(encoding)));
    }
  }
  catch (const std::runtime_error& e) {
    NDN_THROW_NESTED(Error(e.what()));
  }

  return os.buf();
}

void
saveBuffer(span<const uint8_t> buf, std::ostream& os, IoEncoding encoding)
{
  namespace t = ndn::security::transform;

  try {
    switch (encoding) {
      case NO_ENCODING:
        t::bufferSource(buf) >> t::streamSink(os);
        break;
      case BASE64:
        t::bufferSource(buf) >> t::base64Encode() >> t::streamSink(os);
        break;
      case HEX:
        t::bufferSource(buf) >> t::hexEncode(true) >> t::streamSink(os);
        break;
      default:
        NDN_THROW(std::invalid_argument("Unknown IoEncoding " + std::to_string(encoding)));
    }
  }
  catch (const std::runtime_error& e) {
    NDN_THROW_NESTED(Error(e.what()));
  }

  if (!os)
    NDN_THROW(Error("Cannot write to output stream"));
}

} // namespace ndn::io
