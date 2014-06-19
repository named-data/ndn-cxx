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
 * @author Jeff Thompson <jefft0@remap.ucla.edu>
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 */

#ifndef NDN_SECURITY_CERTIFICATE_EXTENSION_HPP
#define NDN_SECURITY_CERTIFICATE_EXTENSION_HPP

#include "../common.hpp"
#include "../encoding/buffer.hpp"
#include "../encoding/oid.hpp"

namespace CryptoPP {
class BufferedTransformation;
}

namespace ndn {

/**
 * A CertificateExtension represents the Extension entry in a certificate.
 */
class CertificateExtension
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

  explicit
  CertificateExtension(CryptoPP::BufferedTransformation& in)
  {
    decode(in);
  }

  /**
   * Create a new CertificateExtension.
   * @param oid The oid of subject description entry.
   * @param isCritical If true, the extension must be handled.
   * @param value The extension value.
   */
  CertificateExtension(const OID& oid, const bool isCritical, const Buffer& value)
    : m_extensionId(oid), m_isCritical(isCritical), m_extensionValue(value)
  {
  }

  CertificateExtension(const OID& oid, const bool isCritical,
                       const uint8_t* value, size_t valueSize)
    : m_extensionId(oid), m_isCritical(isCritical), m_extensionValue(value, valueSize)
  {
  }

  /**
   * The virtual destructor.
   */
  virtual
  ~CertificateExtension()
  {
  }

  void
  encode(CryptoPP::BufferedTransformation& out) const;

  void
  decode(CryptoPP::BufferedTransformation& in);

  inline const OID&
  getOid() const
  {
    return m_extensionId;
  }

  inline bool
  getIsCritical() const
  {
    return m_isCritical;
  }

  inline const Buffer&
  getValue() const
  {
    return m_extensionValue;
  }

protected:
  OID m_extensionId;
  bool m_isCritical;
  Buffer m_extensionValue;
};

} // namespace ndn

#endif //NDN_SECURITY_CERTIFICATE_EXTENSION_HPP
