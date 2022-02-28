/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#ifndef NDN_CXX_MGMT_STATUS_DATASET_CONTEXT_HPP
#define NDN_CXX_MGMT_STATUS_DATASET_CONTEXT_HPP

#include "ndn-cxx/interest.hpp"
#include "ndn-cxx/mgmt/control-response.hpp"

namespace ndn {
namespace mgmt {

/**
 * \brief Provides a context for generating the response to a StatusDataset request.
 */
class StatusDatasetContext : noncopyable
{
public:
  /**
   * \brief Returns the prefix of Data packets, with version component but without segment component.
   */
  const Name&
  getPrefix() const
  {
    return m_prefix;
  }

  /**
   * \brief Changes the prefix of the response Data packets.
   * \param prefix the prefix; it must start with the Interest Name, may contain a version
   *               component, but must not contain a segment component
   *
   * StatusDatasetHandler may change the prefix of Data packets with this method,
   * before sending any response.
   *
   * The version component is optional, and will be generated from the current timestamp if omitted.
   *
   * \throw std::invalid_argument \p prefix does not satisfy the requirements
   * \throw std::logic_error append(), end(), or reject() has already been invoked
   */
  StatusDatasetContext&
  setPrefix(const Name& prefix);

  /**
   * \brief Appends a sequence of bytes to the response.
   * \throw std::logic_error end() or reject() has already been invoked
   */
  void
  append(span<const uint8_t> bytes);

  /**
   * \brief Finalizes the response successfully after appending zero or more blocks.
   * \throw std::logic_error reject() has already been invoked
   */
  void
  end();

  /**
   * \brief Rejects the request.
   * \param resp Content of producer-generated NACK
   *
   * This should be invoked when the incoming Interest is malformed.
   * A producer-generated NACK will be returned to the requester.
   *
   * \throw std::logic_error append() or end() has already been invoked
   */
  void
  reject(const ControlResponse& resp = ControlResponse().setCode(400));

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  using DataSender = std::function<void(const Name& dataName, const Block& content, bool isFinalBlock)>;
  using NackSender = std::function<void(const ControlResponse&)>;

  StatusDatasetContext(const Interest& interest, DataSender dataSender, NackSender nackSender);

private:
  friend class Dispatcher;

  const Interest& m_interest;
  DataSender m_dataSender;
  NackSender m_nackSender;
  Name m_prefix;
  std::vector<uint8_t> m_buffer;
  uint64_t m_segmentNo = 0;

  enum class State {
    INITIAL,   ///< none of append(), end(), reject() has been invoked
    RESPONDED, ///< append() has been invoked
    FINALIZED, ///< end() or reject() has been invoked
  };
  State m_state = State::INITIAL;
};

} // namespace mgmt
} // namespace ndn

#endif // NDN_CXX_MGMT_STATUS_DATASET_CONTEXT_HPP
