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

#ifndef NDN_SECURITY_PIB_HPP
#define NDN_SECURITY_PIB_HPP

#include "identity-container.hpp"

namespace ndn {
namespace security {

class KeyChain;
class PibImpl;

/**
 * @brief represents the PIB
 *
 * The PIB (Public Information Base) stores the public portion of a user's cryptography keys.
 * The format and location of stored information is indicated by the PibLocator.
 * The PIB is designed to work with a TPM (Trusted Platform Module) which stores private keys.
 * There is a one-to-one association between PIB and TPM, and therefore the TpmLocator is recorded
 * by the PIB to enforce this association and prevent one from operating on mismatched PIB and TPM.
 *
 * Information in the PIB is organized in a hierarchy of Identity-Key-Certificate. At the top level,
 * the Pib class provides access to identities, and allows setting a default identity. Properties of
 * an identity can be accessed after obtaining an Identity object.
 *
 * @throw PibImpl::Error when underlying implementation has non-semantic error.
 */
class Pib : noncopyable
{
public:
  friend class KeyChain;

public:
  /// @brief represents a semantic error
  class Error : public std::runtime_error
  {
  public:
    explicit
    Error(const std::string& what)
      : std::runtime_error(what)
    {
    }
  };

public:

  ~Pib();

  /**
   * @brief return the scheme of the PibLocator
   */
  std::string
  getScheme() const
  {
    return m_scheme;
  }

  /**
   * @brief Get PIB Locator
   */
  std::string
  getPibLocator() const;

  /**
   * @brief Set the corresponding TPM information to @p tpmLocator.
   *
   * If the provided @p tpmLocator is different from the existing one, the
   * PIB will be reset, otherwise nothing will be changed.
   *
   * @param tmpLocator The name for the new tmpLocator
   */
  void
  setTpmLocator(const std::string& tpmLocator);

  /**
   * @brief Get TPM Locator
   */
  std::string
  getTpmLocator() const;

  /**
   * @brief Get an identity with name @p identityName.
   *
   * @param identityName The name for the identity to get.
   * @throw Pib::Error if the identity does not exist.
   */
  Identity
  getIdentity(const Name& identityName) const;

  /// @brief Get all the identities
  const IdentityContainer&
  getIdentities() const;

  /**
   * @brief Get the default identity.
   *
   * @return the default identity.
   * @throws Pib::Error if no default identity.
   */
  Identity&
  getDefaultIdentity() const;

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE: // write operations should be private

  /*
   * @brief Create an identity with name @p identityName and return a reference to it.
   *
   * If there already exists an identity for the name @p identityName, then it is returned.
   * If no default identity is set, the newly created identity will be set as the default.
   *
   * @param identityName The name for the identity to be added
   */
  Identity
  addIdentity(const Name& identityName);

  /*
   * @brief Remove an identity with name @p identityName.
   *
   * @param identityName The name for the identity to be deleted
   */
  void
  removeIdentity(const Name& identityName);

  /**
   * @brief Set an identity with name @p identityName as the default identity.
   *
   * Also create the identity if it does not exist.
   *
   * @param identityName The name for the default identity.
   * @return the default identity
   */
  Identity&
  setDefaultIdentity(const Name& identityName);

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  /*
   * @brief Create a new Pib with the specified @p location
   *
   * @param scheme The scheme for the Pib
   * @param location The location for the Pib
   * @param impl The backend implementation
   */
  Pib(const std::string scheme, const std::string& location, shared_ptr<PibImpl> impl);

  shared_ptr<PibImpl>
  getImpl()
  {
    return m_impl;
  }

protected:
  std::string m_scheme;
  std::string m_location;

  mutable bool m_hasDefaultIdentity;
  mutable Identity m_defaultIdentity;

  mutable bool m_needRefreshIdentities;
  mutable IdentityContainer m_identities;

  shared_ptr<PibImpl> m_impl;
};

} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_PIB_HPP
