/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 University of California, Los Angeles
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef MULTI_TYPE_CONTAINER_H_
#define MULTI_TYPE_CONTAINER_H_

#include <boost/mpl/inherit_linearly.hpp>
#include <boost/mpl/inherit.hpp>
#include <boost/mpl/at.hpp>

namespace ns3 {
namespace ndn {
namespace ndnSIM {
namespace detail {

template <class T>
struct wrap
{
  T value_;
};

template< class Vector >
struct multi_type_container
  : public boost::mpl::inherit_linearly< Vector, boost::mpl::inherit<wrap<boost::mpl::_2>, boost::mpl::_1 >
  >::type
{
  template<int N>
  struct index
  {
    typedef typename boost::mpl::at_c<Vector, N>::type type;
  };
  
  template<class T>
  T &
  get ()
  {
    return static_cast< wrap<T> &> (*this).value_;
  }

  template<class T>
  const T &
  get () const
  {
    return static_cast< const wrap<T> &> (*this).value_;
  }
  
  template<int N>
  typename boost::mpl::at_c<Vector, N>::type &
  get ()
  {
    typedef typename boost::mpl::at_c<Vector, N>::type T;
    return static_cast< wrap<T> &> (*this).value_;
  }

  template<int N>
  const typename boost::mpl::at_c<Vector, N>::type &
  get () const
  {
    typedef typename boost::mpl::at_c<Vector, N>::type T;
    return static_cast< const wrap<T> &> (*this).value_;
  }
};
  
} // detail
} // ndnSIM
} // ndn
} // ns3

#endif // MULTI_TYPE_CONTAINER_H_
