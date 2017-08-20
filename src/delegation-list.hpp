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

#ifndef NDN_DELEGATION_LIST_HPP
#define NDN_DELEGATION_LIST_HPP

#include "delegation.hpp"
#include <initializer_list>

namespace ndn {

/** \brief represents a list of Delegations
 *  \sa https://named-data.net/doc/ndn-tlv/link.html
 *
 *  Delegations are stored in an std::vector, under the assumption that there is usually only a
 *  small number of Delegations, so that copying is acceptable when they are modified.
 */
class DelegationList
{
public:
  class Error : public tlv::Error
  {
  public:
    explicit
    Error(const std::string& what);

    Error(const std::string& what, const std::exception& innerException);
  };

  /** \brief construct an empty DelegationList
   */
  DelegationList();

  /** \brief construct a sorted DelegationList with specified delegations
   *
   *  This is equivalent to inserting each delegation into an empty DelegationList with INS_REPLACE
   *  conflict resolution.
   */
  DelegationList(std::initializer_list<Delegation> dels);

  /** \brief decode a DelegationList
   *  \sa wireDecode
   */
  explicit
  DelegationList(const Block& block, bool wantSort = true);

  /** \brief encode into wire format
   *  \param encoder either an EncodingBuffer or an EncodingEstimator
   *  \param type TLV-TYPE number, either Content (for \p Link) or ForwardingHint
   *  \throw std::invalid_argument \p type is invalid
   *  \throw Error there is no Delegation
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder, uint32_t type = tlv::ForwardingHint) const;

  /** \brief decode a DelegationList
   *  \param block either a Content block (from \p Link) or a ForwardingHint block
   *  \param wantSort if true, delegations are sorted
   *  \throw Error the block cannot be parsed as a list of Delegations
   */
  void
  wireDecode(const Block& block, bool wantSort = true);

  bool
  isSorted() const noexcept
  {
    return m_isSorted;
  }

  using const_iterator = std::vector<Delegation>::const_iterator;

  const_iterator
  begin() const noexcept
  {
    return m_dels.begin();
  }

  const_iterator
  end() const noexcept
  {
    return m_dels.end();
  }

  bool
  empty() const noexcept
  {
    return m_dels.empty();
  }

  size_t
  size() const noexcept
  {
    return m_dels.size();
  }

  /** \brief get the i-th delegation
   *  \pre i < size()
   */
  const Delegation&
  operator[](size_t i) const
  {
    BOOST_ASSERT(i < size());
    return m_dels[i];
  }

  /** \brief get the i-th delegation
   *  \throw std::out_of_range i >= size()
   */
  const Delegation&
  at(size_t i) const
  {
    return m_dels.at(i);
  }

public: // modifiers
  /** \brief sort the delegation list
   *  \post isSorted() == true
   *  \post Delegations are sorted in increasing preference order.
   *
   *  A DelegationList can be constructed as sorted or unsorted. In most cases, it is recommended
   *  to use a sorted DelegationList. An unsorted DelegationList is useful for extracting the i-th
   *  delegation from a received ForwardingHint or Link object.
   *
   *  This method turns an unsorted DelegationList into a sorted DelegationList.
   *  If access to unsorted DelegationList is not needed, it is more efficient to sort the
   *  DelegationList in wireDecode.
   */
  void
  sort();

  /** \brief what to do when inserting a duplicate name
   */
  enum InsertConflictResolution {
    /** \brief existing delegation(s) with the same name are replaced with the new delegation
     */
    INS_REPLACE,

    /** \brief multiple delegations with the same name are kept in the DelegationList
     *  \note This is NOT RECOMMENDED by Link specification.
     */
    INS_APPEND,

    /** \brief new delegation is not inserted if an existing delegation has the same name
     */
    INS_SKIP
  };

  /** \brief insert Delegation
   *  \return whether inserted
   */
  bool
  insert(uint64_t preference, const Name& name,
         InsertConflictResolution onConflict = INS_REPLACE);

  /** \brief insert Delegation
   *  \return whether inserted
   */
  bool
  insert(const Delegation& del, InsertConflictResolution onConflict = INS_REPLACE)
  {
    return this->insert(del.preference, del.name, onConflict);
  }

  /** \brief delete Delegation(s) with specified preference and name
   *  \return count of erased Delegation(s)
   */
  size_t
  erase(uint64_t preference, const Name& name)
  {
    return this->eraseImpl(preference, name);
  }

  /** \brief delete Delegation(s) with matching preference and name
   *  \return count of erased Delegation(s)
   */
  size_t
  erase(const Delegation& del)
  {
    return this->eraseImpl(del.preference, del.name);
  }

  /** \brief erase Delegation(s) with specified name
   *  \return count of erased Delegation(s)
   */
  size_t
  erase(const Name& name)
  {
    return this->eraseImpl(nullopt, name);
  }

private:
  static bool
  isValidTlvType(uint32_t type);

  void
  insertImpl(uint64_t preference, const Name& name);

  size_t
  eraseImpl(optional<uint64_t> preference, const Name& name);

private:
  bool m_isSorted;

  /** \brief delegation container; its contents are sorted when \p m_isSorted is true
   *  \note This container is a member field rather than a base class, in order to ensure contents
   *        are sorted when \p m_isSorted is true.
   *  \note A vector is chosen instead of a std::set, so that the container can be unsorted when
   *        \p m_isSorted is false. This container is expected to have less than seven items, and
   *        therefore the overhead of moving items during insertion and deletion is small.
   */
  std::vector<Delegation> m_dels;

  friend bool operator==(const DelegationList&, const DelegationList&);
};

#ifndef DOXYGEN
extern template size_t
DelegationList::wireEncode<encoding::EncoderTag>(EncodingBuffer&, uint32_t) const;

extern template size_t
DelegationList::wireEncode<encoding::EstimatorTag>(EncodingEstimator&, uint32_t) const;
#endif

/** \brief compare whether two DelegationLists are equal
 *  \note Order matters! If two DelegationLists contain the same Delegations but at least one is
 *        unsorted, they may compare unequal if the Delegations appear in different order.
 */
bool
operator==(const DelegationList& lhs, const DelegationList& rhs);

inline bool
operator!=(const DelegationList& lhs, const DelegationList& rhs)
{
  return !(lhs == rhs);
}

std::ostream&
operator<<(std::ostream& os, const DelegationList& dl);

} // namespace ndn

#endif // NDN_DELEGATION_LIST_HPP
