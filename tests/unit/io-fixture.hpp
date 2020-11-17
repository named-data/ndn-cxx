/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2020 Regents of the University of California.
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

#ifndef NDN_CXX_TESTS_UNIT_IO_FIXTURE_HPP
#define NDN_CXX_TESTS_UNIT_IO_FIXTURE_HPP

#include "tests/unit/clock-fixture.hpp"

#include <boost/asio/io_service.hpp>

namespace ndn {
namespace tests {

class IoFixture : public ClockFixture
{
private:
  void
  afterTick() final
  {
    if (m_io.stopped()) {
#if BOOST_VERSION >= 106600
      m_io.restart();
#else
      m_io.reset();
#endif
    }
    m_io.poll();
  }

protected:
  boost::asio::io_service m_io;
};

} // namespace tests
} // namespace ndn

#endif // NDN_CXX_TESTS_UNIT_IO_FIXTURE_HPP
