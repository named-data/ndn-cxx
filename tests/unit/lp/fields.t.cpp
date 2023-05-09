/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2023 Regents of the University of California.
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

#include "ndn-cxx/lp/fields.hpp"

namespace ndn::tests {

using namespace ndn::lp;

BOOST_CONCEPT_ASSERT((Field<FragmentField>));
BOOST_CONCEPT_ASSERT((Field<SequenceField>));
BOOST_CONCEPT_ASSERT((Field<FragIndexField>));
BOOST_CONCEPT_ASSERT((Field<FragCountField>));
BOOST_CONCEPT_ASSERT((Field<PitTokenField>));
BOOST_CONCEPT_ASSERT((Field<NackField>));
BOOST_CONCEPT_ASSERT((Field<IncomingFaceIdField>));
BOOST_CONCEPT_ASSERT((Field<NextHopFaceIdField>));
BOOST_CONCEPT_ASSERT((Field<CachePolicyField>));
BOOST_CONCEPT_ASSERT((Field<CongestionMarkField>));
BOOST_CONCEPT_ASSERT((Field<AckField>));
BOOST_CONCEPT_ASSERT((Field<TxSequenceField>));
BOOST_CONCEPT_ASSERT((Field<NonDiscoveryField>));
BOOST_CONCEPT_ASSERT((Field<PrefixAnnouncementField>));

} // namespace ndn::tests
