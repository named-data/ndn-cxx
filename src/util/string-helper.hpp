/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 *
 * @author Jeff Thompson <jefft0@remap.ucla.edu>
 */

#ifndef NDN_STRING_HELPER_HPP
#define NDN_STRING_HELPER_HPP

#include <string>
#include <sstream>

namespace ndn {

static const char* WHITESPACE_CHARS = " \n\r\t";

/**
 * @brief Return the hex representation of the bytes in array
 *
 * @param array The array of bytes
 * @param arraySize Size of the array
 */
inline std::string
toHex(const uint8_t* array, size_t arraySize)
{
  if (array == 0 || arraySize == 0)
    return "";

  std::ostringstream result;
  result.flags(std::ios::hex | std::ios::uppercase);
  for (size_t i = 0; i < arraySize; ++i) {
    uint8_t x = array[i];
    if (x < 16)
      result << '0';
    result << (unsigned int)x;
  }

  return result.str();
}

/**
 * @brief Modify str in place to erase whitespace on the left
 */
inline void
trimLeft(std::string& str)
{
  size_t found = str.find_first_not_of(WHITESPACE_CHARS);
  if (found != std::string::npos) {
    if (found > 0)
      str.erase(0, found);
  }
  else
    // All whitespace
    str.clear();
}

/**
 * @brief Modify str in place to erase whitespace on the right
 */
inline void
trimRight(std::string& str)
{
  size_t found = str.find_last_not_of(WHITESPACE_CHARS);
  if (found != std::string::npos) {
    if (found + 1 < str.size())
      str.erase(found + 1);
  }
  else
    // All whitespace
    str.clear();
}

/**
 * @brief Modify str in place to erase whitespace on the left and right
 */
inline void
trim(std::string& str)
{
  trimLeft(str);
  trimRight(str);
}

/**
 * @brief Convert the hex character to an integer from 0 to 15, or -1 if not a hex character
 */
inline int
fromHexChar(uint8_t c)
{
  if (c >= '0' && c <= '9')
    return (int)c - (int)'0';
  else if (c >= 'A' && c <= 'F')
    return (int)c - (int)'A' + 10;
  else if (c >= 'a' && c <= 'f')
    return (int)c - (int)'a' + 10;
  else
    return -1;
}

/**
 * @brief Return a copy of str, converting each escaped "%XX" to the char value
 */
inline std::string
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
        result << (uint8_t)(16 * hi + lo);

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

#endif // NDN_STRING_HELPER_HPP
