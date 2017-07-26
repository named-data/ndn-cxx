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

#ifndef NDN_LINK_HPP
#define NDN_LINK_HPP

#include "data.hpp"
#include "delegation-list.hpp"
#include <set>

namespace ndn {

/** @brief represents a Link object
 */
class Link : public Data
{
public:
  class Error : public Data::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : Data::Error(what)
    {
    }
  };

  /** @brief Create an empty Link object
   *
   *  Note that in certain contexts that use Link::shared_from_this(), Link must be
   *  created using `make_shared`:
   *
   *      shared_ptr<Link> linkObject = make_shared<Link>();
   */
  Link();

  /** @brief Decode a Link object from a Block
   *  @param wire a TLV block
   *  @param wantSort if false, relative order among delegations is preserved
   *
   *  Note that in certain contexts that use Link::shared_from_this(), Link must be
   *  created using `make_shared`:
   *
   *      shared_ptr<Link> linkObject = make_shared<Link>(block);
   */
  explicit
  Link(const Block& wire, bool wantSort = true);

  /** @brief Create a Link object with the given name and delegations
   *  @param name A reference to the name of the redirected namespace
   *  @param dels Delegations in payload
   *
   *  Note that in certain contexts that use Link::shared_from_this(), Link must be
   *  created using `make_shared`:
   *
   *      shared_ptr<Link> link = make_shared<Link>(name, dels);
   */
  explicit
  Link(const Name& name, std::initializer_list<Delegation> dels = {});

  /** @brief Decode from the wire format
   *  @param wire a TLV block
   *  @param wantSort if false, relative order among delegations is preserved
   */
  void
  wireDecode(const Block& wire, bool wantSort = true);

  /** @brief Get the delegations
   */
  const DelegationList&
  getDelegationList() const
  {
    return m_delList;
  }

  /** @brief Set the delegations
   *  @note This is more efficient than multiple addDelegation and removeDelegation invocations.
   */
  void
  setDelegationList(const DelegationList& dels);

  /** @brief Add a delegation in the format of <Name, Preference>
   *  @param preference The preference of the delegation to be added
   *  @param name The name of the delegation to be added
   *  @note If a delegation with @p name exists, its preference will be updated
   */
  void
  addDelegation(uint32_t preference, const Name& name);

  /** @brief Remove a delegation whose name is @p name
   *  @param name The name of the delegation to be removed
   *  @return true if delegation is removed, otherwise false
   */
  bool
  removeDelegation(const Name& name);

public: // deprecated APIs
  using DelegationSet = std::set<std::pair<uint32_t, Name>>;
  using DelegationTuple = std::tuple<uint32_t, Name>;

  class PairInitializerListHelper
  {
  public:
    PairInitializerListHelper(std::initializer_list<std::pair<uint32_t, Name>> dels);

  private:
    DelegationList m_delList;
    friend class Link;
  };

  /** @brief Create a Link object with the given name and delegations
   *  @param name A reference to the name of the redirected namespace
   *  @param dels Delegations in payload
   *  @deprecated use Link(const Name&, std::initializer_list<Delegation>)
   *  @note This overload is selected only if the caller explicitly passes
   *        std::initializer_list<std::pair<uint32_t, Name>> to Link constructor;
   *        otherwise, Link(const Name&, std::initializer_list<Delegation>) is preferred.
   */
  DEPRECATED(
  Link(const Name& name, PairInitializerListHelper dels));

  /** @deprecated use getDelegationList()
   */
  DEPRECATED(
  const DelegationSet&
  getDelegations() const);

  /** @brief gets the delegation at @p index from @p block
   *  @param block wire format of a Link object
   *  @param index 0-based index of a delegation in the Link object
   *  @return delegation preference and name
   *  @throw std::out_of_range index is out of range
   *  @deprecated use Link(block, false).getDelegationList().at(index)
   */
  DEPRECATED(
  static DelegationTuple
  getDelegationFromWire(const Block& block, size_t index));

  /** @brief finds index of a delegation with @p delegationName from @p block
   *  @param block wire format of a Link object
   *  @param delegationName delegation name in the Link object
   *  @return 0-based index of the first delegation with @p delegationName ,
   *          or -1 if no such delegation exists
   *  @deprecated find within Link(block, false).getDelegationList()
   */
  DEPRECATED(
  static ssize_t
  findDelegationFromWire(const Block& block, const Name& delegationName));

  /** @deprecated use Link(block, false).getDelegationList().size()
   */
  DEPRECATED(
  static ssize_t
  countDelegationsFromWire(const Block& block));

private:
  void
  encodeContent();

private:
  DelegationList m_delList;
  mutable bool m_isDelSetDirty = false;
  mutable DelegationSet m_delSet;
};

} // namespace ndn

#endif // NDN_LINK_HPP
