/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (C) 2016-2019, Arizona Board of Regents.
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
 *
 * @author Shuo Yang
 * @author Weiwei Liu
 * @author Chavoosh Ghasemi
 * @author Davide Pesavento
 */

#ifndef NDN_CXX_UTIL_RTT_ESTIMATOR_HPP
#define NDN_CXX_UTIL_RTT_ESTIMATOR_HPP

#include "ndn-cxx/util/time.hpp"

namespace ndn {
namespace util {

/**
 * @brief RTT/RTO estimator.
 *
 * This class implements the "Mean-Deviation" RTT estimator, as discussed in RFC 6298,
 * with the modifications to RTO calculation described in RFC 7323 Appendix G.
 */
class RttEstimator
{
public:
  struct Options
  {
    double alpha = 0.125; ///< weight of exponential moving average for smoothed RTT
    double beta = 0.25; ///< weight of exponential moving average for RTT variation
    time::nanoseconds initialRto = 1_s; ///< initial RTO value
    time::nanoseconds minRto = 200_ms; ///< lower bound of RTO
    time::nanoseconds maxRto = 1_min; ///< upper bound of RTO
    int k = 4; ///< RTT variation multiplier used when calculating RTO
    int rtoBackoffMultiplier = 2; ///< RTO multiplier used in backoff operation
  };

  /**
   * @brief Constructor.
   * @param options options for the estimator; if nullptr, a default set of options is used
   */
  explicit
  RttEstimator(shared_ptr<const Options> options = nullptr);

  /**
   * @brief Records a new RTT measurement.
   * @param rtt the sampled RTT
   * @param nExpectedSamples number of expected samples, must be greater than 0. It should be
   *                         set to the current number of in-flight Interests. Please refer to
   *                         Appendix G of RFC 7323 for details.
   * @note Do not call this function with RTT samples from retransmitted Interests (per Karn's algorithm).
   */
  void
  addMeasurement(time::nanoseconds rtt, size_t nExpectedSamples = 1);

  bool
  hasSamples() const
  {
    return m_sRtt != -1_ns;
  }

  /**
   * @brief Returns the estimated RTO value.
   */
  time::nanoseconds
  getEstimatedRto() const
  {
    return m_rto;
  }

  /**
   * @brief Returns the smoothed RTT value (SRTT).
   * @pre `hasSamples() == true`
   */
  time::nanoseconds
  getSmoothedRtt() const
  {
    return m_sRtt;
  }

  /**
   * @brief Returns the RTT variation (RTTVAR).
   * @pre `hasSamples() == true`
   */
  time::nanoseconds
  getRttVariation() const
  {
    return m_rttVar;
  }

  /**
   * @brief Backoff RTO by a factor of Options::rtoBackoffMultiplier.
   */
  void
  backoffRto();

protected:
  shared_ptr<const Options> m_options;

private:
  time::nanoseconds m_sRtt{-1};   ///< smoothed round-trip time
  time::nanoseconds m_rttVar{-1}; ///< round-trip time variation
  time::nanoseconds m_rto;        ///< retransmission timeout
};

/**
 * @brief RTT/RTO estimator that also maintains min/max/average RTT statistics.
 */
class RttEstimatorWithStats : private RttEstimator
{
public:
  using RttEstimator::Options;
  using RttEstimator::RttEstimator;

  using RttEstimator::hasSamples;
  using RttEstimator::getEstimatedRto;
  using RttEstimator::getSmoothedRtt;
  using RttEstimator::getRttVariation;
  using RttEstimator::backoffRto;

  /**
   * @brief Records a new RTT measurement.
   * @param rtt the sampled RTT
   * @param nExpectedSamples number of expected samples, must be greater than 0. It should be
   *                         set to the current number of in-flight Interests. Please refer to
   *                         Appendix G of RFC 7323 for details.
   * @note Do not call this function with RTT samples from retransmitted Interests (per Karn's algorithm).
   */
  void
  addMeasurement(time::nanoseconds rtt, size_t nExpectedSamples = 1);

  /**
   * @brief Returns the minimum RTT observed.
   */
  time::nanoseconds
  getMinRtt() const
  {
    return m_rttMin;
  }

  /**
   * @brief Returns the maximum RTT observed.
   */
  time::nanoseconds
  getMaxRtt() const
  {
    return m_rttMax;
  }

  /**
   * @brief Returns the average RTT.
   */
  time::nanoseconds
  getAvgRtt() const
  {
    return m_rttAvg;
  }

private:
  time::nanoseconds m_rttMin = time::nanoseconds::max();
  time::nanoseconds m_rttMax = time::nanoseconds::min();
  time::nanoseconds m_rttAvg = 0_ns;
  int64_t m_nRttSamples = 0;
};

} // namespace util
} // namespace ndn

#endif // NDN_CXX_UTIL_RTT_ESTIMATOR_HPP
