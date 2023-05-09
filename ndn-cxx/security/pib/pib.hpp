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

#ifndef NDN_CXX_SECURITY_PIB_PIB_HPP
#define NDN_CXX_SECURITY_PIB_PIB_HPP

#include "ndn-cxx/security/pib/identity-container.hpp"

namespace ndn::security {
namespace pib {

class PibImpl;

/**
 * @brief Frontend to the Public Information Base.
 *
 * The PIB (Public Information Base) stores the public portion of a user's cryptography keys.
 * The format and location of stored information is indicated by the PibLocator.
 * The PIB is designed to work with a TPM (Trusted Platform Module) which stores private keys.
 * There is a one-to-one association between PIB and TPM, and therefore the TpmLocator is recorded
 * by the PIB to enforce this association and prevent one from operating on mismatched PIB and TPM.
 *
 * Information in the PIB is organized in a hierarchy of Identity-Key-Certificate. At the top level,
 * the Pib class provides access to identities and allows setting a default identity. The properties
 * of an identity can be accessed after obtaining an Identity object.
 *
 * @note Pib instance is created and managed only by KeyChain. KeyChain::getPib() returns
 *       a reference to the managed Pib instance, through which it is possible to
 *       retrieve information about identities, keys, and certificates.
 *
 * @throw PibImpl::Error When the underlying implementation has a non-semantic error.
 */
class Pib : noncopyable
{
public:
  /// @brief Represents a semantic error.
  class Error : public std::runtime_error
  {
  public:
    using std::runtime_error::runtime_error;
  };

  ~Pib();

  /**
   * @brief Return the PIB Locator.
   */
  const std::string&
  getPibLocator() const
  {
    return m_locator;
  }

  /**
   * @brief Return the associated TPM Locator or an empty string if unset.
   */
  std::string
  getTpmLocator() const;

  /**
   * @brief Set the associated TPM information to @p tpmLocator.
   * @note If the provided @p tpmLocator differs from the current one, reset() is called.
   */
  void
  setTpmLocator(const std::string& tpmLocator);

  /**
   * @brief Reset the contents of the PIB, including reset of the TPM Locator.
   */
  void
  reset();

  /**
   * @brief Return an identity with name @p identityName.
   * @throw Pib::Error The desired identity does not exist.
   */
  Identity
  getIdentity(const Name& identityName) const;

  /**
   * @brief Return all the identities.
   */
  const IdentityContainer&
  getIdentities() const;

  /**
   * @brief Return the default identity.
   * @throw Pib::Error No default identity exists.
   */
  Identity
  getDefaultIdentity() const;

NDN_CXX_PUBLIC_WITH_TESTS_ELSE_PRIVATE: // write operations are accessible only by KeyChain
  /**
   * @brief Create a Pib instance.
   *
   * @param locator The PIB locator
   * @param impl The backend implementation
   */
  Pib(const std::string& locator, shared_ptr<PibImpl> impl);

  /**
   * @brief Add an identity.
   *
   * If no default identity is currently set, the new identity will become the default identity.
   *
   * @return Handle of the added identity.
   */
  Identity
  addIdentity(const Name& identity);

  /**
   * @brief Remove an identity.
   *
   * If the default identity is being removed, the PIB will no longer have a default identity.
   */
  void
  removeIdentity(const Name& identity);

  /**
   * @brief Set an identity as the default identity.
   *
   * The identity will be created if it does not exist.
   *
   * @return Handle of the default identity.
   */
  Identity
  setDefaultIdentity(const Name& identity);

private:
  const std::string m_locator;
  const shared_ptr<PibImpl> m_impl;

  IdentityContainer m_identities;
  mutable Identity m_defaultIdentity;

  friend KeyChain;
};

} // namespace pib

using pib::Pib;

} // namespace ndn::security

#endif // NDN_CXX_SECURITY_PIB_PIB_HPP
