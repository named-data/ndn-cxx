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

#ifndef NDN_CXX_SECURITY_PIB_CERTIFICATE_CONTAINER_HPP
#define NDN_CXX_SECURITY_PIB_CERTIFICATE_CONTAINER_HPP

#include "ndn-cxx/security/certificate.hpp"

#include <iterator>
#include <set>
#include <unordered_map>

namespace ndn::security::pib {

class PibImpl;

/**
 * @brief Container of certificates of a key.
 *
 * The container is used to search/enumerate the certificates of a key.
 * It can be created only by the KeyImpl private class.
 *
 * @sa Key::getCertificates()
 */
class CertificateContainer : noncopyable
{
private:
  using NameSet = std::set<Name>;

public:
  class const_iterator
  {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = const Certificate;
    using difference_type   = std::ptrdiff_t;
    using pointer           = value_type*;
    using reference         = value_type&;

    const_iterator() = default;

    Certificate
    operator*();

    const_iterator&
    operator++()
    {
      ++m_it;
      return *this;
    }

    const_iterator
    operator++(int)
    {
      const_iterator it(*this);
      ++m_it;
      return it;
    }

    bool
    operator==(const const_iterator& other) const;

    bool
    operator!=(const const_iterator& other) const
    {
      return !this->operator==(other);
    }

  private:
    const_iterator(NameSet::const_iterator it, const CertificateContainer& container) noexcept;

  private:
    NameSet::const_iterator m_it;
    const CertificateContainer* m_container = nullptr;

    friend CertificateContainer;
  };

  using iterator = const_iterator;

public:
  const_iterator
  begin() const noexcept
  {
    return {m_certNames.begin(), *this};
  }

  const_iterator
  end() const noexcept
  {
    return {};
  }

  const_iterator
  find(const Name& certName) const;

  /**
   * @brief Check whether the container is empty.
   */
  [[nodiscard]] bool
  empty() const noexcept
  {
    return m_certNames.empty();
  }

  /**
   * @brief Return the number of certificates in the container.
   */
  size_t
  size() const noexcept
  {
    return m_certNames.size();
  }

  /**
   * @brief Add @p certificate into the container.
   * @throw std::invalid_argument The name of @p certificate does not match the key name.
   */
  void
  add(const Certificate& certificate);

  /**
   * @brief Remove a certificate with @p certName from the container.
   * @throw std::invalid_argument @p certName does not match the key name.
   */
  void
  remove(const Name& certName);

  /**
   * @brief Return a certificate by name.
   * @throw Pib::Error The certificate does not exist.
   * @throw std::invalid_argument @p certName does not match the key name.
   */
  Certificate
  get(const Name& certName) const;

  /**
   * @brief Check if the container is consistent with the backend storage.
   * @note This method is heavyweight and should be used in debugging mode only.
   */
  bool
  isConsistent() const;

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE: // private interface for KeyImpl
  /**
   * @brief Create certificate container for @p keyName.
   * @param pibImpl The PIB backend implementation.
   */
  CertificateContainer(const Name& keyName, shared_ptr<PibImpl> pibImpl);

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  // cache of loaded certificates
  mutable std::unordered_map<Name, Certificate> m_certs;

private:
  NameSet m_certNames;
  const Name m_keyName;
  const shared_ptr<PibImpl> m_pib;

  friend class KeyImpl;
};

} // namespace ndn::security::pib

#endif // NDN_CXX_SECURITY_PIB_CERTIFICATE_CONTAINER_HPP
