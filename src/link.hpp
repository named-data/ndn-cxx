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

#ifndef NDN_LINK_HPP
#define NDN_LINK_HPP

#include "data.hpp"
#include <set>

namespace ndn {

const size_t INVALID_SELECTED_DELEGATION_INDEX = std::numeric_limits<size_t>::max();

/** @brief represents a Link instance
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

  // The ordering is based on the preference number and needs to be preserved
  typedef std::set<std::pair<uint32_t, Name>> DelegationSet;

  /**
   * @brief Create an empty Link object
   *
   * Note that in certain contexts that use Link::shared_from_this(), Link must be
   * created using `make_shared`:
   *
   *     shared_ptr<Link> linkObject = make_shared<Link>();
   *
   * Otherwise, Link::shared_from_this() will throw std::bad_weak_ptr.
   */
  Link() = default;

  /**
   * @brief Create a Link object from a Block
   *
   * Note that in certain contexts that use Link::shared_from_this(), Link must be
   * created using `make_shared`:
   *
   *     shared_ptr<Link> linkObject = make_shared<Link>(block);
   *
   * Otherwise, Link::shared_from_this() will throw std::bad_weak_ptr.
   */
  explicit
  Link(const Block& block);

  /**
   * @brief Create a Link object with the given name
   *
   * @param name A reference to the name of the redirected namespace
   *
   * Note that in certain contexts that use Link::shared_from_this(), Link must be
   * created using `make_shared`:
   *
   *     shared_ptr<Link> link = make_shared<Link>(name);
   *
   * Otherwise, Link::shared_from_this() will throw std::bad_weak_ptr.
   */
  explicit
  Link(const Name& name);

 /**
   * @brief Create a Link object with the given name and pairs of <Preference, Name>
   *
   * @param name A reference to the name of the redirected namespace
   * @param links A reference to the list of pairs of the redirected namespace
   * along with its priority
   *
   * Note that in certain contexts that use Link::shared_from_this(), Link must be
   * created using `make_shared`:
   *
   *     shared_ptr<Link> link = make_shared<Link>(name, links);
   *
   * Otherwise, Link::shared_from_this() will throw std::bad_weak_ptr.
   */
  Link(const Name& name, std::initializer_list<std::pair<uint32_t, Name>> links);

  /**
   * @brief Add a delegation in the format of <Name, Preference>
   * @param preference The preference of the delegation to be added
   * @param name The name of the delegation to be added
   * @note If a delegation with @p name exists, its preference will be updated
   */
  void
  addDelegation(uint32_t preference, const Name& name);

  /**
   * @brief Remove a delegation whose name is @p name
   * @param name The name of the delegation to be removed
   * @return true if delegation is removed, otherwise false
   */
  bool
  removeDelegation(const Name& name);

  /**
   * @brief Get the pairs of <Name, Preference>
   * @return a set of delegations
   */
  const DelegationSet&
  getDelegations() const;

  /**
   * @brief Decode from the wire format
   * @warning This method does not preserve the relative order between delegations.
   *     To get a delegation by index, use \p getDelegationFromWire method.
   */
  void
  wireDecode(const Block& wire);

  /** @brief gets the delegation at \p index from \p block
   *  @param block wire format of a Link object
   *  @param index 0-based index of a delegation in the Link object
   *  @return delegation preference and name
   *  @throw std::out_of_range index is out of range
   */
  static std::tuple<uint32_t, Name>
  getDelegationFromWire(const Block& block, size_t index);

  /** @brief finds index of a delegation with \p delegationName from \p block
   *  @param block wire format of a Link object
   *  @return 0-based index of the first delegation with \p delegationName ,
   *          or -1 if no such delegation exists
   */
  static ssize_t
  findDelegationFromWire(const Block& block, const Name& delegationName);

  static ssize_t
  countDelegationsFromWire(const Block& block);

protected:
  /** @brief prepend Link object as a Content block to the encoder
   *
   *  The outermost Content element is not part of Link object structure.
   */
  template<encoding::Tag TAG>
  size_t
  encodeContent(EncodingImpl<TAG>& encoder) const;

  void
  encodeContent();

  void
  decodeContent();

private:
  bool
  removeDelegationNoEncode(const Name& name);

private:
  DelegationSet m_delegations;
};

} // namespace ndn

#endif // NDN_LINK_HPP
