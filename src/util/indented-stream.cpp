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

#include "indented-stream.hpp"

#include <vector>

#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace ndn {
namespace util {

IndentedStream::IndentedStream(std::ostream& os, const std::string& indent)
  : std::ostream(&m_buffer)
  , m_buffer(os, indent)
{
}

IndentedStream::~IndentedStream()
{
  flush();
}

IndentedStream::StreamBuf::StreamBuf(std::ostream& os, const std::string& indent)
  : m_output(os)
  , m_indent(indent)
{
}

int
IndentedStream::StreamBuf::sync()
{
  typedef boost::iterator_range<std::string::const_iterator> StringView;

  const std::string& output = str();
  std::vector<StringView> splitOutput;
  boost::split(splitOutput, output, boost::is_any_of("\n"));

  if (!splitOutput.empty() && splitOutput.back().empty()) {
    splitOutput.pop_back();
  }
  for (const StringView& line : splitOutput) {
    m_output << m_indent << line << "\n";
  }
  str(std::string());
  return 0; // success
}

} // namespace util
} // namespace ndn
