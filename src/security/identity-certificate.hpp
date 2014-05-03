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
