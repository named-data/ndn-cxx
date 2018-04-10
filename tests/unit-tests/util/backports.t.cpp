/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
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

#include "util/backports.hpp"

#include "boost-test.hpp"
#include <numeric>

namespace ndn {
namespace tests {

BOOST_AUTO_TEST_SUITE(Util)
BOOST_AUTO_TEST_SUITE(TestBackports)

namespace deprecated_test {

// Deprecate a variable.
static int g_deprecatedVar NDN_CXX_DEPRECATED = 0;

// Deprecate an enum member (a variable).
enum NamedEnum
{
  ModernEnumMember,
};
constexpr NamedEnum DeprecatedEnumMember NDN_CXX_DEPRECATED = ModernEnumMember;

// Deprecate a type.
class ModernType
{
public:
  // Deprecate a function.
  NDN_CXX_DEPRECATED
  void
  deprecatedFunc(int a, NamedEnum b)
  {
  }
};
typedef ModernType DeprecatedType NDN_CXX_DEPRECATED;

} // namespace deprecated_test

BOOST_AUTO_TEST_CASE(Deprecated)
{
  using namespace deprecated_test;
  DeprecatedType instance;
  instance.deprecatedFunc(g_deprecatedVar, DeprecatedEnumMember);
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(MakeUnique)
{
  std::unique_ptr<int> v0 = make_unique<int>();
  std::unique_ptr<int> v1 = make_unique<int>(1728);
  BOOST_CHECK_EQUAL(*v0, 0);
  BOOST_CHECK_EQUAL(*v1, 1728);

  auto str = make_unique<std::string>("meow");
  BOOST_CHECK_EQUAL(*str, "meow");

  class Movable
  {
  public:
    Movable()
      : value(42)
    {
    }

    Movable(const Movable&) = delete;
    Movable(Movable&&) = default;

  public:
    int value;
  };

  class Container
  {
  public:
    explicit
    Container(Movable m)
      : m(std::move(m))
    {
    }

  public:
    Movable m;
  };

  Movable m;
  auto ptr = make_unique<Container>(std::move(m));
  BOOST_CHECK_EQUAL(ptr->m.value, 42);
}

BOOST_AUTO_TEST_CASE(Clamp)
{
  int x = clamp(5, 1, 10);
  BOOST_CHECK_EQUAL(x, 5);

  x = clamp(-5, 1, 10);
  BOOST_CHECK_EQUAL(x, 1);

  x = clamp(15, 1, 10);
  BOOST_CHECK_EQUAL(x, 10);

  x = clamp(5, 10, 1, std::greater<int>());
  BOOST_CHECK_EQUAL(x, 5);

  x = clamp(-5, 10, 1, std::greater<int>());
  BOOST_CHECK_EQUAL(x, 1);

  x = clamp(15, 10, 1, std::greater<int>());
  BOOST_CHECK_EQUAL(x, 10);
}

BOOST_AUTO_TEST_SUITE(Optional)

BOOST_AUTO_TEST_CASE(Construct)
{
  optional<int> o1;
  BOOST_CHECK_EQUAL(static_cast<bool>(o1), false);

  optional<int> o2(1);
  BOOST_CHECK_EQUAL(static_cast<bool>(o2), true);
  BOOST_CHECK_EQUAL(o2.value(), 1);

  optional<int> o3(o2);
  BOOST_CHECK_EQUAL(static_cast<bool>(o3), true);
  BOOST_CHECK_EQUAL(static_cast<bool>(o2), true);
  BOOST_CHECK_EQUAL(o3.value(), 1);

  optional<std::pair<int, int>> o4(in_place, 41, 42);
  BOOST_CHECK_EQUAL(static_cast<bool>(o4), true);
  BOOST_CHECK_EQUAL(o4.value().first, 41);
  BOOST_CHECK_EQUAL(o4.value().second, 42);
}

BOOST_AUTO_TEST_CASE(Assign)
{
  optional<int> o1;
  BOOST_CHECK_EQUAL(static_cast<bool>(o1), false);

  optional<int> o2(2);
  o1 = o2;
  BOOST_CHECK_EQUAL(static_cast<bool>(o1), true);
  BOOST_CHECK_EQUAL(o1.value(), 2);

  o1 = nullopt;
  BOOST_CHECK_EQUAL(static_cast<bool>(o1), false);

  o1 = 18763;
  BOOST_CHECK_EQUAL(static_cast<bool>(o1), true);
  BOOST_CHECK_EQUAL(o1.value(), 18763);
}

BOOST_AUTO_TEST_CASE(Access)
{
  optional<int> o1(1);
  BOOST_CHECK_EQUAL(*o1, 1);

  optional<std::string> o2("abc");
  BOOST_CHECK_EQUAL(o2->size(), 3);
}

BOOST_AUTO_TEST_CASE(HasValue)
{
  optional<int> o1;
  BOOST_CHECK_EQUAL(static_cast<bool>(o1), false);
  BOOST_CHECK_EQUAL(!o1, true);
  if (o1) {
    BOOST_ERROR("o1 should evaluate to false");
  }

  optional<int> o2(1);
  BOOST_CHECK_EQUAL(static_cast<bool>(o2), true);
  BOOST_CHECK_EQUAL(!o2, false);
  if (o2) {
  }
  else {
    BOOST_ERROR("o2 should evaluate to true");
  }
}

BOOST_AUTO_TEST_CASE(Value)
{
  optional<int> o1;
  BOOST_CHECK_THROW(o1.value(), bad_optional_access);

  optional<int> o2(2);
  BOOST_CHECK_NO_THROW(o2.value());
}

BOOST_AUTO_TEST_CASE(ValueOr)
{
  optional<int> o1;
  BOOST_CHECK_EQUAL(o1.value_or(3.0), 3);

  optional<int> o2(2);
  BOOST_CHECK_EQUAL(o2.value_or(3.0), 2);
}

BOOST_AUTO_TEST_CASE(Swap)
{
  optional<int> o1;
  optional<int> o2(2);

  o1.swap(o2);
  BOOST_CHECK_EQUAL(static_cast<bool>(o1), true);
  BOOST_CHECK_EQUAL(o1.value(), 2);
  BOOST_CHECK_EQUAL(static_cast<bool>(o2), false);
}

BOOST_AUTO_TEST_CASE(Reset)
{
  optional<int> o1(1);
  o1.reset();
  BOOST_CHECK_EQUAL(static_cast<bool>(o1), false);
}

BOOST_AUTO_TEST_CASE(Emplace)
{
  optional<std::pair<int, int>> o1;
  o1.emplace(11, 12);
  BOOST_CHECK_EQUAL(static_cast<bool>(o1), true);
  BOOST_CHECK_EQUAL(o1.value().first, 11);
  BOOST_CHECK_EQUAL(o1.value().second, 12);
}

BOOST_AUTO_TEST_CASE(Compare)
{
  optional<int> o0a;
  optional<int> o0b;
  optional<int> o1a(1);
  optional<int> o1b(1);
  optional<int> o2(2);

  BOOST_CHECK_EQUAL(o0a == o0b, true);
  BOOST_CHECK_EQUAL(o0a != o0b, false);
  BOOST_CHECK_EQUAL(o0a <  o0b, false);
  BOOST_CHECK_EQUAL(o0a <= o0b, true);
  BOOST_CHECK_EQUAL(o0a >  o0b, false);
  BOOST_CHECK_EQUAL(o0a >= o0b, true);

  BOOST_CHECK_EQUAL(o1a == o1b, true);
  BOOST_CHECK_EQUAL(o1a != o1b, false);
  BOOST_CHECK_EQUAL(o1a <  o1b, false);
  BOOST_CHECK_EQUAL(o1a <= o1b, true);
  BOOST_CHECK_EQUAL(o1a >  o1b, false);
  BOOST_CHECK_EQUAL(o1a >= o1b, true);

  BOOST_CHECK_EQUAL(o0a == o1a, false);
  BOOST_CHECK_EQUAL(o0a != o1a, true);
  BOOST_CHECK_EQUAL(o0a <  o1a, true);
  BOOST_CHECK_EQUAL(o0a <= o1a, true);
  BOOST_CHECK_EQUAL(o0a >  o1a, false);
  BOOST_CHECK_EQUAL(o0a >= o1a, false);

  BOOST_CHECK_EQUAL(o1a == o0a, false);
  BOOST_CHECK_EQUAL(o1a != o0a, true);
  BOOST_CHECK_EQUAL(o1a <  o0a, false);
  BOOST_CHECK_EQUAL(o1a <= o0a, false);
  BOOST_CHECK_EQUAL(o1a >  o0a, true);
  BOOST_CHECK_EQUAL(o1a >= o0a, true);

  BOOST_CHECK_EQUAL(o1a == o2, false);
  BOOST_CHECK_EQUAL(o1a != o2, true);
  BOOST_CHECK_EQUAL(o1a <  o2, true);
  BOOST_CHECK_EQUAL(o1a <= o2, true);
  BOOST_CHECK_EQUAL(o1a >  o2, false);
  BOOST_CHECK_EQUAL(o1a >= o2, false);

  BOOST_CHECK_EQUAL(o2 == o1a, false);
  BOOST_CHECK_EQUAL(o2 != o1a, true);
  BOOST_CHECK_EQUAL(o2 <  o1a, false);
  BOOST_CHECK_EQUAL(o2 <= o1a, false);
  BOOST_CHECK_EQUAL(o2 >  o1a, true);
  BOOST_CHECK_EQUAL(o2 >= o1a, true);
}

BOOST_AUTO_TEST_CASE(MakeOptional)
{
  auto o1 = make_optional(1);
  static_assert(std::is_same<decltype(o1), optional<int>>::value, "o1 must be optional<int>");
  BOOST_CHECK_EQUAL(static_cast<bool>(o1), true);
  BOOST_CHECK_EQUAL(o1.value(), 1);

  auto o2 = make_optional<std::pair<int, int>>(21, 22);
  static_assert(std::is_same<decltype(o2), optional<std::pair<int, int>>>::value,
                "o2 must be optional<std::pair<int, int>>");
  BOOST_CHECK_EQUAL(static_cast<bool>(o2), true);
  BOOST_CHECK_EQUAL(o2.value().first, 21);
  BOOST_CHECK_EQUAL(o2.value().second, 22);
}

BOOST_AUTO_TEST_SUITE_END() // Optional

BOOST_AUTO_TEST_CASE(OstreamJoiner)
{
  boost::test_tools::output_test_stream os;

  auto joiner1 = ostream_joiner<char>(os, ' ');
  auto joiner2 = make_ostream_joiner(os, ' ');
  static_assert(std::is_same<decltype(joiner1), decltype(joiner2)>::value, "");

  std::vector<int> v(5);
  std::iota(v.begin(), v.end(), 1);
  std::copy(v.begin(), v.end(), joiner2);
  BOOST_CHECK(os.is_equal("1 2 3 4 5"));

  auto joiner3 = make_ostream_joiner(os, "...");
  std::copy(v.begin(), v.end(), joiner3);
  BOOST_CHECK(os.is_equal("1...2...3...4...5"));

  joiner3 = "one";
  BOOST_CHECK(os.is_equal("one"));
  joiner3 = "two";
  BOOST_CHECK(os.is_equal("...two"));
  ++joiner3 = "three";
  BOOST_CHECK(os.is_equal("...three"));
  joiner3++ = "four";
  BOOST_CHECK(os.is_equal("...four"));

  std::copy(v.begin(), v.end(), make_ostream_joiner(os, ""));
  BOOST_CHECK(os.is_equal("12345"));

  std::string delimiter("_");
  std::copy(v.begin(), v.end(), make_ostream_joiner(os, delimiter));
  BOOST_CHECK(os.is_equal("1_2_3_4_5"));
}

BOOST_AUTO_TEST_SUITE_END() // TestBackports
BOOST_AUTO_TEST_SUITE_END() // Util

} // namespace tests
} // namespace ndn
