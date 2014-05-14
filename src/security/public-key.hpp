/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 *
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 * @author Jeff Thompson <jefft0@remap.ucla.edu>
 */

#ifndef NDN_SECURITY_PUBLIC_KEY_HPP
#define NDN_SECURITY_PUBLIC_KEY_HPP

#include "../common.hpp"

#include "../encoding/buffer.hpp"
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

  inline const Buffer&
  get() const
  {
    return m_key;
  }

  inline void
  set(const uint8_t* keyDerBuf, size_t keyDerSize)
  {
    Buffer buf(keyDerBuf, keyDerSize);
    m_key.swap(buf);
  }

  void
  encode(CryptoPP::BufferedTransformation& out) const;

  void
  decode(CryptoPP::BufferedTransformation& in);

  inline bool
  operator==(const PublicKey& key) const
  {
    return m_key == key.m_key;
  }

  inline bool
  operator!=(const PublicKey& key) const
  {
    return m_key != key.m_key;
  }

private:
  Buffer m_key;
};

std::ostream&
operator<<(std::ostream& os, const PublicKey& key);

} // namespace ndn

#endif //NDN_SECURITY_PUBLIC_KEY_HPP
