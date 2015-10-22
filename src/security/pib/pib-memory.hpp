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

#ifndef NDN_SECURITY_PIB_PIB_MEMORY_HPP
#define NDN_SECURITY_PIB_PIB_MEMORY_HPP

#include "pib-impl.hpp"

namespace ndn {
namespace security {
namespace pib {

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
  /**
   * @brief Create memory based PIB backend
   * @param location Not used (required by the PIB-registration interface)
   */
  explicit
  PibMemory(const std::string& location = "");

  static const std::string&
  getScheme();

public: // TpmLocator management
  void
  setTpmLocator(const std::string& tpmLocator) override;

  std::string
  getTpmLocator() const override;

public: // Identity management
  bool
  hasIdentity(const Name& identity) const override;

  void
  addIdentity(const Name& identity) override;

  void
  removeIdentity(const Name& identity) override;

  void
  clearIdentities() override;

  std::set<Name>
  getIdentities() const override;

  void
  setDefaultIdentity(const Name& identityName) override;

  Name
  getDefaultIdentity() const override;

public: // Key management
  bool
  hasKey(const Name& keyName) const override;

  void
  addKey(const Name& identity, const Name& keyName, const uint8_t* key, size_t keyLen) override;

  void
  removeKey(const Name& keyName) override;

  Buffer
  getKeyBits(const Name& keyName) const override;

  std::set<Name>
  getKeysOfIdentity(const Name& identity) const override;

  void
  setDefaultKeyOfIdentity(const Name& identity, const Name& keyName) override;

  Name
  getDefaultKeyOfIdentity(const Name& identity) const override;

public: // Certificate management
  bool
  hasCertificate(const Name& certName) const override;

  void
  addCertificate(const v2::Certificate& certificate) override;

  void
  removeCertificate(const Name& certName) override;

  v2::Certificate
  getCertificate(const Name& certName) const override;

  std::set<Name>
  getCertificatesOfKey(const Name& keyName) const override;

  void
  setDefaultCertificateOfKey(const Name& keyName, const Name& certName) override;

  v2::Certificate
  getDefaultCertificateOfKey(const Name& keyName) const override;

private:
  std::string m_tpmLocator;

  bool m_hasDefaultIdentity;
  Name m_defaultIdentity;

  std::set<Name> m_identities;

  /// @brief identity => default key Name
  std::map<Name, Name> m_defaultKeys;

  /// @brief keyName => keyBits
  std::map<Name, Buffer> m_keys;

  /// @brief keyName => default certificate Name
  std::map<Name, Name> m_defaultCerts;

  /// @brief certificate Name => certificate
  std::map<Name, v2::Certificate> m_certs;
};

} // namespace pib
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_PIB_PIB_MEMORY_HPP
