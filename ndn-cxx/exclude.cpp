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
 *
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 */

#include "exclude.hpp"
#include "encoding/block-helpers.hpp"

#include <boost/range/adaptor/reversed.hpp>
#include <sstream>

namespace ndn {

Exclude::ExcludeComponent::ExcludeComponent(const name::Component& component1)
  : isNegInf(false)
  , component(component1)
{
}

Exclude::ExcludeComponent::ExcludeComponent(bool isNegInf1)
  : isNegInf(true)
{
  BOOST_ASSERT(isNegInf1 == true);
}

bool
operator==(const Exclude::ExcludeComponent& a, const Exclude::ExcludeComponent& b)
{
  return (a.isNegInf && b.isNegInf) ||
         (a.isNegInf == b.isNegInf && a.component == b.component);
}

bool
operator>(const Exclude::ExcludeComponent& a, const Exclude::ExcludeComponent& b)
{
  return a.isNegInf < b.isNegInf ||
         (a.isNegInf == b.isNegInf && a.component > b.component);
}

Exclude::Range::Range()
  : fromInfinity(false)
  , toInfinity(false)
{
}

Exclude::Range::Range(bool fromInfinity, const name::Component& from, bool toInfinity, const name::Component& to)
  : fromInfinity(fromInfinity)
  , from(from)
  , toInfinity(toInfinity)
  , to(to)
{
}

bool
Exclude::Range::operator==(const Exclude::Range& other) const
{
  return this->fromInfinity == other.fromInfinity && this->toInfinity == other.toInfinity &&
         (this->fromInfinity || this->from == other.from) &&
         (this->toInfinity || this->to == other.to);
}

std::ostream&
operator<<(std::ostream& os, const Exclude::Range& range)
{
  if (range.isSingular()) {
    return os << '{' << range.from << '}';
  }

  if (range.fromInfinity) {
    os << "(-∞";
  }
  else {
    os << '[' << range.from;
  }

  os << ",";

  if (range.toInfinity) {
    os << "+∞)";
  }
  else {
    os << range.to << ']';
  }

  return os;
}

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Exclude>));
BOOST_CONCEPT_ASSERT((WireEncodable<Exclude>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<Exclude>));
BOOST_CONCEPT_ASSERT((WireDecodable<Exclude>));
static_assert(std::is_base_of<tlv::Error, Exclude::Error>::value,
              "Exclude::Error must inherit from tlv::Error");

Exclude::Exclude() = default;

Exclude::Exclude(const Block& wire)
{
  wireDecode(wire);
}

template<encoding::Tag TAG>
size_t
Exclude::wireEncode(EncodingImpl<TAG>& encoder) const
{
  if (m_entries.empty()) {
    BOOST_THROW_EXCEPTION(Error("cannot encode empty Exclude selector"));
  }

  size_t totalLength = 0;

  // Exclude ::= EXCLUDE-TYPE TLV-LENGTH Any? (GenericNameComponent (Any)?)+
  // Any     ::= ANY-TYPE TLV-LENGTH(=0)

  for (const Entry& entry : m_entries) {
    if (entry.second) {
      totalLength += prependEmptyBlock(encoder, tlv::Any);
    }
    if (!entry.first.isNegInf) {
      totalLength += entry.first.component.wireEncode(encoder);
    }
  }

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::Exclude);
  return totalLength;
}

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(Exclude);

const Block&
Exclude::wireEncode() const
{
  if (m_wire.hasWire())
    return m_wire;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  m_wire = buffer.block();
  return m_wire;
}

