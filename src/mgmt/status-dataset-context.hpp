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

#ifndef NDN_MGMT_STATUS_DATASET_CONTEXT_HPP
#define NDN_MGMT_STATUS_DATASET_CONTEXT_HPP

#include "../interest.hpp"
#include "../data.hpp"
#include "../util/time.hpp"
#include "../encoding/encoding-buffer.hpp"
#include "control-response.hpp"

namespace ndn {
namespace mgmt {

/** \brief provides a context for generating response to a StatusDataset request
 */
class StatusDatasetContext : noncopyable
{
public:
  /** \return prefix of Data packets, with version component but without segment component
   */
  const Name&
  getPrefix() const;

  /** \brief change prefix of Data packets
   *  \param prefix the prefix; it must start with Interest Name, may contain version component,
   *         but must not contain segment component
   *  \throw std::invalid_argument prefix does not start with Interest Name
   *  \throw std::domain_error append, end, or reject has been invoked
   *
   *  StatusDatasetHandler may change the prefix of Data packets with this method,
   *  before sending any response.
   *  The version component is optional, and will be generated from current timestamp when omitted.
   */
  StatusDatasetContext&
  setPrefix(const Name& prefix);

  /** \return expiration duration for this dataset response
   */
  const time::milliseconds&
  getExpiry() const;

  /** \brief set expiration duration
   *
   *  The response will be cached for the specified duration.
   *  Incoming Interest that matches a cached response will be satisfied with that response,
   *  without invoking StatusDatasetHandler again.
   */
  StatusDatasetContext&
  setExpiry(const time::milliseconds& expiry);

  /** \brief append a Block to the response
   *  \throw std::domain_error end or reject has been invoked
   */
  void
  append(const Block& block);

  /** \brief end the response successfully after appending zero or more blocks
   *  \throw std::domain_error reject has been invoked
   */
  void
  end();

  /** \brief declare the non-existence of a response
   *  \throw std::domain_error append or end has been invoked
   *
   *  This should be invoked when the incoming Interest is malformed.
   *  A producer-generated NACK will be returned to requester.
   *
   *  \param resp Content of producer-generated NACK
   */
  void
  reject(const ControlResponse& resp = ControlResponse().setCode(400));

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  typedef std::function<void(const Name& dataName, const Block& content, time::milliseconds imsFresh,
                             bool isFinalBlock)> DataSender;
  typedef std::function<void(const ControlResponse& resp)> NackSender;

  StatusDatasetContext(const Interest& interest,
                       const DataSender& dataSender,
                       const NackSender& nackSender);

private:
  friend class Dispatcher;

  const Interest& m_interest;
  DataSender m_dataSender;
  NackSender m_nackSender;
  Name m_prefix;
  time::milliseconds m_expiry;

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  shared_ptr<EncodingBuffer> m_buffer;
  uint64_t m_segmentNo;

  enum class State {
    INITIAL, ///< none of .append, .end, .reject has been invoked
    RESPONDED, ///< .append has been invoked
    FINALIZED ///< .end or .reject has been invoked
  };
  State m_state;
};

} // namespace mgmt
} // namespace ndn

#endif // NDN_MGMT_STATUS_DATASET_CONTEXT_HPP
