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
 *
 * @author Zhiyi Zhang <dreamerbarrychang@gmail.com>
 */
#ifndef NDN_CXX_SECURITY_SAFE_BAG_HPP
#define NDN_CXX_SECURITY_SAFE_BAG_HPP

#include "../common.hpp"
#include "../data.hpp"
#include "../encoding/block.hpp"
#include "../encoding/buffer.hpp"
#include "security-common.hpp"

namespace ndn {
namespace security {

/** @brief a secured container for sensitive information(certificate, private key)
 */
class SafeBag
{
public:
  /**
   * @brief Create a new empty SafeBag object
   */
  SafeBag();

  /**
   * @brief Create a new SafeBag object from the block
   */
  explicit
  SafeBag(const Block& wire);

  /**
   * @brief Create a new Safe object with the given certificate and private key
   *
   * @param certificate A reference to the certificate data packet
   * @param encryptedKeyBag A reference to the Buffer of private key in PKCS#8
   */
  SafeBag(const Data& certificate,
          const Buffer& encryptedKeyBag);

  /**
   * @brief Create a new Safe object with the given certificate and private key
   *
   * @param certificate A reference to the certificate data packet
   * @param encryptedKey A reference to the uint8_t* of private key in PKCS#8
   * @param encryptedKeyLen The length of the encryptedKey
   */
  SafeBag(const Data& certificate,
          const uint8_t* encryptedKey,
          size_t encryptedKeyLen);

public:
  /**
   * @brief Fast encoding or block size estimation
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  /**
   * @brief Encode to a wire format
   */
  const Block&
  wireEncode() const;

  /**
   * @brief Decode the input from wire format
   */
  void
  wireDecode(const Block& wire);

public:
  /**
   * @brief Get the certificate data packet from safe bag
   */
  const Data&
  getCertificate() const
  {
    return m_certificate;
  }

  /**
   * @brief Get the private key in PKCS#8 from safe bag
   */
  const Buffer&
  getEncryptedKeyBag() const
  {
    return m_encryptedKeyBag;
  }

private:
  Data m_certificate;
  Buffer m_encryptedKeyBag;

  mutable Block m_wire;
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(SafeBag);

} // namespace security
} // namespace ndn

#endif // NDN_CXX_SECURITY_SAFE_BAG_HPP
