/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#include "ndn-cxx/util/random.hpp"
#include "ndn-cxx/security/impl/openssl.hpp"

#include "tests/boost-test.hpp"

#include <array>
#include <cmath>
#include <thread>

#include <boost/mpl/vector.hpp>

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(Util)
BOOST_AUTO_TEST_SUITE(TestRandom)

BOOST_AUTO_TEST_CASE(ThreadLocalRng)
{
  random::RandomNumberEngine* r1 = &random::getRandomNumberEngine();
  random::RandomNumberEngine* r2 = nullptr;
  std::thread t([&r2] { r2 = &random::getRandomNumberEngine(); });
  t.join();
  random::RandomNumberEngine* r3 = &random::getRandomNumberEngine();

  BOOST_CHECK(r2 != nullptr);
  BOOST_CHECK_NE(r1, r2);
  BOOST_CHECK_EQUAL(r1, r3);
}

class PseudoRandomWord32
{
public:
  static uint32_t
  generate()
  {
    return random::generateWord32();
  }
};

class PseudoRandomWord64
{
public:
  static uint64_t
  generate()
  {
    return random::generateWord64();
  }
};

class SecureRandomWord32
{
public:
  static uint32_t
  generate()
  {
    return random::generateSecureWord32();
  }
};

class SecureRandomWord64
{
public:
  static uint64_t
  generate()
  {
    return random::generateSecureWord64();
  }
};

using RandomGenerators = boost::mpl::vector<PseudoRandomWord32,
                                            PseudoRandomWord64,
                                            SecureRandomWord32,
                                            SecureRandomWord64>;

static double
getDeviation(const std::vector<uint32_t>& counts, size_t size)
{
  // Kolmogorov-Smirnov Goodness-of-Fit Test
  // http://www.itl.nist.gov/div898/handbook/eda/section3/eda35g.htm

  std::vector<double> edf(counts.size(), 0.0);
  double probability = 0.0;
  for (size_t i = 0; i < counts.size(); i++) {
    probability += 1.0 * counts[i] / size;
    edf[i] = probability;
  }

  double t = 0.0;
  for (size_t i = 0; i < counts.size(); i++) {
    t = std::max(t, std::abs(edf[i] - (i * 1.0 / counts.size())));
  }

  return t;
}

BOOST_AUTO_TEST_CASE_TEMPLATE(GoodnessOfFit, RandomGenerator, RandomGenerators)
{
  const size_t MAX_BINS = 32;
  const uint32_t MAX_ITERATIONS = 35;

  std::vector<uint32_t> counts(MAX_BINS, 0);
  for (uint32_t i = 0; i < MAX_ITERATIONS; i++) {
    counts[RandomGenerator::generate() % MAX_BINS]++;
  }

  // Check if it is uniform distribution with confidence 0.95
  // http://dlc.erieri.com/onlinetextbook/index.cfm?fuseaction=textbook.appendix&FileName=Table7
  BOOST_CHECK_LE(getDeviation(counts, MAX_ITERATIONS), 0.230);
}

BOOST_AUTO_TEST_CASE(GenerateSecureBytes)
{
  // Kolmogorov-Smirnov Goodness-of-Fit Test
  // http://www.itl.nist.gov/div898/handbook/eda/section3/eda35g.htm

  std::array<uint8_t, 1024> buf;
  random::generateSecureBytes(buf.data(), buf.size());

  std::vector<uint32_t> counts(256, 0);
  for (size_t i = 0; i < buf.size(); i++) {
    counts[buf[i]]++;
  }

  // Check if it is uniform distribution with confidence 0.95
  // http://dlc.erieri.com/onlinetextbook/index.cfm?fuseaction=textbook.appendix&FileName=Table7
  BOOST_CHECK_LE(getDeviation(counts, buf.size()), 0.230);
}

// This fixture uses OpenSSL routines to set a dummy random generator that always fails
class FailRandMethodFixture
{
public:
  FailRandMethodFixture()
    : m_dummyRandMethod{&FailRandMethodFixture::seed,
                        &FailRandMethodFixture::bytes,
                        &FailRandMethodFixture::cleanup,
                        &FailRandMethodFixture::add,
                        &FailRandMethodFixture::pseudorand,
                        &FailRandMethodFixture::status}
  {
    m_origRandMethod = RAND_get_rand_method();
    RAND_set_rand_method(&m_dummyRandMethod);
  }

  ~FailRandMethodFixture()
  {
    RAND_set_rand_method(m_origRandMethod);
  }

private: // RAND_METHOD callbacks
#if OPENSSL_VERSION_NUMBER < 0x1010000fL
  static void
  seed(const void* buf, int num)
  {
  }
#else
  static int
  seed(const void* buf, int num)
  {
    return 0;
  }
#endif // OPENSSL_VERSION_NUMBER < 0x1010000fL

  static int
  bytes(unsigned char* buf, int num)
  {
    return 0;
  }

  static void
  cleanup()
  {
  }

#if OPENSSL_VERSION_NUMBER < 0x1010000fL
  static void
  add(const void* buf, int num, double entropy)
  {
  }
#else
  static int
  add(const void* buf, int num, double entropy)
  {
    return 0;
  }
#endif // OPENSSL_VERSION_NUMBER < 0x1010000fL

  static int
  pseudorand(unsigned char* buf, int num)
  {
    return 0;
  }

  static int
  status()
  {
    return 0;
  }

private:
  const RAND_METHOD* m_origRandMethod;
  RAND_METHOD m_dummyRandMethod;
};

BOOST_FIXTURE_TEST_CASE(Error, FailRandMethodFixture)
{
  std::array<uint8_t, 1024> buf;
  BOOST_CHECK_THROW(random::generateSecureBytes(buf.data(), buf.size()), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END() // TestRandom
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace tests
} // namespace ndn
