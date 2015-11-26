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

#ifndef NDN_SECURITY_PIB_PIB_IMPL_HPP
#define NDN_SECURITY_PIB_PIB_IMPL_HPP

#include "pib.hpp"
#include <set>
#include "../v2/certificate.hpp"

namespace ndn {
namespace security {
namespace pib {

/**
 * @brief Abstract class of PIB implementation
 *
 * This class defines the interface that an actual PIB (e.g., one based on sqlite3)
 * implementation should provide.
 */
class PibImpl : noncopyable
{
public:
  /**
   * @brief represents a non-semantic error
   *
   * A subclass of PibImpl may throw a subclass of this type when
   * there's a non-semantic error, such as a storage problem.
  */
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
  virtual
  ~PibImpl() = default;

public: // TpmLocator management
  /**
   * @brief Set the corresponding TPM information to @p tpmLocator
   *
   * This method does not reset contents of the PIB
   */
  virtual void
  setTpmLocator(const std::string& tpmLocator) = 0;

  /**
   * @brief Get TPM Locator
   */
  virtual std::string
  getTpmLocator() const = 0;

public: // Identity management
  /**
   * @brief Check the existence of an identity.
   *
   * @param identity The name of the identity.
   * @return true if the identity exists, otherwise false.
   */
  virtual bool
  hasIdentity(const Name& identity) const = 0;

  /**
   * @brief Add an identity.
   *
   * If the identity already exists, do nothing.  If no default identity has been set, set the
   * added one as default identity.
   *
   * @param identity The name of the identity to add.
   */
  virtual void
  addIdentity(const Name& identity) = 0;

  /**
   * @brief Remove an identity and related keys and certificates.
   *
   * If the default identity is being removed, no default identity will be selected.  If the
   * identity does not exist, do nothing.
   *
   * @param identity The name of the identity to remove.
   */
  virtual void
  removeIdentity(const Name& identity) = 0;

  /**
   * @brief Erasing all certificates, keys, and identities
   */
  virtual void
  clearIdentities() = 0;

  /**
   * @brief Get the name of all the identities
   */
  virtual std::set<Name>
  getIdentities() const = 0;

  /**
   * @brief Set an identity with name @p identityName as the default identity.
   *
   * If @p identityName identity does not exist, it will be created.
   *
   * @param identityName The name for the default identity.
   */
  virtual void
  setDefaultIdentity(const Name& identityName) = 0;

  /**
   * @brief Get the default identity.
   *
   * @return The name for the default identity.
   * @throw Pib::Error no default identity.
   */
  virtual Name
  getDefaultIdentity() const = 0;

public: // Key management
  /**
   * @brief Check the existence of a key with @p keyName.
   *
   * @return true if the key exists, otherwise false. Return false if the identity does not exist
   */
  virtual bool
  hasKey(const Name& keyName) const = 0;

  /**
   * @brief Add a key.
   *
   * If a key with the same name already exists, overwrite the key.  If the identity does not
   * exist, it will be created.  If no default key of the identity has been set, set the added
   * one as default key of the identity.  If no default identity has been set, @p identity
   * becomes the default.
   *
   * @param identity The name of the belonged identity.
   * @param keyName The key name.
   * @param key The public key bits.
   * @param keyLen The length of the public key.
   */
  virtual void
  addKey(const Name& identity, const Name& keyName, const uint8_t* key, size_t keyLen) = 0;

  /**
   * @brief Remove a key with @p keyName and related certificates
   *
   * If the key does not exist, do nothing.
   */
  virtual void
  removeKey(const Name& keyName) = 0;

  /**
   * @brief Get the key bits of a key with name @p keyName.
   *
   * @return key bits
   * @throw Pib::Error the key does not exist.
   */
  virtual Buffer
  getKeyBits(const Name& keyName) const = 0;

  /**
   * @brief Get all the key names of an identity with name @p identity
   *
   * The returned key names can be used to create a KeyContainer.  With key name and backend
   * implementation, one can create a Key frontend instance.
   *
   * @return the key name component set. If the identity does not exist, return an empty set.
   */
  virtual std::set<Name>
  getKeysOfIdentity(const Name& identity) const = 0;

  /**
   * @brief Set an key with @p keyName as the default key of an identity with name @p identity.
   *
   * @throw Pib::Error the key does not exist.
   */
  virtual void
  setDefaultKeyOfIdentity(const Name& identity, const Name& keyName) = 0;

  /**
   * @return The name of the default key of an identity with name @p identity.
   *
   * @throw Pib::Error no default key or the identity does not exist.
   */
  virtual Name
  getDefaultKeyOfIdentity(const Name& identity) const = 0;

public: // Certificate Management
  /**
   * @brief Check the existence of a certificate with name @p certName.
   *
   * @param certName The name of the certificate.
   * @return true if the certificate exists, otherwise false.
   */
  virtual bool
  hasCertificate(const Name& certName) const = 0;

  /**
   * @brief Add a certificate.
   *
   * If a certificate with the same name (without implicit digest) already exists, overwrite
   * the certificate.  If the key or identity does not exist, they will be created.  If no
   * default certificate of the key has been set, set the added one as default certificate of
   * the key.  If no default key was set for the identity, it will be set as default key for
   * the identity.  If no default identity was selected, the certificate's identity becomes
   * default.
   *
   * @param certificate The certificate to add.
   */
  virtual void
  addCertificate(const v2::Certificate& certificate) = 0;

  /**
   * @brief Remove a certificate with name @p certName.
   *
   * If the certificate does not exist, do nothing.
   *
   * @param certName The name of the certificate.
   */
  virtual void
  removeCertificate(const Name& certName) = 0;

  /**
   * @brief Get a certificate with name @p certName.
   *
   * @param certName The name of the certificate.
   * @return the certificate.
   * @throw Pib::Error the certificate does not exist.
   */
  virtual v2::Certificate
  getCertificate(const Name& certName) const = 0;

  /**
   * @brief Get a list of certificate names of a key with id @p keyName.
   *
   * The returned certificate names can be used to create a CertificateContainer.  With
   * certificate name and backend implementation, one can obtain the certificate.
   *
   * @return The certificate name set. If the key does not exist, return an empty set.
   */
  virtual std::set<Name>
  getCertificatesOfKey(const Name& keyName) const = 0;

  /**
   * @brief Set a cert with name @p certName as the default of a key with @p keyName.
   *
   * @throw Pib::Error the certificate with name @p certName does not exist.
   */
  virtual void
  setDefaultCertificateOfKey(const Name& keyName, const Name& certName) = 0;

  /**
   * @return Get the default certificate of a key with @p keyName.
   *
   * @throw Pib::Error the default certificate does not exist.
   */
  virtual v2::Certificate
  getDefaultCertificateOfKey(const Name& keyName) const = 0;
};

} // namespace pib
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_PIB_PIB_IMPL_HPP
