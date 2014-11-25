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
  class Error : public Certificate::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : Certificate::Error(what)
    {
    }
  };

  /**
   * @brief The default constructor.
   */
  IdentityCertificate();

  /**
   * @brief Create an IdentityCertificate from the content in the data packet.
   * @param data The data packet with the content to decode.
   */
  explicit
  IdentityCertificate(const Data& data);

  /**
   * @brief Create an IdentityCertificate from a block.
   * @param block The raw block of the certificate.
   */
  explicit
  IdentityCertificate(const Block& block);

  void
  wireDecode(const Block& wire);

  void
  setName(const Name& name);

  const Name&
  getPublicKeyName() const
  {
    return m_publicKeyName;
  }

  static bool
  isIdentityCertificate(const Certificate& certificate);

  /**
   * @brief Get the public key name from the full certificate name.
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

} // namespace ndn

#endif // NDN_SECURITY_IDENTITY_CERTIFICATE_HPP
