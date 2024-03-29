/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2024 Regents of the University of California.
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

#ifndef NDN_CXX_SECURITY_TRANSFORM_BOOL_SINK_HPP
#define NDN_CXX_SECURITY_TRANSFORM_BOOL_SINK_HPP

#include "ndn-cxx/security/transform/transform-base.hpp"

namespace ndn::security::transform {

/**
 * @brief A sink which outputs only one boolean value.
 *
 * It checks the first byte that is ever written into the sink,
 * and set output true if the byte is non-zero, otherwise false.
 */
class BoolSink : public Sink
{
public:
  /**
   * @brief Create a bool sink whose output will be stored in @p value.
   */
  explicit
  BoolSink(bool& value)
    : m_value(value)
  {
  }

private:
  /**
   * @brief Check the first byte that is ever received and set the boolean variable.
   *
   * @return the same value as @p size.
   */
  size_t
  doWrite(span<const uint8_t> buf) final;

  /**
   * @brief Finalize sink processing.
   */
  void
  doEnd() final
  {
    // nothing to do
  }

private:
  bool m_hasValue = false;
  bool& m_value;
};

unique_ptr<Sink>
boolSink(bool& value);

} // namespace ndn::security::transform

#endif // NDN_CXX_SECURITY_TRANSFORM_BOOL_SINK_HPP
