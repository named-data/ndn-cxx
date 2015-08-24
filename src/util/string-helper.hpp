/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#ifndef NDN_STRING_HELPER_HPP
#define NDN_STRING_HELPER_HPP

#include "../common.hpp"
#include "../encoding/buffer.hpp"

namespace ndn {

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
 * @brief Output the hex representation of the bytes in array to the output stream @p os
 *
 * @param os Output stream
 * @param buffer The array of bytes
 * @param length Size of the array
 * @param isUpperCase if true (default) output use uppercase for hex values
 *
 * Examples:
 *
 *     printHex(std::cout, "Hello, World!") outputs "48656C6C6F2C20776F726C6421"
 *     printHex(std::cout, "Hello, World!", false) outputs "48656c6c6f2c20776f726c6421"
 *
 * Each octet is always represented as two hex characters ("00" for octet==0).
 *
 * The output string is a continuous sequence of hex characters without any whitespace separators.
 */
void
printHex(std::ostream& os, const uint8_t* buffer, size_t length, bool isUpperCase = true);

/**
 * @brief Output the hex representation of the bytes in the @p buffer to the output stream @p os
 *
 * @param os Output stream
 * @param buffer The array of bytes
 * @param isUpperCase if true (default) output use uppercase for hex values
 */
void
printHex(std::ostream& os, const Buffer& buffer, bool isUpperCase = true);

/**
 * @brief Return the hex representation of the bytes in array
 *
 * @param buffer The array of bytes
 * @param length Size of the array
 * @param isUpperCase if true (default) output use uppercase for hex values
 *
 * Examples:
 *
 *     toHex("Hello, World!") == "48656C6C6F2C20776F726C6421"
 *     toHex("Hello, World!", false) == "48656c6c6f2c20776f726c6421"
 *
 * Each octet is always represented as two hex characters ("00" for octet==0).
 *
 * The output string is a continuous sequence of hex characters without any whitespace separators.
 */
std::string
toHex(const uint8_t* buffer, size_t length, bool isUpperCase = true);

/**
 * @brief Return the hex representation of the bytes in the @p buffer to the output stream @p os
 *
 * @param buffer The array of bytes
 * @param isUpperCase if true (default) output use uppercase for hex values
 */
std::string
toHex(const Buffer& buffer, bool isUpperCase = true);

/**
 * @brief Convert the hex string to buffer
 * @param hexString sequence of pairs of hex numbers (lower and upper case can be mixed)
 *        without any whitespace separators (e.g., "48656C6C6F2C20776F726C6421")
 * @throw StringHelperError if input is invalid
 */
shared_ptr<const Buffer>
fromHex(const std::string& hexString);

/**
 * @brief Modify str in place to erase whitespace on the left
 */
void
trimLeft(std::string& str);

/**
 * @brief Modify str in place to erase whitespace on the right
 */
void
trimRight(std::string& str);

/**
 * @brief Modify str in place to erase whitespace on the left and right
 */
void
trim(std::string& str);

/**
 * @brief Convert the hex character to an integer from 0 to 15, or -1 if not a hex character
 */
int
fromHexChar(uint8_t c);

/**
 * @brief Decode a percent-encoded string
 * @see RFC 3986 section 2
 *
 * When % is not followed by two hex characters, the output is not transformed.
 *
 * Examples:
 *
 *     unescape("hello%20world") == "hello world"
 *     unescape("hello%20world%FooBar") == "hello world%FooBar"
 */
std::string
unescape(const std::string& str);

} // namespace ndn

#endif // NDN_STRING_HELPER_HPP
