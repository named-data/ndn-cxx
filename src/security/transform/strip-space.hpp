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

#ifndef NDN_CXX_SECURITY_TRANSFORM_STRIP_SPACE_HPP
#define NDN_CXX_SECURITY_TRANSFORM_STRIP_SPACE_HPP

#include "transform-base.hpp"
#include <bitset>
#include <climits>

namespace ndn {
namespace security {
namespace transform {

/** \brief strip whitespace characters from a stream
 *
 *  This transform interprets the input as a byte string, and puts all bytes except
 *  whitespace characters on the output.
 */
class StripSpace : public Transform
{
public:
  static const char* const DEFAULT_WHITESPACES;

  explicit
  StripSpace(const char* whitespaces = DEFAULT_WHITESPACES);

private:
  size_t
  convert(const uint8_t* buf, size_t buflen) final;

private:
  static constexpr size_t CHARMAP_SIZE = 1 << CHAR_BIT;
  std::bitset<CHARMAP_SIZE> m_isWhitespace; // char => whether char is whitespace
};

/** \brief constructs a StripSpace transform
 *  \param whitespaces characters classified as whitespaces, terminated with null
 */
unique_ptr<Transform>
stripSpace(const char* whitespaces = StripSpace::DEFAULT_WHITESPACES);

} // namespace transform
} // namespace security
} // namespace ndn

#endif // NDN_CXX_SECURITY_TRANSFORM_STRIP_SPACE_HPP
