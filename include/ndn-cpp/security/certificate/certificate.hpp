/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_CERTIFICATE_HPP
#define NDN_CERTIFICATE_HPP

#include "../../data.hpp"
#include "../../common.hpp"

#include "certificate-subject-description.hpp"
#include "certificate-extension.hpp"
#include "public-key.hpp"

namespace ndn {

typedef std::vector<CertificateSubjectDescription> SubjectDescriptionList;
typedef std::vector<CertificateExtension> ExtensionList;

class Certificate : public Data {
public:
  /**
   * The default constructor.
   */
  Certificate();

  /**
   * Create a Certificate from the content in the data packet.
   * @param data The data packet with the content to decode.
   */
  Certificate(const Data& data);
 
  /**
   * The virtual destructor.
   */
  virtual 
  ~Certificate();

  /**
   * encode certificate info into content
   */
  void
  encode();

  /**
   * Add a subject description.
   * @param description The description to be added.
   */
  void 
  addSubjectDescription(const CertificateSubjectDescription& description) { subjectDescriptionList_.push_back(description); }
 
  /**
   * Add a certificate extension.
   * @param extension the extension to be added
   */
  void 
  addExtension(const CertificateExtension& extension) { extensionList_.push_back(extension); }

  void 
  setNotBefore(const Time& notBefore) { notBefore_ = notBefore; }

  Time& 
  getNotBefore() { return notBefore_; }
  
  const Time& 
  getNotBefore() const { return notBefore_; }

  void
  setNotAfter(const Time& notAfter) { notAfter_ = notAfter; }

  Time& 
  getNotAfter() { return notAfter_; }

  const Time& 
  getNotAfter() const { return notAfter_; }

  void
  setPublicKeyInfo(const PublicKey& key) { key_ = key; }
  
  PublicKey& 
  getPublicKeyInfo() { return key_; }

  const PublicKey& 
  getPublicKeyInfo() const { return key_; }

  virtual Name 
  getPublicKeyName() const = 0;
  
  /**
   * Check if the certificate is valid.
   * @return True if the current time is earlier than notBefore.
   */
  bool 
  isTooEarly();

  /**
   * Check if the certificate is valid.
   * @return True if the current time is later than notAfter.
   */
  bool
  isTooLate();

  void 
  printCertificate();

protected:
  void
  decode();

protected:
  SubjectDescriptionList subjectDescriptionList_;
  Time notBefore_;
  Time notAfter_;
  PublicKey key_;
  ExtensionList extensionList_;
};

}

#endif
