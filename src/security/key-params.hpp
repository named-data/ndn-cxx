/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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

#ifndef NDN_SECURITY_KEY_PARAMS_HPP
#define NDN_SECURITY_KEY_PARAMS_HPP

#include "../common.hpp"
#include "security-common.hpp"

namespace ndn {

/**
 * @brief Base class of key parameters.
 *
 * Its subclasses are used to store parameters for key generation.
 */
class KeyParams
{
public:
  class Error : public std::runtime_error
  {
  public:
    explicit
    Error(const std::string& what)
      : std::runtime_error(what)
    {
    }
  };

  virtual
  ~KeyParams()
  {
  }

  KeyType
  getKeyType() const
  {
    return m_keyType;
  }

protected:
  explicit
  KeyParams(KeyType keyType)
    : m_keyType(keyType)
  {
  }

private:
  KeyType m_keyType;
};


/// @brief RsaKeyParamInfo is used to initialize a SimplePublicKeyParams template for RSA key.
class RsaKeyParamsInfo
{
public:
  static KeyType
  getType()
  {
    return KEY_TYPE_RSA;
  }

  /// @brief check if size is qualified, otherwise return the default key size.
  static uint32_t
  checkKeySize(uint32_t size);

  static uint32_t
  getDefaultSize();
};

/// @brief EcdsaKeyParamInfo is used to initialize a SimplePublicKeyParams template for ECDSA key.
class EcdsaKeyParamsInfo
{
public:
  static KeyType
  getType()
  {
    return KEY_TYPE_ECDSA;
  }

  /// @brief check if size is qualified, otherwise return the default key size.
  static uint32_t
  checkKeySize(uint32_t size);

  static uint32_t
  getDefaultSize();
};


/// @brief SimplePublicKeyParams is a template for public keys with only one parameter: size.
template<typename KeyParamsInfo>
class SimplePublicKeyParams : public KeyParams
{
public:
  explicit
  SimplePublicKeyParams(uint32_t size = KeyParamsInfo::getDefaultSize())
    : KeyParams(KeyParamsInfo::getType())
  {
    setKeySize(size);
  }

  explicit
  SimplePublicKeyParams(const SimplePublicKeyParams& params)
    : KeyParams(params)
    , m_size(params.m_size)
  {
  }

  explicit
  SimplePublicKeyParams(const KeyParams& params)
    : KeyParams(params.getKeyType())
  {
    BOOST_THROW_EXCEPTION(KeyParams::Error("Incorrect key parameters (incompatible key type)"));
  }

  uint32_t
  getKeySize() const
  {
    return m_size;
  }

private:
  void
  setKeySize(uint32_t size)
  {
    m_size = KeyParamsInfo::checkKeySize(size);
  }

  uint32_t
  getDefaultKeySize() const
  {
    return KeyParamsInfo::getDefaultSize();
  }

private:
  uint32_t m_size;
};

/// @brief RsaKeyParams carries parameters for RSA key.
typedef SimplePublicKeyParams<RsaKeyParamsInfo> RsaKeyParams;

/// @brief EcdsaKeyParams carries parameters for ECDSA key.
typedef SimplePublicKeyParams<EcdsaKeyParamsInfo> EcdsaKeyParams;


/// @brief AesKeyParamsInfo is used to initialize a SimpleSymmetricKeyParams template for AES key.
class AesKeyParamsInfo
{
public:
  static KeyType
  getType()
  {
    return KEY_TYPE_AES;
  }

  /// @brief check if size is qualified, otherwise return the default key size.
  static uint32_t
  checkKeySize(uint32_t size);

  static uint32_t
  getDefaultSize();
};


/// @brief SimpleSymmetricKeyParams is a template for symmetric keys with only one parameter: size.
template<typename KeyParamsInfo>
class SimpleSymmetricKeyParams : public KeyParams
{
public:
  explicit
  SimpleSymmetricKeyParams(uint32_t size = KeyParamsInfo::getDefaultSize())
    : KeyParams(KeyParamsInfo::getType())
  {
    setKeySize(size);
  }

  explicit
  SimpleSymmetricKeyParams(const SimpleSymmetricKeyParams& params)
    : KeyParams(params)
    , m_size(params.m_size)
  {
  }

  explicit
  SimpleSymmetricKeyParams(const KeyParams& params)
  {
    BOOST_THROW_EXCEPTION(KeyParams::Error("Incorrect key parameters (incompatible key type)"));
  }

  uint32_t
  getKeySize() const
  {
    return m_size;
  }

private:
  void
  setKeySize(uint32_t size)
  {
    m_size = KeyParamsInfo::checkKeySize(size);
  }

  uint32_t
  getDefaultKeySize() const
  {
    return KeyParamsInfo::getDefaultSize();
  }

private:
  uint32_t m_size;

};

typedef SimpleSymmetricKeyParams<AesKeyParamsInfo> AesKeyParams;

} // namespace ndn

#endif // NDN_SECURITY_KEY_PARAMS_HPP
