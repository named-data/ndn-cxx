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

BOOST_AUTO_TEST_CASE(CopyAssign)
{
  RttEstimator est1;
  RttEstimator est2;

  est1.addMeasurement(100_ms);
  est1.addMeasurement(150_ms);
  est2.addMeasurement(75_ms);
  est2.addMeasurement(70_ms);

  BOOST_CHECK_NE(est1.getEstimatedRto(), est2.getEstimatedRto());
  BOOST_CHECK_NE(est1.getRttVariation(), est2.getRttVariation());
  BOOST_CHECK_NE(est1.getSmoothedRtt(), est2.getSmoothedRtt());

  est1 = est2;

  BOOST_CHECK_EQUAL(est1.getEstimatedRto(), est2.getEstimatedRto());
  BOOST_CHECK_EQUAL(est1.getRttVariation(), est2.getRttVariation());
  BOOST_CHECK_EQUAL(est1.getSmoothedRtt(), est2.getSmoothedRtt());
}

BOOST_AUTO_TEST_CASE(EstimatedRto)
{
  auto opts = make_shared<RttEstimator::Options>();
  opts->initialRto = 400_ms;
  opts->maxRto = 2_s;
  RttEstimator rttEstimator(opts);

  // check initial values
  BOOST_CHECK_EQUAL(rttEstimator.hasSamples(), false);
  BOOST_CHECK_EQUAL(rttEstimator.getSmoothedRtt(), -1_ns);
  BOOST_CHECK_EQUAL(rttEstimator.getRttVariation(), -1_ns);
  BOOST_CHECK_EQUAL(rttEstimator.getEstimatedRto(), opts->initialRto);

  // first measurement
  rttEstimator.addMeasurement(200_ms);

  BOOST_CHECK_EQUAL(rttEstimator.hasSamples(), true);
  BOOST_CHECK_EQUAL(rttEstimator.getSmoothedRtt(), 200_ms);
  BOOST_CHECK_EQUAL(rttEstimator.getRttVariation(), 100_ms);
  BOOST_CHECK_EQUAL(rttEstimator.getEstimatedRto(), 600_ms);

  rttEstimator.addMeasurement(100_ms, 1);

  BOOST_CHECK_EQUAL(rttEstimator.hasSamples(), true);
  BOOST_CHECK_EQUAL(rttEstimator.getSmoothedRtt(), 187500_us);
  BOOST_CHECK_EQUAL(rttEstimator.getRttVariation(), 100000_us);
  BOOST_CHECK_EQUAL(rttEstimator.getEstimatedRto(), 587500_us);

  // expected samples larger than 1
  rttEstimator.addMeasurement(50_ms, 5);

  BOOST_CHECK_EQUAL(rttEstimator.hasSamples(), true);
  BOOST_CHECK_EQUAL(rttEstimator.getSmoothedRtt(), 184062500_ns);
  BOOST_CHECK_EQUAL(rttEstimator.getRttVariation(), 101875000_ns);
  BOOST_CHECK_EQUAL(rttEstimator.getEstimatedRto(), 591562500_ns);

  // check if minRto works
  for (int i = 0; i < 20; i++) {
    rttEstimator.addMeasurement(10_ms);
  }

  BOOST_CHECK_EQUAL(rttEstimator.getSmoothedRtt(), 22046646_ns);
  BOOST_CHECK_EQUAL(rttEstimator.getEstimatedRto(), opts->minRto);

  // check if maxRto works
  for (int i = 0; i < 10; i++) {
    rttEstimator.addMeasurement(1_s);
    rttEstimator.addMeasurement(10_ms);
  }

  BOOST_CHECK_EQUAL(rttEstimator.getSmoothedRtt(), 440859284_ns);
  BOOST_CHECK_EQUAL(rttEstimator.getEstimatedRto(), opts->maxRto);
}

BOOST_AUTO_TEST_CASE(BackoffRto)
{
  auto opts = make_shared<RttEstimator::Options>();
  opts->initialRto = 500_ms;
  opts->maxRto = 4_s;
  RttEstimator rttEstimator(opts);

  rttEstimator.backoffRto();
  BOOST_CHECK_EQUAL(rttEstimator.getEstimatedRto(), 1_s);

  // check if minRto works
  for (int i = 0; i < 10; i++) {
    rttEstimator.addMeasurement(5_ms);
  }
  rttEstimator.backoffRto();
  BOOST_CHECK_EQUAL(rttEstimator.getEstimatedRto(), 400_ms);

  // check if maxRto works
  for (int i = 0; i < 10; i++) {
    rttEstimator.addMeasurement(5_s);
  }
  rttEstimator.backoffRto();
  BOOST_CHECK_EQUAL(rttEstimator.getEstimatedRto(), 4_s);
}

BOOST_AUTO_TEST_CASE(Stats)
{
  RttEstimatorWithStats rttEstimator;

  // check initial values
  BOOST_CHECK_EQUAL(rttEstimator.getMinRtt().count(), std::numeric_limits<time::nanoseconds::rep>::max());
  BOOST_CHECK_EQUAL(rttEstimator.getAvgRtt().count(), 0);
  BOOST_CHECK_EQUAL(rttEstimator.getMaxRtt().count(), std::numeric_limits<time::nanoseconds::rep>::min());

  // start with three samples
  rttEstimator.addMeasurement(100_ms);
  rttEstimator.addMeasurement(400_ms);
  rttEstimator.addMeasurement(250_ms);

  BOOST_CHECK_EQUAL(rttEstimator.getMinRtt(), 100_ms);
  BOOST_CHECK_EQUAL(rttEstimator.getAvgRtt(), 250_ms);
  BOOST_CHECK_EQUAL(rttEstimator.getMaxRtt(), 400_ms);

  // add another sample (new minimum)
  rttEstimator.addMeasurement(50_ms, 2);
  BOOST_CHECK_EQUAL(rttEstimator.getMinRtt(), 50_ms);
  BOOST_CHECK_EQUAL(rttEstimator.getAvgRtt(), 200_ms);
  BOOST_CHECK_EQUAL(rttEstimator.getMaxRtt(), 400_ms);

  // add another sample (new maximum)
  rttEstimator.addMeasurement(700_ms, 1);
  BOOST_CHECK_EQUAL(rttEstimator.getMinRtt(), 50_ms);
  BOOST_CHECK_EQUAL(rttEstimator.getAvgRtt(), 300_ms);
  BOOST_CHECK_EQUAL(rttEstimator.getMaxRtt(), 700_ms);
}

BOOST_AUTO_TEST_SUITE_END() // TestRttEstimator
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace tests
} // namespace util
} // namespace ndn
