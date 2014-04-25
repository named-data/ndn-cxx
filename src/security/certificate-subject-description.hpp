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
 * @author Jeff Thompson <jefft0@remap.ucla.edu>
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 */

#ifndef NDN_SECURITY_CERTIFICATE_SUBJECT_DESCRIPTION_HPP
#define NDN_SECURITY_CERTIFICATE_SUBJECT_DESCRIPTION_HPP

#include "../common.hpp"
#include "../encoding/oid.hpp"

namespace CryptoPP {
class BufferedTransformation;
}

namespace ndn {

/**
 * A CertificateSubjectDescription represents the SubjectDescription entry in a Certificate.
 */
class CertificateSubjectDescription
{
public:
  CertificateSubjectDescription(CryptoPP::BufferedTransformation& in)
  {
    decode(in);
  }

  /**
   * Create a new CertificateSubjectDescription.
   * @param oid The oid of the subject description entry.
   * @param value The value of the subject description entry.
   */
  CertificateSubjectDescription(const OID& oid, const std::string& value)
  : m_oid(oid), m_value(value)
  {
  }

  void
  encode(CryptoPP::BufferedTransformation& out) const;

  void
  decode(CryptoPP::BufferedTransformation& in);

  std::string
  getOidString() const
  {
    return m_oid.toString();
  }

  const std::string&
  getValue() const
  {
    return m_value;
  }

private:
  OID m_oid;
  std::string m_value;
};

} // namespace ndn

#endif //NDN_SECURITY_CERTIFICATE_SUBJECT_DESCRIPTION_HPP
