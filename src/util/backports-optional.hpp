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

/** \file
 *  \brief C++17 std::optional backport implemented using boost::optional
 *  \sa http://open-std.org/JTC1/SC22/WG21/docs/papers/2016/n4594.pdf section 20.6
 *  \sa http://en.cppreference.com/w/cpp/utility/optional
 *
 *  Differences from C++17 include:
 *  \li No constructor and operator= taking a T&&,
 *      because boost::optional lacks a move constructor as of Boost 1.54
 *  \li No constructor, operator=, emplace, and make_optional with std::initializer_list
 *  \li In-place constructor and emplace require copyable arguments,
 *      because boost::in_place requires such
 *  \li Move constructor may or may not exist (it's implicitly defined when available),
 *      because boost::optional lacks a move constructor as of Boost 1.54
 *  \li Non-const operator-> and operator* are not constexpr
 *  \li value() is not constexpr
 *  \li swap is declared without noexcept specification
 *  \li No comparison operators with const T& or nullopt_t
 *  \li No specialized std::hash support
 */

#ifndef NDN_UTIL_BACKPORTS_OPTIONAL_HPP
#define NDN_UTIL_BACKPORTS_OPTIONAL_HPP

#include "backports.hpp"

#if (__cplusplus > 201402L) && NDN_CXX_HAS_INCLUDE(<optional>)
#  include <optional>
#  define NDN_CXX_HAVE_STD_OPTIONAL
#elif (__cplusplus > 201103L) && NDN_CXX_HAS_INCLUDE(<experimental/optional>)
#  include <experimental/optional>
#  if __cpp_lib_experimental_optional >= 201411
#    define NDN_CXX_HAVE_EXPERIMENTAL_OPTIONAL
#  endif
#endif

#if defined(NDN_CXX_HAVE_STD_OPTIONAL)

namespace ndn {
using std::optional;
using std::in_place;
using std::nullopt;
using std::bad_optional_access;
using std::make_optional;
} // namespace ndn

#elif defined(NDN_CXX_HAVE_EXPERIMENTAL_OPTIONAL)

namespace ndn {
using std::experimental::optional;
using std::experimental::in_place;
using std::experimental::nullopt;
using std::experimental::bad_optional_access;
using std::experimental::make_optional;

template<typename T, typename... Args>
constexpr optional<T>
make_optional(Args&&... args)
{
  return optional<T>(in_place, std::forward<Args>(args)...);
}
} // namespace ndn

#else

#include <boost/none.hpp>
#include <boost/optional.hpp>
#include <boost/utility/typed_in_place_factory.hpp>

namespace ndn {

template<typename T>
class optional;

struct in_place_t
{
};
constexpr in_place_t in_place{};

class nullopt_t
{
public:
  constexpr explicit
  nullopt_t(int)
  {
  }
};
constexpr nullopt_t nullopt{0};

#if BOOST_VERSION >= 105600
using boost::bad_optional_access;
#else
class bad_optional_access : public std::logic_error
{
public:
  bad_optional_access()
    : std::logic_error("bad optional access")
  {
  }
};
#endif

template<typename T>
constexpr bool
operator==(const optional<T>& lhs, const optional<T>& rhs);

template<typename T>
constexpr bool
operator!=(const optional<T>& lhs, const optional<T>& rhs);

template<typename T>
constexpr bool
operator<(const optional<T>& lhs, const optional<T>& rhs);

template<typename T>
constexpr bool
operator<=(const optional<T>& lhs, const optional<T>& rhs);

template<typename T>
constexpr bool
operator>(const optional<T>& lhs, const optional<T>& rhs);

template<typename T>
constexpr bool
operator>=(const optional<T>& lhs, const optional<T>& rhs);

template<typename T>
class optional
{
public:
  static_assert(!std::is_same<typename std::remove_cv<T>::type, in_place_t>::value &&
                !std::is_same<typename std::remove_cv<T>::type, nullopt_t>::value &&
                !std::is_reference<T>::value,
                "Invalid instantiation of optional<T>");

  typedef T value_type;

  constexpr
  optional() noexcept
  {
  }

  constexpr
  optional(nullopt_t) noexcept
  {
  }

  constexpr
  optional(const T& value)
    : m_boostOptional(value)
  {
  }