void
Exclude::wireDecode(const Block& wire)
{
  clear();

  if (wire.type() != tlv::Exclude)
    BOOST_THROW_EXCEPTION(tlv::Error("Unexpected TLV type when decoding Exclude"));

  m_wire = wire;
  m_wire.parse();

  if (m_wire.elements_size() == 0) {
    BOOST_THROW_EXCEPTION(Error("Exclude element cannot be empty"));
  }

  // Exclude ::= EXCLUDE-TYPE TLV-LENGTH Any? (GenericNameComponent (Any)?)+
  // Any     ::= ANY-TYPE TLV-LENGTH(=0)

  Block::element_const_iterator i = m_wire.elements_begin();
  if (i->type() == tlv::Any) {
    this->appendEntry(true, true);
    ++i;
  }

  while (i != m_wire.elements_end()) {
    name::Component component;
    try {
      component = name::Component(*i);
    }
    catch (const name::Component::Error&) {
      BOOST_THROW_EXCEPTION(Error("Incorrect format of Exclude filter"));
    }
    if (!component.isGeneric() && !component.isImplicitSha256Digest()) {
      BOOST_THROW_EXCEPTION(Error("Excluded component must be generic or ImplicitSha256Digest"));
    }
    ++i;

    if (i != m_wire.elements_end() && i->type() == tlv::Any) {
      this->appendEntry(component, true);
      ++i;
    }
    else {
      this->appendEntry(component, false);
    }
  }
}

template<typename T>
void
Exclude::appendEntry(const T& component, bool hasAny)
{
  m_entries.emplace_hint(m_entries.begin(), std::piecewise_construct,
                         std::forward_as_tuple(component),
                         std::forward_as_tuple(hasAny));
}

// example: ANY "b" "d" ANY "f"
// ordered in map as: "f" (false); "d" (true); "b" (false); -Inf (true)
//
// lower_bound("")  -> -Inf (true) <-- excluded (ANY)
// lower_bound("a") -> -Inf (true) <-- excluded (ANY)
// lower_bound("b") -> "b" (false) <--- excluded (equal)
// lower_bound("c") -> "b" (false) <--- not excluded (not equal and no ANY)
// lower_bound("d") -> "d" (true) <- excluded
// lower_bound("e") -> "d" (true) <- excluded
bool
Exclude::isExcluded(const name::Component& comp) const
{
  ExcludeMap::const_iterator lb = m_entries.lower_bound(comp);
  return lb != m_entries.end() && // if false, comp is less than the first excluded component
         (lb->second || // comp matches an ANY range
          (!lb->first.isNegInf && lb->first.component == comp)); // comp equals an exact excluded component
}

Exclude&
Exclude::excludeOne(const name::Component& comp)
{
  if (!isExcluded(comp)) {
    this->appendEntry(comp, false);
    m_wire.reset();
  }
  return *this;
}

Exclude&
Exclude::excludeBefore(const name::Component& to)
{
  return excludeRange(ExcludeComponent(true), to);
}

Exclude&
Exclude::excludeRange(const name::Component& from, const name::Component& to)
{
  return excludeRange(ExcludeComponent(from), to);
}

// example: ANY "b" "d" ANY "g"
// ordered in map as: "f" (false); "d" (true); "b" (false); -Inf (true)
// possible sequence of operations:
// excludeBefore("a") -> excludeRange(-Inf, "a") ->  ANY "a"
//                          "a" (false); -Inf (true)
// excludeBefore("b") -> excludeRange(-Inf, "b") ->  ANY "b"
//                          "b" (false); -Inf (true)
// excludeRange("e", "g") ->  ANY "b" "e" ANY "g"
//                          "g" (false); "e" (true); "b" (false); -Inf (true)
// excludeRange("d", "f") ->  ANY "b" "d" ANY "g"
//                          "g" (false); "d" (true); "b" (false); -Inf (true)

