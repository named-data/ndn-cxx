/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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

#ifndef NDN_CXX_SECURITY_TRANSFORM_PUBLIC_KEY_HPP
#define NDN_CXX_SECURITY_TRANSFORM_PUBLIC_KEY_HPP

#include "../security-common.hpp"
#include "../../encoding/buffer.hpp"

namespace ndn {
namespace security {
namespace transform {

/**
 * @brief Abstraction of public key in crypto transformation
 */
class PublicKey : noncopyable
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

public:
  /**
   * @brief Create an empty public key instance
   *
   * One must call loadXXXX(...) to load a public key.
   */
  PublicKey();

  ~PublicKey();

  /**
   * @brief Get the type of the public key
   */
  KeyType
  getKeyType() const;

  /**
   * @brief Load the public key in PKCS#8 format from a buffer @p buf
   */
  void
  loadPkcs8(const uint8_t* buf, size_t size);

  /**
   * @brief Load the public key in PKCS#8 format from a stream @p is
   */
  void
  loadPkcs8(std::istream& is);

  /**
   * @brief Load the public key in base64-encoded PKCS#8 format from a buffer @p buf
   */
  void
  loadPkcs8Base64(const uint8_t* buf, size_t size);

  /**
   * @brief Load the public key in base64-encoded PKCS#8 format from a stream @p is
   */
  void
  loadPkcs8Base64(std::istream& is);

  /**
   * @brief Save the public key in PKCS#8 format into a stream @p os
   */
  void
  savePkcs8(std::ostream& os) const;

  /**
   * @brief Save the public key in base64-encoded PKCS#8 format into a stream @p os
   */
  void
  savePkcs8Base64(std::ostream& os) const;

  /**
   * @return Cipher text of @p plainText encrypted using this public key.
   *
   * Only RSA encryption is supported for now.
   */
  ConstBufferPtr
  encrypt(const uint8_t* plainText, size_t plainLen) const;

private:
  friend class VerifierFilter;

  /**
   * @return A pointer to an OpenSSL EVP_PKEY instance.
   *
   * The caller needs to explicitly cast the return value to `EVP_PKEY*`.
   */
  void*
  getEvpPkey() const;

private:
  ConstBufferPtr
  toPkcs8() const;

  ConstBufferPtr
  rsaEncrypt(const uint8_t* plainText, size_t plainLen) const;

private:
  class Impl;
  const unique_ptr<Impl> m_impl;
};

} // namespace transform
} // namespace security
} // namespace ndn

#endif // NDN_CXX_SECURITY_TRANSFORM_PUBLIC_KEY_HPP
