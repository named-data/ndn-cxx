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

#ifndef NDN_UTIL_INDENTED_STREAM_HPP
#define NDN_UTIL_INDENTED_STREAM_HPP

#include <ostream>
#include <sstream>
#include <string>

namespace ndn {
namespace util {

/**
 * @brief Output to stream with specified indent or prefix
 *
 * For example, the following code:
 *
 *     std::cout << "Hello" << std::endl;
 *     IndentedStream os1(std::cout, " [prefix] ");
 *     os1 << "," << "\n";
 *     {
 *       IndentedStream os2(os1, " [another prefix] ");
 *       os2 << "World!" << "\n";
 *     }
 *     // either os1 needs to go out of scope or call os1.flush()
 *
 * Will produce the following output:
 *
 *     Hello
 *      [prefix] ,
 *      [prefix] [another prefix] World!
 *
 * Based on http://stackoverflow.com/a/2212940/2150331
 */
class IndentedStream : public std::ostream
{
public:
  IndentedStream(std::ostream& os, const std::string& indent);

  ~IndentedStream() override;

private:
  // Write a stream buffer that prefixes each line
  class StreamBuf : public std::stringbuf
  {
  public:
    StreamBuf(std::ostream& os, const std::string& indent);

    int
    sync() override;

  private:
    std::ostream& m_output;
    std::string m_indent;
  };

  StreamBuf m_buffer;
};

} // namespace util
} // namespace ndn

#endif // NDN_UTIL_INDENTED_STREAM_HPP
