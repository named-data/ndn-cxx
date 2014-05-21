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
 */

#ifndef NDN_SECURITY_IDENTITY_CERTIFICATE_HPP
#define NDN_SECURITY_IDENTITY_CERTIFICATE_HPP

#include "../common.hpp"
#include "certificate.hpp"

namespace ndn {

class IdentityCertificate : public Certificate
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
  IdentityCertificate();

  /**
   * Create an IdentityCertificate from the content in the data packet.
   * @param data The data packet with the content to decode.
   */
  explicit
  IdentityCertificate(const Data& data);

  /**
   * The virtual destructor.
   */
  virtual
  ~IdentityCertificate();

  void
  wireDecode(const Block& wire);

  void
  setName(const Name& name);

  const Name&
  getPublicKeyName() const;

  static bool
  isIdentityCertificate(const Certificate& certificate);

  /**
   * Get the public key name from the full certificate name.
   * @param certificateName The full certificate name.
   * @return The related public key name.
   */
  static Name
  certificateNameToPublicKeyName(const Name& certificateName);

private:
  static bool
  isCorrectName(const Name& name);

  void
  setPublicKeyName();

protected:
  Name m_publicKeyName;
};

inline
IdentityCertificate::IdentityCertificate()
{
}

inline
IdentityCertificate::IdentityCertificate(const Data& data)
  : Certificate(data)
{
  setPublicKeyName();
}

inline
IdentityCertificate::~IdentityCertificate()
{
}

inline void
IdentityCertificate::wireDecode(const Block& wire)
{
  Certificate::wireDecode(wire);
  setPublicKeyName();
}

inline void
IdentityCertificate::setName(const Name& name)
{
  Certificate::setName(name);
  setPublicKeyName();
}

inline const Name&
IdentityCertificate::getPublicKeyName() const
{
  return m_publicKeyName;
}

} // namespace ndn

#endif //NDN_SECURITY_IDENTITY_CERTIFICATE_HPP
