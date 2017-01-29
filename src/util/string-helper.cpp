/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
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

#include "string-helper.hpp"
#include "../encoding/buffer.hpp"
#include "../encoding/buffer-stream.hpp"
#include "../security/transform/buffer-source.hpp"
#include "../security/transform/hex-decode.hpp"
#include "../security/transform/hex-encode.hpp"
#include "../security/transform/stream-sink.hpp"

#include <sstream>
#include <iomanip>

namespace ndn {

void
printHex(std::ostream& os, const uint8_t* buffer, size_t length, bool wantUpperCase)
{
  if (buffer == nullptr || length == 0)
    return;

  auto newFlags = std::ios::hex;
  if (wantUpperCase) {
    newFlags |= std::ios::uppercase;
  }
  auto oldFlags = os.flags(newFlags);
  auto oldFill = os.fill('0');
  for (size_t i = 0; i < length; ++i) {
    os << std::setw(2) << static_cast<unsigned int>(buffer[i]);
  }
  os.fill(oldFill);
  os.flags(oldFlags);
}

void
printHex(std::ostream& os, const Buffer& buffer, bool wantUpperCase)
{
  return printHex(os, buffer.buf(), buffer.size(), wantUpperCase);
}

std::string
toHex(const uint8_t* buffer, size_t length, bool wantUpperCase)
{
  BOOST_ASSERT(buffer != nullptr || length == 0);

  namespace tr = security::transform;

  std::ostringstream result;
  tr::bufferSource(buffer, length) >> tr::hexEncode(wantUpperCase) >> tr::streamSink(result);
  return result.str();
}

std::string
toHex(const Buffer& buffer, bool wantUpperCase)
{
  namespace tr = security::transform;

  std::ostringstream result;
  tr::bufferSource(buffer) >> tr::hexEncode(wantUpperCase) >> tr::streamSink(result);
  return result.str();
}

int
fromHexChar(char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';
  else if (c >= 'A' && c <= 'F')
    return c - 'A' + 0xA;
  else if (c >= 'a' && c <= 'f')
    return c - 'a' + 0xA;
  else
    return -1;
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
unescape(const std::string& str)
{
  std::ostringstream result;

  for (size_t i = 0; i < str.size(); ++i) {
    if (str[i] == '%' && i + 2 < str.size()) {
      int hi = fromHexChar(str[i + 1]);
      int lo = fromHexChar(str[i + 2]);

      if (hi < 0 || lo < 0)
        // Invalid hex characters, so just keep the escaped string.
        result << str[i] << str[i + 1] << str[i + 2];
      else
        result << static_cast<char>((hi << 4) | lo);

      // Skip ahead past the escaped value.
      i += 2;
    }
    else
      // Just copy through.
      result << str[i];
  }

  return result.str();
}

} // namespace ndn
