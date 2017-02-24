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

#ifndef NDN_UTIL_STRING_HELPER_HPP
#define NDN_UTIL_STRING_HELPER_HPP

#include "../common.hpp"

namespace ndn {

class Buffer;

class StringHelperError : public std::invalid_argument
{
public:
  explicit
  StringHelperError(const std::string& what)
    : std::invalid_argument(what)
  {
  }
};

/**
 * @brief Helper class to convert a number to hexadecimal
 *        format, for use with stream insertion operators
 *
 * Example usage:
 *
 * @code
 * std::cout << AsHex{42};                   // outputs "0x2a"
 * std::cout << std::uppercase << AsHex{42}; // outputs "0x2A"
 * @endcode
 */
class AsHex
{
public:
  constexpr explicit
  AsHex(uint64_t val) noexcept
    : m_value(val)
  {
  }

private:
  uint64_t m_value;

  friend std::ostream& operator<<(std::ostream&, const AsHex&);
};

std::ostream&
operator<<(std::ostream& os, const AsHex& hex);

/**
 * @brief Output the hex representation of @p num to the output stream @p os
 *
 * @param os Output stream
 * @param num Number to print in hexadecimal format
 * @param wantUpperCase if true, print uppercase hex chars; the default is to use lowercase
 *
 * The output string is a continuous sequence of hex characters without any whitespace separators.
 */
void
printHex(std::ostream& os, uint64_t num, bool wantUpperCase = false);

/**
 * @brief Output the hex representation of the bytes in @p buffer to the output stream @p os
 *
 * @param os Output stream
 * @param buffer Pointer to an array of bytes
 * @param length Size of the array
 * @param wantUpperCase if true (the default) print uppercase hex chars
 *
 * Examples:
 *
 * @code
 * printHex(std::cout, "Hello, World!");        // outputs "48656C6C6F2C20776F726C6421"
 * printHex(std::cout, "Hello, World!", false); // outputs "48656c6c6f2c20776f726c6421"
 * @endcode
 *
 * Each octet is always represented as two hex characters ("00" for octet==0).
 *
 * The output string is a continuous sequence of hex characters without any whitespace separators.
 */
void
printHex(std::ostream& os, const uint8_t* buffer, size_t length, bool wantUpperCase = true);

/**
 * @brief Output the hex representation of the bytes in @p buffer to the output stream @p os
 *
 * @param os Output stream
 * @param buffer Buffer of bytes to print in hexadecimal format
 * @param wantUpperCase if true (the default) print uppercase hex chars
 */
void
printHex(std::ostream& os, const Buffer& buffer, bool wantUpperCase = true);

/**
 * @brief Return a string containing the hex representation of the bytes in @p buffer
 *
 * @param buffer Pointer to an array of bytes
 * @param length Size of the array
 * @param wantUpperCase if true (the default) use uppercase hex chars
 *
 * Examples:
 *
 * @code
 * toHex("Hello, World!") == "48656C6C6F2C20776F726C6421"
 * toHex("Hello, World!", false) == "48656c6c6f2c20776f726c6421"
 * @endcode
 *
 * Each octet is always represented as two hex characters ("00" for octet==0).
 *
 * The output string is a continuous sequence of hex characters without any whitespace separators.
 */
std::string
toHex(const uint8_t* buffer, size_t length, bool wantUpperCase = true);

/**
 * @brief Return a string containing the hex representation of the bytes in @p buffer
 *
 * @param buffer Buffer of bytes to convert to hexadecimal format
 * @param wantUpperCase if true (the default) use uppercase hex chars
 */
std::string
toHex(const Buffer& buffer, bool wantUpperCase = true);

/**
 * @brief Convert the hex character to an integer from 0 to 15, or -1 if not a hex character
 */
int
fromHexChar(char c);

/**
 * @brief Convert the hex string to buffer
 * @param hexString sequence of pairs of hex numbers (lower and upper case can be mixed)
 *        without any whitespace separators (e.g., "48656C6C6F2C20776F726C6421")
 * @throw StringHelperError if input is invalid
 */
shared_ptr<Buffer>
fromHex(const std::string& hexString);

/**
 * @brief Decode a percent-encoded string
 * @see RFC 3986 section 2
 *
 * When % is not followed by two hex characters, the output is not transformed.
 *
 * Examples:
 *
 * @code
 * unescape("hello%20world") == "hello world"
 * unescape("hello%20world%FooBar") == "hello world%FooBar"
 * @endcode
 */
std::string
unescape(const std::string& str);

} // namespace ndn

#endif // NDN_UTIL_STRING_HELPER_HPP
