/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 University of California, Los Angeles
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

#ifndef FUNCTOR_HOOK_H_
#define FUNCTOR_HOOK_H_

#include <boost/intrusive/parent_from_member.hpp>

namespace ns3 {
namespace ndn {
namespace ndnSIM {
namespace detail {

template<class BaseHook, class ValueType, int N>
struct FunctorHook
{
  typedef typename BaseHook::template index<N>::type hook_type;
  typedef hook_type*            hook_ptr;
  typedef const hook_type*      const_hook_ptr;
  
  typedef ValueType             value_type;
  typedef value_type*           pointer;
  typedef const value_type*     const_pointer;
  
  //Required static functions
  static hook_ptr to_hook_ptr (value_type &value)
  {  return &value.policy_hook_.template get<N> (); }
  
  static const_hook_ptr to_hook_ptr(const value_type &value)
  {  return &value.policy_hook_.template get<N> (); }
  
  static pointer to_value_ptr(hook_ptr n)
  {
    return
      boost::intrusive::get_parent_from_member<value_type>
      (static_cast<BaseHook*> (boost::intrusive::get_parent_from_member< wrap<hook_type> >(n, &wrap<hook_type>::value_)),
       &value_type::policy_hook_);
  }
  static const_pointer to_value_ptr(const_hook_ptr n)
  {
    return
      boost::intrusive::get_parent_from_member<value_type>
      (static_cast<const BaseHook*> (boost::intrusive::get_parent_from_member< wrap<hook_type> >(n, &wrap<hook_type>::value_)),
       &value_type::policy_hook_);
  }
};

} // detail
} // ndnSIM
} // ndn
} // ns3

#endif // FUNCTOR_HOOK_H_
