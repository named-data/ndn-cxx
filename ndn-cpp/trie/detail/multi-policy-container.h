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

#ifndef MULTI_POLICY_CONTAINER_H_
#define MULTI_POLICY_CONTAINER_H_

#include <boost/mpl/inherit_linearly.hpp>
#include <boost/mpl/at.hpp>

namespace ns3 {
namespace ndn {
namespace ndnSIM {
namespace detail {

template< class Base, class Value >
struct policy_wrap
{
  policy_wrap (Base &base) : value_ (base) { }
  Value value_;
};

template< class Base, class Super/*empy_wrap/previous level*/, class Value/*policy_wrap< element in vector >*/ >
struct inherit_with_base : Super, Value
{
  inherit_with_base (Base &base) : Super (base), Value (base) { }

  void
  update (typename Base::iterator item)
  {
    Value::value_.update (item);
    Super::update (item);
  }

  bool
  insert (typename Base::iterator item)
  {
    bool ok = Value::value_.insert (item);
    if (!ok)
      return false;

    ok = Super::insert (item);
    if (!ok)
      {
        Value::value_.erase (item);
        return false;
      }
    return true;
  }

  void
  lookup (typename Base::iterator item)
  {
    Value::value_.lookup (item);
    Super::lookup (item);
  }

  void
  erase (typename Base::iterator item)
  {
    Value::value_.erase (item);
    Super::erase (item);
  }  

  void
  clear ()
  {
    Value::value_.clear ();
    Super::clear ();
  }
};

template< class Base >
struct empty_policy_wrap
{
  empty_policy_wrap (Base &base) { }

  void update (typename Base::iterator item) {}
  bool insert (typename Base::iterator item) { return true; }
  void lookup (typename Base::iterator item) {}
  void erase (typename Base::iterator item) {}
  void clear () {}
};

template< class Base, class Vector >
struct multi_policy_container
  : public boost::mpl::fold< Vector,
                      empty_policy_wrap<Base>,
                      inherit_with_base<Base,
                                        boost::mpl::_1/*empty/previous*/,
                                        policy_wrap<Base, boost::mpl::_2>/*element in vector*/>
                      >::type
{
  typedef typename boost::mpl::fold< Vector,
                              empty_policy_wrap<Base>,
                              inherit_with_base<Base,
                                                boost::mpl::_1/*empty/previous*/,
                                                policy_wrap<Base, boost::mpl::_2>/*element in vector*/>
                              >::type super;

  typedef typename boost::mpl::at_c<Vector, 0>::type::iterator iterator;
  typedef typename boost::mpl::at_c<Vector, 0>::type::const_iterator const_iterator;

  iterator begin ()             { return this->get<0> ().begin (); }
  const_iterator begin () const { return this->get<0> ().begin (); }

  iterator end ()             { return this->get<0> ().end (); }
  const_iterator end () const { return this->get<0> ().end (); }

  size_t size () const { return this->get<0> ().size (); }
  
  multi_policy_container (Base &base)
  : super (base)
  { }

  template<int N>
  struct index
  {
    typedef typename boost::mpl::at_c<Vector, N>::type type;
  };
  
  template<class T>
  T &
  get ()
  {
    return static_cast< policy_wrap<Base, T> &> (*this).value_;
  }

  template<class T>
  const T &
  get () const
  {
    return static_cast< const policy_wrap<Base, T> &> (*this).value_;
  }

  template<int N>
  typename boost::mpl::at_c<Vector, N>::type &
  get ()
  {
    typedef typename boost::mpl::at_c<Vector, N>::type T;
    return static_cast< policy_wrap<Base, T> &> (*this).value_;
  }

  template<int N>
  const typename boost::mpl::at_c<Vector, N>::type &
  get () const
  {
    typedef typename boost::mpl::at_c<Vector, N>::type T;
    return static_cast< const policy_wrap<Base, T> &> (*this).value_;
  }
};


} // detail
} // ndnSIM
} // ndn
} // ns3

#endif // MULTI_POLICY_CONTAINER_H_
