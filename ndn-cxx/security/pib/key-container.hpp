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

#ifndef NDN_CXX_SECURITY_PIB_KEY_CONTAINER_HPP
#define NDN_CXX_SECURITY_PIB_KEY_CONTAINER_HPP

#include "ndn-cxx/security/pib/key.hpp"

#include <set>
#include <unordered_map>

namespace ndn::security::pib {

class PibImpl;

/**
 * @brief Container of keys of an identity.
 *
 * The container is used to search/enumerate the keys of an identity.
 * It can be created only by the IdentityImpl private class.
 *
 * @sa Identity::getKeys()
 */
class KeyContainer : noncopyable
{
private:
  using NameSet = std::set<Name>;

public:
  class const_iterator : public boost::forward_iterator_helper<const_iterator, const Key>
  {
  public:
    const_iterator() = default;

    Key
    operator*() const
    {
      BOOST_ASSERT(m_container != nullptr);
      return m_container->get(*m_it);
    }

    const_iterator&
    operator++()
    {
      ++m_it;
      return *this;
    }

    friend bool
    operator==(const const_iterator& lhs, const const_iterator& rhs) noexcept
    {
      return lhs.equals(rhs);
    }

  private:
    const_iterator(NameSet::const_iterator it, const KeyContainer& container) noexcept
      : m_it(it)
      , m_container(&container)
    {
    }

    bool
    equals(const const_iterator& other) const noexcept;

  private:
    NameSet::const_iterator m_it;
    const KeyContainer* m_container = nullptr;

    friend KeyContainer;
  };

  using iterator = const_iterator;

public:
  const_iterator
  begin() const noexcept
  {
    return {m_keyNames.begin(), *this};
  }

  const_iterator
  end() const noexcept
  {
    return {};
  }

  const_iterator
  find(const Name& keyName) const;

  /**
   * @brief Check whether the container is empty.
   */
  [[nodiscard]] bool
  empty() const noexcept
  {
    return m_keyNames.empty();
  }

  /**
   * @brief Return the number of keys in the container.
   */
  size_t
  size() const noexcept
  {
    return m_keyNames.size();
  }

  /**
   * @brief Add @p key with name @p keyName into the container.
   *
   * If a key with the same name already exists, it will be overwritten.
   *
   * @throw std::invalid_argument @p keyName does not match the identity.
   */
  Key
  add(span<const uint8_t> key, const Name& keyName);

  /**
   * @brief Remove a key with @p keyName from the container.
   * @throw std::invalid_argument @p keyName does not match the identity.
   */
  void
  remove(const Name& keyName);

  /**
   * @brief Return a key by name.
   * @throw Pib::Error The key does not exist.
   * @throw std::invalid_argument @p keyName does not match the identity.
   */
  Key
  get(const Name& keyName) const;

  /**
   * @brief Check if the container is consistent with the backend storage.
   * @note This method is heavyweight and should be used in debugging mode only.
   */
  bool
  isConsistent() const;

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE: // private interface for IdentityImpl
  /**
   * @brief Create key container for @p identity.
   * @param pibImpl The PIB backend implementation.
   */
  KeyContainer(const Name& identity, shared_ptr<PibImpl> pibImpl);

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  // cache of loaded KeyImpl
  mutable std::unordered_map<Name, shared_ptr<KeyImpl>> m_keys;

private:
  NameSet m_keyNames;
  const Name m_identity;
  const shared_ptr<PibImpl> m_pib;

  friend class IdentityImpl;
};

} // namespace ndn::security::pib

#endif // NDN_CXX_SECURITY_PIB_KEY_CONTAINER_HPP
