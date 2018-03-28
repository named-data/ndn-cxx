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

#ifndef NDN_TESTS_BLOCK_LITERAL_HPP
#define NDN_TESTS_BLOCK_LITERAL_HPP

#include "encoding/block.hpp"
#include "encoding/buffer-stream.hpp"
#include "security/transform.hpp"

namespace ndn {
namespace tests {

/** \brief Construct a \c Block from hexadecimal \p input.
 *  \param input a string containing hexadecimal bytes and comments.
 *               0-9 and upper-case A-F are input; all other characters are comments.
 *  \param len length of \p input.
 *  \throw security::transform::Error input has odd number of hexadecimal digits.
 *  \throw tlv::Error input cannot be parsed into valid \c Block.
 *
 *  Example
 *  \code
 *  Block nameBlock = "0706 080141 080142"_block;
 *  Block nackBlock = "FD032005 reason(no-route)=FD03210196"_block;
 *  \endcode
 */
inline Block
operator "" _block(const char* input, std::size_t len)
{
  namespace t = ndn::security::transform;
  t::StepSource ss;
  OBufferStream os;
  ss >> t::hexDecode() >> t::streamSink(os);

  for (const char* end = input + len; input != end; ++input) {
    if (std::strchr("0123456789ABCDEF", *input) != nullptr) {
      ss.write(reinterpret_cast<const uint8_t*>(input), 1);
    }
  }
  ss.end();

  return Block(os.buf());
}

} // namespace tests
} // namespace ndn

#endif // NDN_TESTS_BLOCK_LITERAL_HPP
