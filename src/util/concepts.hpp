/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014-2017 Regents of the University of California,
 *                         Arizona Board of Regents,
 *                         Colorado State University,
 *                         University Pierre & Marie Curie, Sorbonne University,
 *                         Washington University in St. Louis,
 *                         Beijing Institute of Technology,
 *                         The University of Memphis.
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

#ifndef NDN_UTIL_CONCEPTS_HPP
#define NDN_UTIL_CONCEPTS_HPP

#include "../encoding/block.hpp"
#include "../encoding/encoding-buffer.hpp"

#include <boost/concept/usage.hpp>
#include <boost/type_traits/has_equal_to.hpp>
#include <boost/type_traits/has_not_equal_to.hpp>
#include <boost/type_traits/has_left_shift.hpp>

namespace ndn {

/** \brief a concept check for TLV abstraction with .wireEncode method
 */
template<class X>
class WireEncodable
{
public:
  BOOST_CONCEPT_USAGE(WireEncodable)
  {
    Block block = j.wireEncode();
    block.size(); // avoid 'unused variable block'
  }

private:
  X j;
};

/** \brief a concept check for TLV abstraction with .wireEncode method
 */
template<class X>
class WireEncodableWithEncodingBuffer
{
public:
  BOOST_CONCEPT_USAGE(WireEncodableWithEncodingBuffer)
  {
    EncodingEstimator estimator;
    size_t estimatedSize = j.wireEncode(estimator);

    EncodingBuffer encoder(estimatedSize, 0);
    j.wireEncode(encoder);
  }

private:
  X j;
};

/** \brief a concept check for TLV abstraction with .wireDecode method
 *         and constructible from Block
 */
template<class X>
class WireDecodable
{
public:
  BOOST_CONCEPT_USAGE(WireDecodable)
  {
    Block block;
    X j(block);
    j.wireDecode(block);
  }
};

namespace detail {

template<class X>
class NfdMgmtProtocolStruct : public WireEncodable<X>
                            , public WireEncodableWithEncodingBuffer<X>
                            , public WireDecodable<X>
{
public:
  BOOST_CONCEPT_USAGE(NfdMgmtProtocolStruct)
  {
    static_assert(std::is_default_constructible<X>::value, "");
    static_assert(boost::has_equal_to<X, X, bool>::value, "");
    static_assert(boost::has_not_equal_to<X, X, bool>::value, "");
    static_assert(boost::has_left_shift<std::ostream, X, std::ostream&>::value, "");
    static_assert(std::is_base_of<tlv::Error, typename X::Error>::value, "");
  }
};

} // namespace detail

/** \brief concept check for an item in a Status Dataset
 *  \sa https://redmine.named-data.net/projects/nfd/wiki/StatusDataset
 */
template<class X>
class StatusDatasetItem : public detail::NfdMgmtProtocolStruct<X>
{
};

/** \brief concept check for an item in a Notification Stream
 *  \sa https://redmine.named-data.net/projects/nfd/wiki/Notification
 */
template<class X>
class NotificationStreamItem : public detail::NfdMgmtProtocolStruct<X>
{
};

// NDN_CXX_ASSERT_DEFAULT_CONSTRUCTIBLE and NDN_CXX_ASSERT_FORWARD_ITERATOR
// originally written as part of NFD codebase

namespace detail {

// As of Boost 1.61.0, the internal implementation of BOOST_CONCEPT_ASSERT does not allow
// multiple assertions on the same line, so we have to combine multiple concepts together.

template<typename T>
class StlForwardIteratorConcept : public boost::ForwardIterator<T>
                                , public boost::DefaultConstructible<T>
{
};

} // namespace detail

// std::is_default_constructible is broken in gcc-4.8, see bug #3882
/** \brief assert T is default constructible
 *  \sa http://en.cppreference.com/w/cpp/concept/DefaultConstructible
 */
#define NDN_CXX_ASSERT_DEFAULT_CONSTRUCTIBLE(T) \
  static_assert(std::is_default_constructible<T>::value, \
                #T " must be default-constructible"); \
  BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<T>))

/** \brief assert T is a forward iterator
 *  \sa http://en.cppreference.com/w/cpp/concept/ForwardIterator
 *  \note A forward iterator should be default constructible, but boost::ForwardIterator follows
 *        SGI standard which doesn't require DefaultConstructible, so a separate check is needed.
 */
#define NDN_CXX_ASSERT_FORWARD_ITERATOR(T) \
  static_assert(std::is_default_constructible<T>::value, \
                #T " must be default-constructible"); \
  BOOST_CONCEPT_ASSERT((::ndn::detail::StlForwardIteratorConcept<T>))

} // namespace ndn

#endif // NDN_UTIL_CONCEPTS_HPP
