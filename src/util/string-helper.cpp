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

#include "string-helper.hpp"
#include "../encoding/buffer.hpp"
#include "../encoding/buffer-stream.hpp"
#include "../security/transform/buffer-source.hpp"
#include "../security/transform/hex-decode.hpp"
#include "../security/transform/hex-encode.hpp"
#include "../security/transform/stream-sink.hpp"

#include <sstream>

namespace ndn {

std::ostream&
operator<<(std::ostream& os, const AsHex& hex)
{
  printHex(os, hex.m_value, os.flags() & std::ostream::uppercase);
  return os;
}

void
printHex(std::ostream& os, uint64_t num, bool wantUpperCase)
{
  auto osFlags = os.flags();
  // std::showbase doesn't work with number 0
  os << "0x" << std::noshowbase << std::noshowpos
     << (wantUpperCase ? std::uppercase : std::nouppercase)
     << std::hex << num;
  os.flags(osFlags);
}

void
printHex(std::ostream& os, const uint8_t* buffer, size_t length, bool wantUpperCase)
{
  namespace tr = security::transform;
  BOOST_ASSERT(buffer != nullptr || length == 0);
  tr::bufferSource(buffer, length) >> tr::hexEncode(wantUpperCase) >> tr::streamSink(os);
}

void
printHex(std::ostream& os, const Buffer& buffer, bool wantUpperCase)
{
  return printHex(os, buffer.data(), buffer.size(), wantUpperCase);
}

std::string
toHex(const uint8_t* buffer, size_t length, bool wantUpperCase)
{
  std::ostringstream result;
  printHex(result, buffer, length, wantUpperCase);
  return result.str();
}

std::string
toHex(const Buffer& buffer, bool wantUpperCase)
{
  return toHex(buffer.data(), buffer.size(), wantUpperCase);
}

shared_ptr<Buffer>
fromHex(const std::string& hexString)
{
  namespace tr = security::transform;

  OBufferStream os;
  try {
    tr::bufferSource(hexString) >> tr::hexDecode() >> tr::streamSink(os);
  }
  catch (const tr::Error& e) {
    BOOST_THROW_EXCEPTION(StringHelperError(std::string("Conversion from hex failed: ") + e.what()));
  }

  return os.buf();
}

std::string
escape(const std::string& str)
{
  std::ostringstream os;
  escape(os, str.data(), str.size());
  return os.str();
}

void
escape(std::ostream& os, const char* str, size_t len)
{
  for (size_t i = 0; i < len; ++i) {
    auto c = str[i];
    // Unreserved characters don't need to be escaped.
    if ((c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9') ||
        c == '-' || c == '.' ||
        c == '_' || c == '~') {
      os << c;
    }
    else {
      os << '%';
      os << toHexChar((c & 0xf0) >> 4);
      os << toHexChar(c & 0xf);
    }
  }
}

std::string
unescape(const std::string& str)
{
  std::ostringstream os;
  unescape(os, str.data(), str.size());
  return os.str();
}

void
unescape(std::ostream& os, const char* str, size_t len)
{
  for (size_t i = 0; i < len; ++i) {
    if (str[i] == '%' && i + 2 < len) {
      int hi = fromHexChar(str[i + 1]);
      int lo = fromHexChar(str[i + 2]);

      if (hi < 0 || lo < 0)
        // Invalid hex characters, so just keep the escaped string.
        os << str[i] << str[i + 1] << str[i + 2];
      else
        os << static_cast<char>((hi << 4) | lo);

      // Skip ahead past the escaped value.
      i += 2;
    }
    else {
      // Just copy through.
      os << str[i];
    }
  }
}

} // namespace ndn
