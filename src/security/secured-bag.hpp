/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SECURITY_SECURED_BAG_HPP
#define NDN_SECURITY_SECURED_BAG_HPP

#include "../common.hpp"
#include "identity-certificate.hpp"
#include "../encoding/tlv-security.hpp"

namespace ndn {

class SecuredBag
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

  SecuredBag()
    : m_wire(tlv::security::IdentityPackage)
  {}

  SecuredBag(const IdentityCertificate& cert,
	     ConstBufferPtr key)
    : m_cert(cert)
    , m_key(key)
    , m_wire(tlv::security::IdentityPackage)
  {
    Block wireKey(tlv::security::KeyPackage, m_key);
    Block wireCert(tlv::security::CertificatePackage, cert.wireEncode());
    m_wire.push_back(wireCert);
    m_wire.push_back(wireKey);
  }

  virtual
  ~SecuredBag()
  {}

  void
  wireDecode(const Block& wire)
  {
    m_wire = wire;
    m_wire.parse();

    m_cert.wireDecode(m_wire.get(tlv::security::CertificatePackage).blockFromValue());

    Block wireKey = m_wire.get(tlv::security::KeyPackage);
    shared_ptr<Buffer> key = make_shared<Buffer>(wireKey.value(), wireKey.value_size());
    m_key = key;
  }

  inline const Block&
  wireEncode() const
  {
    m_wire.encode();
    return m_wire;
  }

  const IdentityCertificate&
  getCertificate() const
  {
    return m_cert;
  }

  ConstBufferPtr
  getKey() const
  {
    return m_key;
  }

private:
  IdentityCertificate m_cert;
  ConstBufferPtr m_key;

  mutable Block m_wire;
};

} // namespace ndn

#endif //NDN_SECURITY_IDENTITY_CERTIFICATE_HPP
