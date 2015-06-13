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

#ifndef NDN_SECURITY_PIB_MEMORY_HPP
#define NDN_SECURITY_PIB_MEMORY_HPP

#include "pib-impl.hpp"

namespace ndn {
namespace security {

/**
 * @brief An in-memory implementation of Pib
 *
 * All the contents in Pib are stored in memory
 * and have the same lifetime as the implementation instance.
 */
class PibMemory : public PibImpl
{
public:
  class Error : public PibImpl::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : PibImpl::Error(what)
    {
    }
  };

public:
  PibMemory();

public: // TpmLocator management

  virtual void
  setTpmLocator(const std::string& tpmLocator) NDN_CXX_DECL_OVERRIDE;

  virtual std::string
  getTpmLocator() const NDN_CXX_DECL_OVERRIDE;

public: // Identity management

  virtual bool
  hasIdentity(const Name& identity) const NDN_CXX_DECL_OVERRIDE;

  virtual void
  addIdentity(const Name& identity) NDN_CXX_DECL_OVERRIDE;

  virtual void
  removeIdentity(const Name& identity) NDN_CXX_DECL_OVERRIDE;

  virtual std::set<Name>
  getIdentities() const NDN_CXX_DECL_OVERRIDE;

  virtual void
  setDefaultIdentity(const Name& identityName) NDN_CXX_DECL_OVERRIDE;

  virtual Name
  getDefaultIdentity() const NDN_CXX_DECL_OVERRIDE;

public: // Key management

  virtual bool
  hasKey(const Name& identity, const name::Component& keyId) const NDN_CXX_DECL_OVERRIDE;

  virtual void
  addKey(const Name& identity, const name::Component& keyId, const PublicKey& publicKey) NDN_CXX_DECL_OVERRIDE;

  virtual void
  removeKey(const Name& identity, const name::Component& keyId) NDN_CXX_DECL_OVERRIDE;

  virtual PublicKey
  getKeyBits(const Name& identity, const name::Component& keyId) const NDN_CXX_DECL_OVERRIDE;

  virtual std::set<name::Component>
  getKeysOfIdentity(const Name& identity) const NDN_CXX_DECL_OVERRIDE;

  virtual void
  setDefaultKeyOfIdentity(const Name& identity, const name::Component& keyId) NDN_CXX_DECL_OVERRIDE;

  virtual name::Component
  getDefaultKeyOfIdentity(const Name& identity) const NDN_CXX_DECL_OVERRIDE;

public: // Certificate management

  virtual bool
  hasCertificate(const Name& certName) const NDN_CXX_DECL_OVERRIDE;

  virtual void
  addCertificate(const IdentityCertificate& certificate) NDN_CXX_DECL_OVERRIDE;

  virtual void
  removeCertificate(const Name& certName) NDN_CXX_DECL_OVERRIDE;

  virtual IdentityCertificate
  getCertificate(const Name& certName) const NDN_CXX_DECL_OVERRIDE;

  virtual std::set<Name>
  getCertificatesOfKey(const Name& identity, const name::Component& keyId) const NDN_CXX_DECL_OVERRIDE;

  virtual void
  setDefaultCertificateOfKey(const Name& identity, const name::Component& keyId, const Name& certName) NDN_CXX_DECL_OVERRIDE;

  virtual IdentityCertificate
  getDefaultCertificateOfKey(const Name& identity, const name::Component& keyId) const NDN_CXX_DECL_OVERRIDE;

private: // Key management

  Name
  getKeyName(const Name& identity, const name::Component& keyId) const;

private:

  std::set<Name> m_identities;
  bool m_hasDefaultIdentity;
  Name m_defaultIdentity;

  /// @brief keyName => keyBits
  std::map<Name, PublicKey> m_keys;

  /// @brief identity => default key Name
  std::map<Name, Name> m_defaultKey;

  /// @brief certificate Name => certificate
  std::map<Name, IdentityCertificate> m_certs;

  /// @brief keyName => default certificate Name
  std::map<Name, Name> m_defaultCert;
};

} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_PIB_MEMORY_HPP
