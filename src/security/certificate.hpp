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
  class Error : public Data::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : Data::Error(what)
    {
    }
  };

  typedef std::vector<CertificateSubjectDescription> SubjectDescriptionList;
  typedef std::vector<CertificateExtension> ExtensionList;

  /**
   * @brief The default constructor.
   */
  Certificate();

  /**
   * @brief Create a Certificate from the content in the data packet.
   * @param data The data packet with the content to decode.
   */
  explicit
  Certificate(const Data& data);

  /**
   * @brief Create a Certificate from the a block
   * @param block The raw block of the certificate
   */
  explicit
  Certificate(const Block& block);

  virtual
  ~Certificate();

  void
  wireDecode(const Block& wire);

  /**
   * @brief encode certificate info into content
   */
  void
  encode();

  /**
   * @brief Add a subject description.
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
   * @brief Add a certificate extension.
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

  /**
   * @brief Check if the certificate is valid.
   * @return True if the current time is earlier than notBefore.
   */
  bool
  isTooEarly();

  /**
   * @brief Check if the certificate is valid.
   * @return True if the current time is later than notAfter.
   */
  bool
  isTooLate();

  void
  printCertificate(std::ostream& os, const std::string& indent = "") const;

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

std::ostream&
operator<<(std::ostream& os, const Certificate& cert);
} // namespace ndn

#endif // NDN_SECURITY_CERTIFICATE_HPP
