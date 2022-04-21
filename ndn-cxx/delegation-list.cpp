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

#include "ndn-cxx/delegation-list.hpp"
#include "ndn-cxx/util/ostream-joiner.hpp"

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<DelegationList>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<DelegationList>));
BOOST_CONCEPT_ASSERT((WireDecodable<DelegationList>));

DelegationList::DelegationList()
  : m_isSorted(true)
{
}

DelegationList::DelegationList(std::initializer_list<Delegation> dels)
  : m_isSorted(true)
{
  for (const Delegation& del : dels) {
    this->insert(del, INS_REPLACE);
  }
}

DelegationList::DelegationList(const Block& block, bool wantSort)
{
  this->wireDecode(block, wantSort);
}

bool
DelegationList::isValidTlvType(uint32_t type)
{
  switch (type) {
    case tlv::Content:
    case tlv::ForwardingHint:
      return true;
    default:
      return false;
  }
}

template<encoding::Tag TAG>
size_t
DelegationList::wireEncode(EncodingImpl<TAG>& encoder, uint32_t type) const
{
  if (!isValidTlvType(type)) {
    NDN_THROW(std::invalid_argument("Unexpected TLV-TYPE " + to_string(type) +
                                    " while encoding DelegationList"));
  }

  if (this->size() == 0) {
    NDN_THROW(Error("Empty DelegationList"));
  }

  // LinkContent ::= (type) TLV-LENGTH
  //                    Delegation+

  // Delegation ::= LINK-DELEGATION-TYPE TLV-LENGTH
  //                  Preference
  //                  Name

  // Preference ::= LINK-PREFERENCE-TYPE TLV-LENGTH
  //       nonNegativeInteger

  size_t totalLen = 0;
  for (auto i = m_dels.rbegin(); i != m_dels.rend(); ++i) {
    size_t delLen = 0;
    delLen += i->name.wireEncode(encoder);
    delLen += prependNonNegativeIntegerBlock(encoder, tlv::LinkPreference, i->preference);
    delLen += encoder.prependVarNumber(delLen);
    delLen += encoder.prependVarNumber(tlv::LinkDelegation);
    totalLen += delLen;
  }
  totalLen += encoder.prependVarNumber(totalLen);
  totalLen += encoder.prependVarNumber(type);
  return totalLen;
}

template size_t
DelegationList::wireEncode<encoding::EncoderTag>(EncodingBuffer&, uint32_t) const;

template size_t
DelegationList::wireEncode<encoding::EstimatorTag>(EncodingEstimator&, uint32_t) const;

void
DelegationList::wireDecode(const Block& block, bool wantSort)
{
  if (!isValidTlvType(block.type())) {
    NDN_THROW(Error("Unexpected TLV-TYPE " + to_string(block.type()) + " while decoding DelegationList"));
  }

  m_isSorted = wantSort;
  m_dels.clear();

  block.parse();
  for (const auto& del : block.elements()) {
    if (del.type() != tlv::LinkDelegation) {
      NDN_THROW(Error("Unexpected TLV-TYPE " + to_string(del.type()) + " while decoding Delegation"));
    }
    del.parse();

    auto val = del.elements_begin();
    if (val == del.elements_end() || val->type() != tlv::LinkPreference) {
      NDN_THROW(Error("Missing Preference field in Delegation"));
    }
    uint64_t preference = 0;
    try {
      preference = readNonNegativeInteger(*val);
    }
    catch (const tlv::Error&) {
      NDN_THROW_NESTED(Error("Invalid Preference field in Delegation"));
    }

    ++val;
    if (val == del.elements_end() || val->type() != tlv::Name) {
      NDN_THROW(Error("Missing Name field in Delegation"));
    }
    Name name;
    try {
      name.wireDecode(*val);
    }
    catch (const tlv::Error&) {
      NDN_THROW_NESTED(Error("Invalid Name field in Delegation"));
    }

    this->insertImpl(preference, name);
  }

  if (this->size() == 0) {
    NDN_THROW(Error("Empty DelegationList"));
  }
}

void
DelegationList::sort()
{
  if (m_isSorted) {
    return;
  }

  std::vector<Delegation> dels;
  dels.swap(m_dels);

  m_isSorted = true;
  for (const Delegation& del : dels) {
    this->insertImpl(del.preference, del.name);
  }
}

bool
DelegationList::insert(uint64_t preference, const Name& name,
                       InsertConflictResolution onConflict)
{
  switch (onConflict) {
    case INS_REPLACE:
      this->eraseImpl(nullopt, name);
      this->insertImpl(preference, name);
      return true;
    case INS_APPEND:
      this->insertImpl(preference, name);
      return true;
    case INS_SKIP:
      if (!std::any_of(m_dels.begin(), m_dels.end(),
                       [name] (const Delegation& del) { return del.name == name; })) {
        this->insertImpl(preference, name);
        return true;
      }
      return false;
  }

  NDN_THROW(std::invalid_argument("Unknown InsertConflictResolution"));
}

void
DelegationList::insertImpl(uint64_t preference, const Name& name)
{
  if (!m_isSorted) {
    m_dels.push_back({preference, name});
    return;
  }

  Delegation del{preference, name};
  auto pos = std::upper_bound(m_dels.begin(), m_dels.end(), del);
  m_dels.insert(pos, del);
}

size_t
DelegationList::eraseImpl(optional<uint64_t> preference, const Name& name)
{
  size_t nErased = 0;
  for (auto i = m_dels.begin(); i != m_dels.end();) {
    if ((!preference || i->preference == *preference) &&
        i->name == name) {
      ++nErased;
      i = m_dels.erase(i);
    }
    else {
      ++i;
    }
  }
  return nErased;
}

std::ostream&
operator<<(std::ostream& os, const DelegationList& dl)
{
  os << '[';
  std::copy(dl.begin(), dl.end(), make_ostream_joiner(os, ','));
  return os << ']';
}

} // namespace ndn
