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

#ifndef NDN_SECURITY_PIB_CERTIFICATE_CONTAINER_HPP
#define NDN_SECURITY_PIB_CERTIFICATE_CONTAINER_HPP

#include "../v2/certificate.hpp"

#include <set>
#include <unordered_map>

namespace ndn {
namespace security {
namespace pib {

class PibImpl;

namespace detail {
class KeyImpl;
} // namespace detail

/**
 * @brief Container of certificates of a key
 *
 * The container is used to search/enumerate certificates of a key.
 * The container can be created only by detail::KeyImpl.
 */
class CertificateContainer : noncopyable
{
public:
  class const_iterator : public std::iterator<std::forward_iterator_tag, const v2::Certificate>
  {
  public:
    const_iterator();

    v2::Certificate
    operator*();

    const_iterator&
    operator++();

    const_iterator
    operator++(int);

    bool
    operator==(const const_iterator& other) const;

    bool
    operator!=(const const_iterator& other) const;

  private:
    const_iterator(std::set<Name>::const_iterator it, const CertificateContainer& container);

  private:
    std::set<Name>::const_iterator m_it;
    const CertificateContainer* m_container;

    friend class CertificateContainer;
  };

  typedef const_iterator iterator;

public:
  const_iterator
  begin() const;

  const_iterator
  end() const;

  const_iterator
  find(const Name& certName) const;

  size_t
  size() const;

  /**
   * @brief Add @p certificate into the container
   * @throw std::invalid_argument the name of @p certificate does not match the key name
   */
  void
  add(const v2::Certificate& certificate);

  /**
   * @brief Remove a certificate with @p certName from the container
   * @throw std::invalid_argument @p certName does not match the key name
   */
  void
  remove(const Name& certName);

  /**
   * @brief Get a certificate with @p certName from the container
   * @throw std::invalid_argument @p certName does not match the key name
   * @throw Pib::Error the certificate does not exist
   */
  v2::Certificate
  get(const Name& certName) const;

  /**
   * @brief Check if the container is consistent with the backend storage
   * @note this method is heavyweight and should be used in debugging mode only.
   */
  bool
  isConsistent() const;

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  /**
   * @brief Create certificate container for a key with @p keyName
   * @param pibImpl The PIB backend implementation.
   */
  CertificateContainer(const Name& keyName, shared_ptr<PibImpl> pibImpl);

  const std::set<Name>&
  getCertNames() const
  {
    return m_certNames;
  }

  const std::unordered_map<Name, v2::Certificate>&
  getCache() const
  {
    return m_certs;
  }

private:
  Name m_keyName;
  std::set<Name> m_certNames;
  /// @brief Cache of loaded certificates
  mutable std::unordered_map<Name, v2::Certificate> m_certs;

  shared_ptr<PibImpl> m_pib;

  friend class detail::KeyImpl;
};

} // namespace pib

using pib::CertificateContainer;

} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_PIB_CERTIFICATE_CONTAINER_HPP
