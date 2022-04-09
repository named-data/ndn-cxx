/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#include "ndn-cxx/data.hpp"
#include "ndn-cxx/encoding/block.hpp"
#include "ndn-cxx/encoding/buffer.hpp"

namespace ndn {
namespace security {

/**
 * @brief A secured container for sensitive information (certificate, private key)
 * @sa <a href="../specs/safe-bag.html">SafeBag Format</a>
 */
class SafeBag
{
public:
  /**
   * @brief Create a new empty SafeBag object
   */
  SafeBag();

  /**
   * @brief Create a new SafeBag object from a TLV block
   */
  explicit
  SafeBag(const Block& wire);

  /**
   * @brief Create a new SafeBag object with the given certificate and private key
   *
   * @param certificate The certificate data packet
   * @param encryptedKey A buffer with the private key in PKCS #8 format
   */
  SafeBag(const Data& certificate, span<const uint8_t> encryptedKey);

  /**
   * @brief Fast encoding or block size estimation
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  /**
   * @brief Encode to wire format
   */
  const Block&
  wireEncode() const;

  /**
   * @brief Decode the input from wire format
   */
  void
  wireDecode(const Block& wire);

public: // accessors
  /**
   * @brief Get the certificate data packet from safe bag
   */
  const Data&
  getCertificate() const
  {
    return m_certificate;
  }

  /**
   * @brief Get the private key in PKCS #8 format from safe bag
   */
  span<const uint8_t>
  getEncryptedKey() const
  {
    return m_encryptedKey;
  }

private:
  Data m_certificate;
  Buffer m_encryptedKey;

  mutable Block m_wire;
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(SafeBag);

} // namespace security
} // namespace ndn

#endif // NDN_CXX_SECURITY_SAFE_BAG_HPP