  template<typename... Args>
  constexpr explicit
  optional(in_place_t, Args&&... args)
    : m_boostOptional(boost::in_place<T>(std::forward<Args>(args)...))
  {
  }

  optional&
  operator=(nullopt_t) noexcept
  {
    m_boostOptional = boost::none;
    return *this;
  }

  optional&
  operator=(const optional& other)
  {
    m_boostOptional = other.m_boostOptional;
    return *this;
  }

  template<typename U,
           typename = typename std::enable_if<std::is_same<typename std::decay<U>::type, T>::value>::type>
  optional&
  operator=(U&& value)
  {
    m_boostOptional = std::forward<U>(value);
    return *this;
  }

public: // observers
  constexpr const T*
  operator->() const
  {
    return m_boostOptional.get_ptr();
  }

  T*
  operator->()
  {
    return m_boostOptional.get_ptr();
  }

  constexpr const T&
  operator*() const
  {
    return m_boostOptional.get();
  }

  T&
  operator*()
  {
    return m_boostOptional.get();
  }

  constexpr explicit
  operator bool() const noexcept
  {
    return static_cast<bool>(m_boostOptional);
  }

  const T&
  value() const
  {
    return const_cast<optional*>(this)->value();
  }

  T&
  value()
  {
#if BOOST_VERSION >= 105600
    return m_boostOptional.value();
#else
    if (!m_boostOptional) {
      BOOST_THROW_EXCEPTION(bad_optional_access());
    }
    return m_boostOptional.get();
#endif
  }

  template<typename U>
  constexpr T
  value_or(U&& default_value) const
  {
#if BOOST_VERSION >= 105600
    return m_boostOptional.value_or(default_value);
#else
    return m_boostOptional.get_value_or(default_value);
#endif
  }

public: // modifiers
  void
  swap(optional& other)
  {
    boost::swap(m_boostOptional, other.m_boostOptional);
  }

  void
  reset() noexcept
  {
    m_boostOptional = boost::none;
  }

  template<typename... Args>
  void
  emplace(Args&&... args)
  {
    m_boostOptional = boost::in_place<T>(std::forward<Args>(args)...);
  }

private:
  boost::optional<T> m_boostOptional;

  friend bool operator==<T>(const optional<T>&, const optional<T>&);
  friend bool operator!=<T>(const optional<T>&, const optional<T>&);
  friend bool operator< <T>(const optional<T>&, const optional<T>&);
  friend bool operator<=<T>(const optional<T>&, const optional<T>&);
  friend bool operator> <T>(const optional<T>&, const optional<T>&);
  friend bool operator>=<T>(const optional<T>&, const optional<T>&);
};

template<typename T>
constexpr bool
operator==(const optional<T>& lhs, const optional<T>& rhs)
{
  return operator==(lhs.m_boostOptional, rhs.m_boostOptional);
}

template<typename T>
constexpr bool
operator!=(const optional<T>& lhs, const optional<T>& rhs)
{
  return operator!=(lhs.m_boostOptional, rhs.m_boostOptional);
}

template<typename T>
constexpr bool
operator<(const optional<T>& lhs, const optional<T>& rhs)
{
  return operator<(lhs.m_boostOptional, rhs.m_boostOptional);
}

template<typename T>
constexpr bool
operator<=(const optional<T>& lhs, const optional<T>& rhs)
{
  return operator<=(lhs.m_boostOptional, rhs.m_boostOptional);
}

template<typename T>
constexpr bool
operator>(const optional<T>& lhs, const optional<T>& rhs)
{
  return operator>(lhs.m_boostOptional, rhs.m_boostOptional);
}

template<typename T>
constexpr bool
operator>=(const optional<T>& lhs, const optional<T>& rhs)
{
  return operator>=(lhs.m_boostOptional, rhs.m_boostOptional);
}

template<typename T>
constexpr optional<typename std::decay<T>::type>
make_optional(T&& value)
{
  return optional<typename std::decay<T>::type>(std::forward<T>(value));
}

template<typename T, typename... Args>
constexpr optional<T>
make_optional(Args&&... args)
{
  return optional<T>(in_place, std::forward<Args>(args)...);
}

} // namespace ndn

#endif
#endif // NDN_UTIL_BACKPORTS_OPTIONAL_HPP
