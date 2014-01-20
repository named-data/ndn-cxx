/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_CERTIFICATE_EXTENSION_HPP
#define NDN_CERTIFICATE_EXTENSION_HPP

#include "../common.hpp"
#include "../encoding/buffer.hpp"
#include "../encoding/oid.hpp"

namespace CryptoPP { class BufferedTransformation; }

namespace ndn {

/**
 * A CertificateExtension represents the Extension entry in a certificate.
 */
class CertificateExtension
{
public:
  struct Error : public std::runtime_error { Error(const std::string &what) : std::runtime_error(what) {} };

  CertificateExtension(CryptoPP::BufferedTransformation &in)
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
    : extensionId_(oid), isCritical_(isCritical), extensionValue_(value)
  {
  }

  CertificateExtension(const OID& oid, const bool isCritical, const uint8_t* value, size_t valueSize)
    : extensionId_(oid), isCritical_(isCritical), extensionValue_(value, valueSize)
  {
  }
  
  /**
   * The virtual destructor.
   */
  virtual
  ~CertificateExtension() {}

  void
  encode(CryptoPP::BufferedTransformation &out) const;

  void
  decode(CryptoPP::BufferedTransformation &in);
  
  inline const OID& 
  getOid() const { return extensionId_; }

  inline const bool 
  getIsCritical() const { return isCritical_; }

  inline const Buffer& 
  getValue() const { return extensionValue_; }
    
protected:
  OID extensionId_;
  bool isCritical_;
  Buffer extensionValue_;
};

}

#endif
