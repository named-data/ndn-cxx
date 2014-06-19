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
  explicit
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
