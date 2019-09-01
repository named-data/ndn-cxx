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

#ifndef NDN_UTIL_OVERLOAD_HPP
#define NDN_UTIL_OVERLOAD_HPP

#include <boost/predef/compiler/gcc.h>
#include <boost/version.hpp>

// Hana does not support GCC < 6.0.0
#if (BOOST_VERSION >= 106100) && (!BOOST_COMP_GNUC || (BOOST_COMP_GNUC >= BOOST_VERSION_NUMBER(6,0,0)))

#include <boost/hana/functional/overload.hpp>

namespace ndn {
constexpr boost::hana::make_overload_t overload{};
} // namespace ndn

#else

#include <type_traits>

namespace ndn {
namespace detail {

// The following code is copied from the Boost.Hana library.
// Copyright Louis Dionne 2013-2017
// Distributed under the Boost Software License, Version 1.0.
// (See http://boost.org/LICENSE_1_0.txt)

//! Pick one of several functions to call based on overload resolution.
//!
//! Specifically, `overload(f1, f2, ..., fn)` is a function object such
//! that
//! @code
//!     overload(f1, f2, ..., fn)(x...) == fk(x...)
//! @endcode
//!
//! where `fk` is the function of `f1, ..., fn` that would be called if
//! overload resolution was performed amongst that set of functions only.
//! If more than one function `fk` would be picked by overload resolution,
//! then the call is ambiguous.
#ifndef DOXYGEN
template <typename F, typename ...G>
struct overload_t
    : overload_t<F>::type
    , overload_t<G...>::type
{
  using type = overload_t;
  using overload_t<F>::type::operator();
  using overload_t<G...>::type::operator();

  template <typename F_, typename ...G_>
  constexpr explicit overload_t(F_&& f, G_&& ...g)
    : overload_t<F>::type(static_cast<F_&&>(f))
    , overload_t<G...>::type(static_cast<G_&&>(g)...)
  { }
};

template <typename F>
struct overload_t<F> { using type = F; };

template <typename R, typename ...Args>
struct overload_t<R(*)(Args...)> {
  using type = overload_t;
  R (*fptr_)(Args...);

  explicit constexpr overload_t(R (*fp)(Args...))
    : fptr_(fp)
  { }

  constexpr R operator()(Args ...args) const
  { return fptr_(static_cast<Args&&>(args)...); }
};

struct make_overload_t {
  template <typename ...F,
    typename Overload = typename overload_t<
      typename std::decay<F>::type...
    >::type
  >
  constexpr Overload operator()(F&& ...f) const {
    return Overload(static_cast<F&&>(f)...);
  }
};
#endif // DOXYGEN

} // namespace detail

constexpr detail::make_overload_t overload{};

} // namespace ndn

#endif // BOOST_VERSION >= 106100
#endif // NDN_UTIL_OVERLOAD_HPP
