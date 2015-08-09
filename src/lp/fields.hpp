/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#ifndef NDN_CXX_LP_FIELDS_HPP
#define NDN_CXX_LP_FIELDS_HPP

#include "../common.hpp"

#include "tlv.hpp"
#include "detail/field-decl.hpp"
#include "field.hpp"

#include <boost/mpl/set.hpp>

namespace ndn {
namespace lp {

typedef detail::FieldDecl<field_location_tags::Header,
                          Sequence,
                          tlv::Sequence> SequenceField;
BOOST_CONCEPT_ASSERT((Field<SequenceField>));

typedef detail::FieldDecl<field_location_tags::Header,
                          uint64_t,
                          tlv::FragIndex> FragIndexField;
BOOST_CONCEPT_ASSERT((Field<FragIndexField>));

typedef detail::FieldDecl<field_location_tags::Header,
                          uint64_t,
                          tlv::FragCount> FragCountField;
BOOST_CONCEPT_ASSERT((Field<FragCountField>));

typedef detail::FieldDecl<field_location_tags::Header,
                          NackHeader,
                          tlv::Nack> NackField;
BOOST_CONCEPT_ASSERT((Field<NackField>));

typedef detail::FieldDecl<field_location_tags::Header,
                          uint64_t,
                          tlv::NextHopFaceId> NextHopFaceIdField;
BOOST_CONCEPT_ASSERT((Field<NextHopFaceIdField>));

typedef detail::FieldDecl<field_location_tags::Header,
                          CachePolicy,
                          tlv::CachePolicy> CachePolicyField;
BOOST_CONCEPT_ASSERT((Field<CachePolicyField>));

typedef detail::FieldDecl<field_location_tags::Header,
                          uint64_t,
                          tlv::IncomingFaceId> IncomingFaceIdField;
BOOST_CONCEPT_ASSERT((Field<IncomingFaceIdField>));

/**
 * The value of the wire encoded field is the data between the provided iterators. During
 * encoding, the data is copied from the Buffer into the wire buffer.
 */
typedef detail::FieldDecl<field_location_tags::Fragment,
                          std::pair<Buffer::const_iterator, Buffer::const_iterator>,
                          tlv::Fragment> FragmentField;
BOOST_CONCEPT_ASSERT((Field<FragmentField>));

/**
 * \brief set of all field declarations
 */
typedef boost::mpl::set<
  FragmentField,
  SequenceField,
  FragIndexField,
  FragCountField,
  NackField,
  NextHopFaceIdField,
  CachePolicyField,
  IncomingFaceIdField
  > FieldSet;

} // namespace lp
} // namespace ndn

#endif // NDN_CXX_LP_FIELDS_HPP