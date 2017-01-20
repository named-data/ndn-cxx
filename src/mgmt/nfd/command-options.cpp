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

#include "command-options.hpp"

namespace ndn {
namespace nfd {

const time::milliseconds CommandOptions::DEFAULT_TIMEOUT(10000);
const Name CommandOptions::DEFAULT_PREFIX("ndn:/localhost/nfd");

CommandOptions::CommandOptions()
  : m_timeout(DEFAULT_TIMEOUT)
  , m_prefix(DEFAULT_PREFIX)
{
}

CommandOptions&
CommandOptions::setTimeout(const time::milliseconds& timeout)
{
  if (timeout <= time::milliseconds::zero()) {
    BOOST_THROW_EXCEPTION(std::out_of_range("Timeout must be positive"));
  }

  m_timeout = timeout;
  return *this;
}

CommandOptions&
CommandOptions::setPrefix(const Name& prefix)
{
  m_prefix = prefix;
  return *this;
}

CommandOptions&
CommandOptions::setSigningInfo(const security::SigningInfo& signingInfo)
{
  m_signingInfo = signingInfo;
  return *this;
}

} // namespace nfd
} // namespace ndn