Exclude&
Exclude::excludeRange(const ExcludeComponent& from, const name::Component& to)
{
  if (!from.isNegInf && from.component >= to) {
    BOOST_THROW_EXCEPTION(Error("Invalid exclude range [" + from.component.toUri() + ", " + to.toUri() + "] "
                                "(for single name exclude use Exclude::excludeOne)"));
  }

  ExcludeMap::iterator newFrom = m_entries.lower_bound(from);
  if (newFrom == m_entries.end() || !newFrom->second /*without ANY*/) {
    bool isNewEntry = false;
    std::tie(newFrom, isNewEntry) = m_entries.emplace(from, true);
    if (!isNewEntry) {
      // this means that the lower bound is equal to the item itself. So, just update ANY flag
      newFrom->second = true;
    }
  }
  // else
  // nothing special if start of the range already exists with ANY flag set

  ExcludeMap::iterator newTo = m_entries.lower_bound(to);
  BOOST_ASSERT(newTo != m_entries.end());
  if (newTo == newFrom || !newTo->second) {
    newTo = m_entries.emplace_hint(newTo, to, false);
    ++newTo;
  }
  // else
  // nothing to do really

  // remove any intermediate entries, since all of the are excluded
  m_entries.erase(newTo, newFrom);

  m_wire.reset();
  return *this;
}

Exclude&
Exclude::excludeAfter(const name::Component& from)
{
  ExcludeMap::iterator newFrom = m_entries.lower_bound(from);
  if (newFrom == m_entries.end() || !newFrom->second /*without ANY*/) {
    bool isNewEntry = false;
    std::tie(newFrom, isNewEntry) = m_entries.emplace(from, true);
    if (!isNewEntry) {
      // this means that the lower bound is equal to the item itself. So, just update ANY flag
      newFrom->second = true;
    }
  }
  // else
  // nothing special if start of the range already exists with ANY flag set

  // remove any intermediate node, since all of the are excluded
  m_entries.erase(m_entries.begin(), newFrom);

  m_wire.reset();
  return *this;
}

std::ostream&
operator<<(std::ostream& os, const Exclude& exclude)
{
  auto join = make_ostream_joiner(os, ',');
  for (const Exclude::Entry& entry : exclude.m_entries | boost::adaptors::reversed) {
    if (!entry.first.isNegInf) {
      join = entry.first.component;
    }
    if (entry.second) {
      join = '*';
    }
  }
  return os;
}

std::string
Exclude::toUri() const
{
  std::ostringstream os;
  os << *this;
  return os.str();
}

bool
Exclude::operator==(const Exclude& other) const
{
  return m_entries == other.m_entries;
}

size_t
Exclude::size() const
{
  return std::distance(begin(), end());
}

void
Exclude::clear()
{
  m_entries.clear();
  m_wire.reset();
}

Exclude::const_iterator::const_iterator(ExcludeMap::const_reverse_iterator it,
                                        ExcludeMap::const_reverse_iterator rend)
  : m_it(it)
  , m_rend(rend)
{
  this->update();
}

Exclude::const_iterator&
Exclude::const_iterator::operator++()
{
  bool wasInRange = m_it->second;
  ++m_it;
  if (wasInRange && m_it != m_rend) {
    BOOST_ASSERT(m_it->second == false); // consecutive ranges should have been combined
    ++m_it; // skip over range high limit
  }
  this->update();
  return *this;
}

Exclude::const_iterator
Exclude::const_iterator::operator++(int)
{
  const_iterator i = *this;
  this->operator++();
  return i;
}

void
Exclude::const_iterator::update()
{
  if (m_it == m_rend) {
    return;
  }

  if (m_it->second) { // range
    if (m_it->first.isNegInf) {
      m_range.fromInfinity = true;
    }
    else {
      m_range.fromInfinity = false;
      m_range.from = m_it->first.component;
    }

    auto next = std::next(m_it);
    if (next == m_rend) {
      m_range.toInfinity = true;
    }
    else {
      m_range.toInfinity = false;
      m_range.to = next->first.component;
    }
  }
  else { // single
    BOOST_ASSERT(!m_it->first.isNegInf);
    m_range.fromInfinity = m_range.toInfinity = false;
    m_range.from = m_range.to = m_it->first.component;
  }
}

} // namespace ndn
