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
 *
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 * @author Jeff Thompson <jefft0@remap.ucla.edu>
 */

#ifndef NDN_SECURITY_PUBLIC_KEY_HPP
#define NDN_SECURITY_PUBLIC_KEY_HPP

#include "../common.hpp"

#include "../encoding/buffer.hpp"
#include "../encoding/block.hpp"
#include "security-common.hpp"

namespace CryptoPP {
class BufferedTransformation;
}

namespace ndn {

class PublicKey
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

  /**
   * The default constructor.
   */
  PublicKey();

  /**
   * @brief Create a new PublicKey from @param keyDerBuf in DER buffer
   *
   * @param keyDerBuf The pointer to the first byte of buffer containing DER of public key
   * @param keyDerSize Size of the buffer
   *
   * @throws PublicKey::Error If DER in buffer cannot be decoded
   */
  PublicKey(const uint8_t* keyDerBuf, size_t keyDerSize);

  const Buffer&
  get() const
  {
    return m_key;
  }

  void
  set(const uint8_t* keyDerBuf, size_t keyDerSize)
  {
    Buffer buf(keyDerBuf, keyDerSize);
    m_key.swap(buf);
  }

  KeyType
  getKeyType() const
  {
    return m_type;
  }

  /**
   * @return a KeyDigest block that matches this public key
   */
  const Block&
  computeDigest() const;

  void
  encode(CryptoPP::BufferedTransformation& out) const;

  void
  decode(CryptoPP::BufferedTransformation& in);

  bool
  operator==(const PublicKey& key) const
  {
    return m_key == key.m_key;
  }

  bool
  operator!=(const PublicKey& key) const
  {
    return m_key != key.m_key;
  }

private:
  KeyType m_type;
  Buffer m_key;
  mutable Block m_digest;
};

std::ostream&
operator<<(std::ostream& os, const PublicKey& key);

} // namespace ndn

#endif //NDN_SECURITY_PUBLIC_KEY_HPP
