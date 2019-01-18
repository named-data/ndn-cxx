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
#include <thread>

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(Util)
BOOST_AUTO_TEST_SUITE(TestRandom)

BOOST_AUTO_TEST_CASE(ThreadLocalEngine)
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
