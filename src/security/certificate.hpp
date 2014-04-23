/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SECURITY_CERTIFICATE_HPP
#define NDN_SECURITY_CERTIFICATE_HPP

#include "../common.hpp"
#include "../data.hpp"
#include "certificate-subject-description.hpp"
#include "certificate-extension.hpp"
#include "public-key.hpp"

namespace ndn {

class Certificate : public Data
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

  typedef std::vector<CertificateSubjectDescription> SubjectDescriptionList;
  typedef std::vector<CertificateExtension> ExtensionList;

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

  inline void
  wireDecode(const Block& wire);

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
  addSubjectDescription(const CertificateSubjectDescription& description)
  {
    m_subjectDescriptionList.push_back(description);
  }

  const SubjectDescriptionList&
  getSubjectDescriptionList() const
  {
    return m_subjectDescriptionList;
  }

  SubjectDescriptionList&
  getSubjectDescriptionList()
  {
    return m_subjectDescriptionList;
  }

  /**
   * Add a certificate extension.
   * @param extension the extension to be added
   */
  void
  addExtension(const CertificateExtension& extension)
  {
    m_extensionList.push_back(extension);
  }

  const ExtensionList&
  getExtensionList() const
  {
    return m_extensionList;
  }

  ExtensionList&
  getExtensionList()
  {
    return m_extensionList;
  }

  void
  setNotBefore(const time::system_clock::TimePoint& notBefore)
  {
    m_notBefore = notBefore;
  }

  time::system_clock::TimePoint&
  getNotBefore()
  {
    return m_notBefore;
  }

  const time::system_clock::TimePoint&
  getNotBefore() const
  {
    return m_notBefore;
  }

  void
  setNotAfter(const time::system_clock::TimePoint& notAfter)
  {
    m_notAfter = notAfter;
  }

  time::system_clock::TimePoint&
  getNotAfter()
  {
    return m_notAfter;
  }

  const time::system_clock::TimePoint&
  getNotAfter() const
  {
    return m_notAfter;
  }

  void
  setPublicKeyInfo(const PublicKey& key)
  {
    m_key = key;
  }

  PublicKey&
  getPublicKeyInfo()
  {
    return m_key;
  }

  const PublicKey&
  getPublicKeyInfo() const
  {
    return m_key;
  }

  // virtual Name
  // getPublicKeyName() const = 0;

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
  printCertificate(std::ostream& os) const;

protected:
  void
  decode();

protected:
  SubjectDescriptionList m_subjectDescriptionList;
  time::system_clock::TimePoint m_notBefore;
  time::system_clock::TimePoint m_notAfter;
  PublicKey m_key;
  ExtensionList m_extensionList;
};

inline void
Certificate::wireDecode(const Block& wire)
{
  Data::wireDecode(wire);
  decode();
}


inline std::ostream&
operator<<(std::ostream& os, const Certificate& cert)
{
  cert.printCertificate(os);
  return os;
}

} // namespace ndn

#endif //NDN_SECURITY_CERTIFICATE_HPP
