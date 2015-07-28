/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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
#include "../encoding/buffer-stream.hpp"
#include "../security/cryptopp.hpp"

#include <sstream>
#include <iomanip>

#include <boost/algorithm/string/trim.hpp>

namespace ndn {

void
printHex(std::ostream& os, const uint8_t* buffer, size_t length, bool isUpperCase/* = true*/)
{
  if (buffer == nullptr || length == 0)
    return;

  auto newFlags = std::ios::hex;
  if (isUpperCase) {
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
printHex(std::ostream& os, const Buffer& buffer, bool isUpperCase/* = true*/)
{
  return printHex(os, buffer.buf(), buffer.size(), isUpperCase);
}

std::string
toHex(const uint8_t* buffer, size_t length, bool isUpperCase/* = true*/)
{
  if (buffer == nullptr || length == 0)
    return "";

  std::ostringstream result;
  printHex(result, buffer, length, isUpperCase);
  return result.str();
}

std::string
toHex(const Buffer& buffer, bool isUpperCase/* = true*/)
{
  return toHex(buffer.buf(), buffer.size(), isUpperCase);
}

int
fromHexChar(uint8_t c)
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

shared_ptr<const Buffer>
fromHex(const std::string& hexString)
{
  if (hexString.size() % 2 != 0) {
    BOOST_THROW_EXCEPTION(StringHelperError("Invalid number of characters in the supplied hex "
                                            "string"));
  }

  using namespace CryptoPP;

  OBufferStream os;
  StringSource(hexString, true, new HexDecoder(new FileSink(os)));
  shared_ptr<const Buffer> buffer = os.buf();

  if (buffer->size() * 2 != hexString.size()) {
    BOOST_THROW_EXCEPTION(StringHelperError("The supplied hex string contains non-hex characters"));
  }

  return buffer;
}

void
trimLeft(std::string& str)
{
  boost::algorithm::trim_left(str);
}

void
trimRight(std::string& str)
{
  boost::algorithm::trim_right(str);
}

void
trim(std::string& str)
{
  boost::algorithm::trim(str);
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
