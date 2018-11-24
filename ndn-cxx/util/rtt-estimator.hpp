/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (C) 2016-2018, Arizona Board of Regents.
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

public:
  class Options
  {
  public:
    constexpr
    Options() noexcept
    {
    }

  public:
    double alpha = 0.125; ///< weight of exponential moving average for meanRtt
    double beta = 0.25; ///< weight of exponential moving average for varRtt
    int k = 4; ///< factor of RTT variation when calculating RTO
    MillisecondsDouble initialRto = MillisecondsDouble(1000.0); ///< initial RTO value
    MillisecondsDouble minRto = MillisecondsDouble(200.0); ///< lower bound of RTO
    MillisecondsDouble maxRto = MillisecondsDouble(20000.0); ///< upper bound of RTO
    int rtoBackoffMultiplier = 2;
  };

  /**
   * @brief Create a RTT Estimator
   *
   * Configures the RTT Estimator with the default parameters if an instance of Options
   * is not passed to the constructor.
   */
  explicit
  RttEstimator(const Options& options = Options());

  /**
   * @brief Add a new RTT measurement to the estimator.
   *
   * @param rtt the sampled rtt
   * @param nExpectedSamples number of expected samples, must be greater than 0.
   *        It should be set to current number of in-flight Interests. Please
   *        refer to Appendix G of RFC 7323 for details.
   * @note Don't add RTT measurements for retransmissions
   */
  void
  addMeasurement(MillisecondsDouble rtt, size_t nExpectedSamples);

  /**
   * @brief Returns the estimated RTO value
   */
  MillisecondsDouble
  getEstimatedRto() const
  {
    return m_rto;
  }

  /**
   * @brief Returns the minimum RTT observed
   */
  MillisecondsDouble
  getMinRtt() const
  {
    return m_rttMin;
  }

  /**
   * @brief Returns the maximum RTT observed
   */
  MillisecondsDouble
  getMaxRtt() const
  {
    return m_rttMax;
  }

  /**
   * @brief Returns the average RTT
   */
  MillisecondsDouble
  getAvgRtt() const
  {
    return m_rttAvg;
  }

  /**
   * @brief Backoff RTO by a factor of Options::rtoBackoffMultiplier
   */
  void
  backoffRto();

private:
  const Options m_options;
  MillisecondsDouble m_sRtt; ///< smoothed round-trip time
  MillisecondsDouble m_rttVar; ///< round-trip time variation
  MillisecondsDouble m_rto; ///< retransmission timeout
  MillisecondsDouble m_rttMin;
  MillisecondsDouble m_rttMax;
  MillisecondsDouble m_rttAvg;
  int64_t m_nRttSamples; ///< number of RTT samples
};

} // namespace util
} // namespace ndn

#endif // NDN_CXX_UTIL_RTT_ESTIMATOR_HPP
