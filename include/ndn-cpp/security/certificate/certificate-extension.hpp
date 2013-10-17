/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_CERTIFICATE_EXTENSION_HPP
#define NDN_CERTIFICATE_EXTENSION_HPP

#include "../../common.hpp"
#include "../../util/blob.hpp"
#include "../../encoding/oid.hpp"

namespace ndn {

namespace der { class DerNode; }

/**
 * A CertificateExtension represents the Extension entry in a certificate.
 */
class CertificateExtension
{
public:
  /**
   * Create a new CertificateExtension.
   * @param oid The oid of subject description entry.
   * @param isCritical If true, the extension must be handled.
   * @param value The extension value.
   */
  CertificateExtension(const std::string& oid, const bool isCritical, const Blob& value)
  : extensionId_(oid), isCritical_(isCritical), extensionValue_(value)
  {
  }

  /**
   * Create a new CertificateExtension.
   * @param oid The oid of subject description entry.
   * @param isCritical If true, the extension must be handled.
   * @param value The extension value.
   */
  CertificateExtension(const OID& oid, const bool isCritical, const Blob& value)
  : extensionId_(oid), isCritical_(isCritical), extensionValue_(value)
  {
  }

  /**
   * The virtual destructor.
   */
  virtual
  ~CertificateExtension() {}

  /**
   * encode the object into DER syntax tree
   * @return the encoded DER syntax tree
   */
  ptr_lib::shared_ptr<der::DerNode> 
  toDer();

  Blob
  toDerBlob();

  inline const OID& 
  getOid() const { return extensionId_; }

  inline const bool 
  getIsCritical() const { return isCritical_; }

  inline const Blob& 
  getValue() const { return extensionValue_; }
    
protected:
  OID extensionId_;
  bool isCritical_;
  Blob extensionValue_;
};

}

#endif
