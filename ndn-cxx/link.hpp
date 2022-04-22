/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#ifndef NDN_CXX_LINK_HPP
#define NDN_CXX_LINK_HPP

#include "ndn-cxx/data.hpp"

namespace ndn {

/** @brief represents a Link object
 */
class Link : public Data
{
public:
  class Error : public Data::Error
  {
  public:
    using Data::Error::Error;
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
   *
   *  Note that in certain contexts that use Link::shared_from_this(), Link must be
   *  created using `make_shared`:
   *
   *      shared_ptr<Link> linkObject = make_shared<Link>(block);
   */
  explicit
  Link(const Block& wire);

  /** @brief Create a Link object with the given name and delegations
   *  @param name A reference to the name of the redirected namespace
   *  @param delegations Delegations in payload
   *
   *  Note that in certain contexts that use Link::shared_from_this(), Link must be
   *  created using `make_shared`:
   *
   *      shared_ptr<Link> link = make_shared<Link>(name, delegations);
   */
  explicit
  Link(const Name& name, std::initializer_list<Name> delegations = {});

  /** @brief Decode from the wire format
   *  @param wire a TLV block
   */
  void
  wireDecode(const Block& wire);

  /** @brief Get the delegations
   */
  span<const Name>
  getDelegationList() const
  {
    return m_delegations;
  }

  /** @brief Set the delegations
   *  @note This is more efficient than multiple addDelegation and removeDelegation invocations.
   */
  void
  setDelegationList(std::vector<Name> delegations);

  /** @brief Append a delegation at the end
   *  @param name Delegation name
   *  @return true if delegation is added, false if same name already exists
   */
  bool
  addDelegation(const Name& name);

  /** @brief Remove a delegation whose name is @p name
   *  @param name Delegation name
   *  @return true if delegation is removed, otherwise false
   */
  bool
  removeDelegation(const Name& name);

private:
  void
  encodeContent();

private:
  std::vector<Name> m_delegations;
};

} // namespace ndn

#endif // NDN_CXX_LINK_HPP
