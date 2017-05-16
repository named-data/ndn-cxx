/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
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

#ifndef NDN_SECURITY_PIB_KEY_CONTAINER_HPP
#define NDN_SECURITY_PIB_KEY_CONTAINER_HPP

#include "key.hpp"

#include <set>
#include <unordered_map>

namespace ndn {
namespace security {
namespace pib {

class PibImpl;

namespace detail {
class KeyImpl;
class IdentityImpl;
} // namespace detail

/**
 * @brief Container of keys of an identity
 *
 * The container is used to search/enumerate keys of an identity.
 * The container can be created only by detail::IdentityImpl.
 */
class KeyContainer : noncopyable
{
public:
  class const_iterator : public std::iterator<std::forward_iterator_tag, const Key>
  {
  public:
    const_iterator();

    Key
    operator*();

    const_iterator&
    operator++();

    const_iterator
    operator++(int);

    bool
    operator==(const const_iterator& other);

    bool
    operator!=(const const_iterator& other);

  private:
    const_iterator(std::set<Name>::const_iterator it, const KeyContainer& container);

  private:
    std::set<Name>::const_iterator m_it;
    const KeyContainer* m_container;

    friend class KeyContainer;
  };

  typedef const_iterator iterator;

public:
  const_iterator
  begin() const;

  const_iterator
  end() const;

  const_iterator
  find(const Name& keyName) const;

  size_t
  size() const;

  /**
   * @brief Add @p key of @p keyLen bytes with @p keyName into the container
   * @throw std::invalid_argument @p keyName does not match the identity
   *
   * If a key with the same name already exists, overwrite the key.
   */
  Key
  add(const uint8_t* key, size_t keyLen, const Name& keyName);

  /**
   * @brief Remove a key with @p keyName from the container
   * @throw std::invalid_argument @p keyName does not match the identity
   */
  void
  remove(const Name& keyName);

  /**
   * @brief Get a key with @p keyName from the container
   * @throw std::invalid_argument @p keyName does not match the identity
   * @throw Pib::Error the key does not exist
   */
  Key
  get(const Name& keyName) const;

  /**
   * @brief Check if the container is consistent with the backend storage
   *
   * @note this method is heavyweight and should be used in debugging mode only.
   */
  bool
  isConsistent() const;

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  /**
   * @brief Create key container for @p identity
   * @param pibImpl The PIB backend implementation.
   */
  KeyContainer(const Name& identity, shared_ptr<PibImpl> pibImpl);

  const std::set<Name>&
  getKeyNames() const
  {
    return m_keyNames;
  }

  const std::unordered_map<Name, shared_ptr<detail::KeyImpl>>&
  getLoadedKeys() const
  {
    return m_keys;
  }

private:
  Name m_identity;
  std::set<Name> m_keyNames;
  /// @brief Cache of loaded detail::KeyImpl.
  mutable std::unordered_map<Name, shared_ptr<detail::KeyImpl>> m_keys;

  shared_ptr<PibImpl> m_pib;

  friend class detail::IdentityImpl;
};

} // namespace pib

using pib::KeyContainer;

} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_PIB_KEY_CONTAINER_HPP
