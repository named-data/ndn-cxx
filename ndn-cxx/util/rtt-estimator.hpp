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
 */

#ifndef NDN_CXX_UTIL_RTT_ESTIMATOR_HPP
#define NDN_CXX_UTIL_RTT_ESTIMATOR_HPP

#include "ndn-cxx/util/signal.hpp"
#include "ndn-cxx/util/time.hpp"

namespace ndn {
namespace util {

/**
 * @brief RTT Estimator.
 *
 * This class implements the "Mean-Deviation" RTT estimator, as discussed in RFC 6298,
 * with the modifications to RTO calculation described in RFC 7323 Appendix G.
 */
class RttEstimator
{
public:
  using MillisecondsDouble = time::duration<double, time::milliseconds::period>;

  class Options
  {
  public:
    constexpr
    Options() noexcept
    {
    }

  public:
    double alpha = 0.125; ///< weight of exponential moving average for smoothed RTT
    double beta = 0.25; ///< weight of exponential moving average for RTT variation
    MillisecondsDouble initialRto{1000.0}; ///< initial RTO value
    MillisecondsDouble minRto{200.0}; ///< lower bound of RTO
    MillisecondsDouble maxRto{60000.0}; ///< upper bound of RTO
    int k = 4; ///< RTT variation multiplier used when calculating RTO
    int rtoBackoffMultiplier = 2; ///< RTO multiplier used in backoff operation
  };

  /**
   * @brief Creates an RTT estimator.
   *
   * Configures the RTT estimator with the default parameters if an instance of Options
   * is not passed to the constructor.
   */
  explicit
  RttEstimator(const Options& options = Options());

  /**
   * @brief Records a new RTT measurement.
   *
   * @param rtt the sampled RTT
   * @param nExpectedSamples number of expected samples, must be greater than 0. It should be
   *                         set to the current number of in-flight Interests. Please refer to
   *                         Appendix G of RFC 7323 for details.
   * @param segNum segment number or other opaque sample identifier. This value is not used by
   *               the estimator, but is passed verbatim to afterMeasurement() signal subscribers.
   *
   * @note Do not call this function with RTT samples from retransmitted Interests (per Karn's algorithm).
   */
  void
  addMeasurement(MillisecondsDouble rtt, size_t nExpectedSamples,
                 optional<uint64_t> segNum = nullopt);

  /**
   * @brief Returns the estimated RTO value.
   */
  MillisecondsDouble
  getEstimatedRto() const
  {
    return m_rto;
  }

  /**
   * @brief Returns the minimum RTT observed.
   */
  MillisecondsDouble
  getMinRtt() const
  {
    return m_rttMin;
  }

  /**
   * @brief Returns the maximum RTT observed.
   */
  MillisecondsDouble
  getMaxRtt() const
  {
    return m_rttMax;
  }

  /**
   * @brief Returns the average RTT.
   */
  MillisecondsDouble
  getAvgRtt() const
  {
    return m_rttAvg;
  }

  /**
   * @brief Backoff RTO by a factor of Options::rtoBackoffMultiplier.
   */
  void
  backoffRto();

public:
  struct Sample
  {
    MillisecondsDouble rtt;     ///< measured RTT
    MillisecondsDouble sRtt;    ///< smoothed RTT
    MillisecondsDouble rttVar;  ///< RTT variation
    MillisecondsDouble rto;     ///< retransmission timeout
    optional<uint64_t> segNum;  ///< segment number, see description in addMeasurement()
  };

  Signal<RttEstimator, Sample> afterMeasurement;

private:
  const Options m_options;

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  MillisecondsDouble m_sRtt;    ///< smoothed round-trip time
  MillisecondsDouble m_rttVar;  ///< round-trip time variation
  MillisecondsDouble m_rto;     ///< retransmission timeout

private:
  MillisecondsDouble m_rttMin;
  MillisecondsDouble m_rttMax;
  MillisecondsDouble m_rttAvg;
  int64_t m_nRttSamples;
};

} // namespace util
} // namespace ndn

#endif // NDN_CXX_UTIL_RTT_ESTIMATOR_HPP
