/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016-2019, Regents of the University of California,
 *                          Colorado State University,
 *                          University Pierre & Marie Curie, Sorbonne University.
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

#include "ndn-cxx/util/rtt-estimator.hpp"

#include "tests/boost-test.hpp"

#include <cmath>

namespace ndn {
namespace util {
namespace tests {

BOOST_AUTO_TEST_SUITE(Util)
BOOST_AUTO_TEST_SUITE(TestRttEstimator)

using Millis = RttEstimator::MillisecondsDouble;

BOOST_AUTO_TEST_CASE(MinAvgMaxRtt)
{
  RttEstimator rttEstimator;

  // check initial values
  BOOST_CHECK_CLOSE(rttEstimator.getMinRtt().count(), std::numeric_limits<double>::max(), 0.001);
  BOOST_CHECK_CLOSE(rttEstimator.getAvgRtt().count(), 0.0, 0.001);
  BOOST_CHECK_CLOSE(rttEstimator.getMaxRtt().count(), std::numeric_limits<double>::min(), 0.001);

  // start with three samples
  rttEstimator.addMeasurement(Millis(100), 1);
  rttEstimator.addMeasurement(Millis(400), 1);
  rttEstimator.addMeasurement(Millis(250), 1);

  BOOST_CHECK_CLOSE(rttEstimator.getMinRtt().count(), 100.0, 0.001);
  BOOST_CHECK_CLOSE(rttEstimator.getAvgRtt().count(), 250.0, 0.001);
  BOOST_CHECK_CLOSE(rttEstimator.getMaxRtt().count(), 400.0, 0.001);

  // add another sample (new minimum)
  rttEstimator.addMeasurement(Millis(50), 2);
  BOOST_CHECK_CLOSE(rttEstimator.getMinRtt().count(), 50.0, 0.001);
  BOOST_CHECK_CLOSE(rttEstimator.getAvgRtt().count(), 200.0, 0.001);
  BOOST_CHECK_CLOSE(rttEstimator.getMaxRtt().count(), 400.0, 0.001);

  // add another sample (new maximum)
  rttEstimator.addMeasurement(Millis(700), 1);
  BOOST_CHECK_CLOSE(rttEstimator.getMinRtt().count(), 50.0, 0.001);
  BOOST_CHECK_CLOSE(rttEstimator.getAvgRtt().count(), 300.0, 0.001);
  BOOST_CHECK_CLOSE(rttEstimator.getMaxRtt().count(), 700.0, 0.001);
}

BOOST_AUTO_TEST_CASE(EstimatedRto)
{
  RttEstimator::Options opts;
  opts.initialRto = Millis(1000);
  opts.maxRto = Millis(4000);
  RttEstimator rttEstimator(opts);

  // check initial values
  BOOST_CHECK(std::isnan(rttEstimator.m_sRtt.count()));
  BOOST_CHECK(std::isnan(rttEstimator.m_rttVar.count()));
  BOOST_CHECK_CLOSE(rttEstimator.getEstimatedRto().count(), 1000.0, 0.001);

  // first measurement
  rttEstimator.addMeasurement(Millis(100), 1);

  BOOST_CHECK_CLOSE(rttEstimator.m_sRtt.count(), 100.0, 0.001);
  BOOST_CHECK_CLOSE(rttEstimator.m_rttVar.count(), 50.0, 0.001);
  BOOST_CHECK_CLOSE(rttEstimator.getEstimatedRto().count(), 300.0, 0.001);

  rttEstimator.m_sRtt = Millis(500);
  rttEstimator.m_rttVar = Millis(100);
  rttEstimator.m_rto = Millis(900);

  rttEstimator.addMeasurement(Millis(100), 1);

  BOOST_CHECK_CLOSE(rttEstimator.m_sRtt.count(), 450.0, 0.001);
  BOOST_CHECK_CLOSE(rttEstimator.m_rttVar.count(), 175.0, 0.001);
  BOOST_CHECK_CLOSE(rttEstimator.getEstimatedRto().count(), 1150.0, 0.001);

  // expected samples larger than 1
  rttEstimator.addMeasurement(Millis(100), 5);

  BOOST_CHECK_CLOSE(rttEstimator.m_sRtt.count(), 441.25, 0.001);
  BOOST_CHECK_CLOSE(rttEstimator.m_rttVar.count(), 183.75, 0.001);
  BOOST_CHECK_CLOSE(rttEstimator.getEstimatedRto().count(), 1176.25, 0.001);

  rttEstimator.m_sRtt = Millis(100.0);
  rttEstimator.m_rttVar = Millis(30.0);
  rttEstimator.m_rto = Millis(220.0);

  // check if minRto works
  rttEstimator.addMeasurement(Millis(100), 1);

  BOOST_CHECK_CLOSE(rttEstimator.m_sRtt.count(), 100.0, 0.001);
  BOOST_CHECK_CLOSE(rttEstimator.m_rttVar.count(), 22.5, 0.001);
  BOOST_CHECK_CLOSE(rttEstimator.getEstimatedRto().count(), 200.0, 0.001);

  rttEstimator.m_sRtt = Millis(2000);
  rttEstimator.m_rttVar = Millis(400);
  rttEstimator.m_rto = Millis(3600);

  // check if maxRto works
  rttEstimator.addMeasurement(Millis(100), 1);

  BOOST_CHECK_CLOSE(rttEstimator.m_sRtt.count(), 1762.5, 0.001);
  BOOST_CHECK_CLOSE(rttEstimator.m_rttVar.count(), 775.0, 0.001);
  BOOST_CHECK_CLOSE(rttEstimator.getEstimatedRto().count(), 4000.0, 0.001);
}

BOOST_AUTO_TEST_CASE(BackoffRto)
{
  RttEstimator::Options opts;
  opts.initialRto = Millis(500);
  opts.maxRto = Millis(4000);
  RttEstimator rttEstimator(opts);

  rttEstimator.backoffRto();
  BOOST_CHECK_CLOSE(rttEstimator.getEstimatedRto().count(), 1000.0, 0.001);

  // check if minRto works
  rttEstimator.m_rto = Millis(10);
  rttEstimator.backoffRto();
  BOOST_CHECK_CLOSE(rttEstimator.getEstimatedRto().count(), 200.0, 0.001);

  // check if maxRto works
  rttEstimator.m_rto = Millis(3000);
  rttEstimator.backoffRto();
  BOOST_CHECK_CLOSE(rttEstimator.getEstimatedRto().count(), 4000.0, 0.001);
}

BOOST_AUTO_TEST_CASE(AfterMeasurement)
{
  RttEstimator rttEstimator;

  int nHandlerInvocations = 0;
  rttEstimator.afterMeasurement.connectSingleShot([&nHandlerInvocations] (const auto& sample) {
    ++nHandlerInvocations;
    BOOST_CHECK_CLOSE(sample.rtt.count(), 80.0, 0.001);
    BOOST_CHECK_CLOSE(sample.sRtt.count(), 80.0, 0.001);
    BOOST_CHECK_CLOSE(sample.rttVar.count(), 40.0, 0.001);
    BOOST_CHECK_CLOSE(sample.rto.count(), 240.0, 0.001);
    BOOST_CHECK(!sample.segNum.has_value());
  });
  rttEstimator.addMeasurement(Millis(80), 1);
  BOOST_CHECK_EQUAL(nHandlerInvocations, 1);

  rttEstimator.afterMeasurement.connectSingleShot([&nHandlerInvocations] (const auto& sample) {
    ++nHandlerInvocations;
    BOOST_CHECK_CLOSE(sample.rtt.count(), 40.0, 0.001);
    BOOST_CHECK_CLOSE(sample.sRtt.count(), 75.0, 0.001);
    BOOST_CHECK_CLOSE(sample.rttVar.count(), 40.0, 0.001);
    BOOST_CHECK_CLOSE(sample.rto.count(), 235.0, 0.001);
    BOOST_CHECK(sample.segNum == 42U);
  });
  rttEstimator.addMeasurement(Millis(40), 1, 42);
  BOOST_CHECK_EQUAL(nHandlerInvocations, 2);
}

BOOST_AUTO_TEST_SUITE_END() // TestRttEstimator
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace tests
} // namespace util
} // namespace ndn
