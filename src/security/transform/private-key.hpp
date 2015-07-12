/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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

#ifndef NDN_CXX_SECURITY_TRANSFORM_PRIVATE_KEY_HPP
#define NDN_CXX_SECURITY_TRANSFORM_PRIVATE_KEY_HPP

#include "public-key.hpp"
#include "../../encoding/buffer.hpp"

namespace ndn {

class KeyParams;

namespace security {
namespace transform {

/**
 * @brief Abstraction of private key in crypto transformation
 */
class PrivateKey : noncopyable
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

  friend class SignerFilter;

  /**
   * @brief Callback for application to handle password input
   *
   * Password should be stored in @p buf and should not be longer than @p size.  It is
   * recommended to ask the user to verify the passphrase if @p shouldConfirm is true, e.g., by
   * prompting for the password twice.
   */
  typedef function<int(char* buf, size_t bufSize, bool shouldConfirm)> PasswordCallback;

public:
  /**
   * @brief Create a private key instance
   *
   * One must call loadXXXX(...) to load private key.
   */
  PrivateKey();

  ~PrivateKey();

  /**
   * @brief Load the private key in PKCS#1 format from a buffer @p buf
   */
  void
  loadPkcs1(const uint8_t* buf, size_t size);

  /**
   * @brief Load the private key in PKCS#1 format from a stream @p is
   */
  void
  loadPkcs1(std::istream& is);

  /**
   * @brief Load the private key in base64-encoded PKCS#1 format from a buffer @p buf
   */
  void
  loadPkcs1Base64(const uint8_t* buf, size_t size);

  /**
   * @brief Load the private key in base64-encoded PKCS#1 format from a stream @p is
   */
  void
  loadPkcs1Base64(std::istream& is);

  /**
   * @brief Load the private key in encrypted PKCS#8 format from a buffer @p buf with passphrase @p pw
   *
   * @pre strlen(pw) == pwLen
   */
  void
  loadPkcs8(const uint8_t* buf, size_t size, const char* pw, size_t pwLen);

  /**
   * @brief Load the private key in encrypted PKCS#8 format from a buffer @p buf with
   *        passphrase obtained from @p pwCallback
   *
   * The default password callback is provided by OpenSSL
   */
  void
  loadPkcs8(const uint8_t* buf, size_t size, PasswordCallback pwCallback = nullptr);

  /**
   * @brief Load the private key in encrypted PKCS#8 format from a stream @p is with passphrase @p pw
   *
   * @pre strlen(pw) == pwLen
   */
  void
  loadPkcs8(std::istream& is, const char* pw, size_t pwLen);

  /**
   * @brief Load the private key in encrypted PKCS#8 format from a stream @p is with passphrase
   *        obtained from @p pwCallback
   *
   * The default password callback is provided by OpenSSL
   */
  void
  loadPkcs8(std::istream& is, PasswordCallback pwCallback = nullptr);

  /**
   * @brief Load the private key in base64-encoded encrypted PKCS#8 format from a buffer @p buf
   *        with passphrase @p pw
   *
   * @pre strlen(pw) == pwLen
   */
  void
  loadPkcs8Base64(const uint8_t* buf, size_t size, const char* pw, size_t pwLen);

  /**
   * @brief Load the private key in encrypted PKCS#8 format from a buffer @p buf with
   *        passphrase obtained from @p pwCallback
   *
   * The default password callback is provided by OpenSSL
   */
  void
  loadPkcs8Base64(const uint8_t* buf, size_t size, PasswordCallback pwCallback = nullptr);

  /**
   * @brief Load the private key in base64-encoded encrypted PKCS#8 format from a stream @p is
   *        with passphrase @p pw
   *
   * @pre strlen(pw) == pwLen
   */
  void
  loadPkcs8Base64(std::istream& is, const char* pw, size_t pwLen);

  /**
   * @brief Load the private key in base64-encoded encrypted PKCS#8 format from a stream @p is
   *        with passphrase obtained from @p pwCallback
   *
   * The default password callback is provided by OpenSSL
   */
  void
  loadPkcs8Base64(std::istream& is, PasswordCallback pwCallback = nullptr);

  /**
   * @brief Save the private key in PKCS#1 format into a stream @p os
   */
  void
  savePkcs1(std::ostream& os) const;

  /**
   * @brief Save the private key in base64-encoded PKCS#1 format into a stream @p os
   */
  void
  savePkcs1Base64(std::ostream& os) const;

  /**
   * @brief Save the private key in encrypted PKCS#8 format into a stream @p os
   */
  void
  savePkcs8(std::ostream& os, const char* pw, size_t pwLen) const;

  /**
   * @brief Save the private key in encrypted PKCS#8 format into a stream @p os with passphrase
   *        obtained from @p pwCallback
   *
   * The default password callback is provided by OpenSSL
   */
  void
  savePkcs8(std::ostream& os, PasswordCallback pwCallback = nullptr) const;

  /**
   * @brief Save the private key in base64-encoded encrypted PKCS#8 format into a stream @p os
   */
  void
  savePkcs8Base64(std::ostream& os, const char* pw, size_t pwLen) const;

  /**
   * @brief Save the private key in base64-encoded encrypted PKCS#8 format into a stream @p os
   *        with passphrase obtained from @p pwCallback
   *
   * The default password callback is provided by OpenSSL
   */
  void
  savePkcs8Base64(std::ostream& os, PasswordCallback pwCallback = nullptr) const;

  /**
   * @return Public key bits in PKCS#8 format
   */
  ConstBufferPtr
  derivePublicKey() const;

  /**
   * @return Plain text of @p cipherText decrypted using the private key.
   *
   * Only RSA encryption is supported for now.
   */
  ConstBufferPtr
  decrypt(const uint8_t* cipherText, size_t cipherLen) const;

private:
  /**
   * @return A pointer to an EVP_PKEY instance.
   *
   * One need to explicitly cast the return value to EVP_PKEY*.
   */
  void*
  getEvpPkey() const;

private:
  ConstBufferPtr
  toPkcs1() const;

  ConstBufferPtr
  toPkcs8(const char* pw, size_t pwLen) const;

  ConstBufferPtr
  toPkcs8(PasswordCallback pwCallback = nullptr) const;

  ConstBufferPtr
  rsaDecrypt(const uint8_t* cipherText, size_t cipherLen) const;

private:
  class Impl;
  unique_ptr<Impl> m_impl;
};

/**
 * @brief generate a private key according to @p keyParams.
 *
 * @note the public key can be derived from the private key
 *
 * @throw std::argument_error if the key type is not supported
 * @throw std::runtime_error when failing to generate the key
 */
unique_ptr<PrivateKey>
generatePrivateKey(const KeyParams& keyParams);

} // namespace transform
} // namespace security
} // namespace ndn

#endif // NDN_CXX_SECURITY_TRANSFORM_PRIVATE_KEY_HPP
