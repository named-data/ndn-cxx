/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
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
 *  \brief Backport of ostream_joiner from the Library Fundamentals v2 TS
 *  \sa http://en.cppreference.com/w/cpp/experimental/ostream_joiner
 */

#ifndef NDN_UTIL_BACKPORTS_OSTREAM_JOINER_HPP
#define NDN_UTIL_BACKPORTS_OSTREAM_JOINER_HPP

#include "backports.hpp"

#if (__cplusplus >= 201402L) && NDN_CXX_HAS_INCLUDE(<experimental/iterator>)
#  include <experimental/iterator>
#  if __cpp_lib_experimental_ostream_joiner >= 201411
#    define NDN_CXX_HAVE_EXPERIMENTAL_OSTREAM_JOINER
#  endif
#endif

#ifdef NDN_CXX_HAVE_EXPERIMENTAL_OSTREAM_JOINER

namespace ndn {
using std::experimental::ostream_joiner;
using std::experimental::make_ostream_joiner;
} // namespace ndn

#else

#include <iterator>

namespace ndn {

template<typename DelimT,
         typename CharT = char,
         typename Traits = std::char_traits<CharT>>
class ostream_joiner
{
public:
  typedef CharT char_type;
  typedef Traits traits_type;
  typedef std::basic_ostream<CharT, Traits> ostream_type;
  typedef std::output_iterator_tag iterator_category;
  typedef void value_type;
  typedef void difference_type;
  typedef void pointer;
  typedef void reference;

  ostream_joiner(ostream_type& os, const DelimT& delimiter)
  noexcept(std::is_nothrow_copy_constructible<DelimT>::value)
    : m_os(std::addressof(os)), m_delim(delimiter)
  {
  }

  ostream_joiner(ostream_type& os, DelimT&& delimiter)
  noexcept(std::is_nothrow_move_constructible<DelimT>::value)
    : m_os(std::addressof(os)), m_delim(std::move(delimiter))
  {
  }

  template<typename T>
  ostream_joiner&
  operator=(const T& value)
  {
    if (!m_isFirst) {
      *m_os << m_delim;
    }
    m_isFirst = false;
    *m_os << value;
    return *this;
  }

  ostream_joiner&
  operator*() noexcept
  {
    return *this;
  }

  ostream_joiner&
  operator++() noexcept
  {
    return *this;
  }

  ostream_joiner&
  operator++(int) noexcept
  {
    return *this;
  }

private:
  ostream_type* m_os;
  DelimT m_delim;
  bool m_isFirst = true;
};

template<typename CharT, typename Traits, typename DelimT>
inline ostream_joiner<typename std::decay<DelimT>::type, CharT, Traits>
make_ostream_joiner(std::basic_ostream<CharT, Traits>& os, DelimT&& delimiter)
{
  return {os, std::forward<DelimT>(delimiter)};
}

} // namespace ndn

#endif // NDN_CXX_HAVE_EXPERIMENTAL_OSTREAM_JOINER
#endif // NDN_UTIL_BACKPORTS_OSTREAM_JOINER_HPP
