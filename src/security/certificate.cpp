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

#include "common.hpp"

#include "certificate.hpp"
#include "../util/time.hpp"
#include "cryptopp.hpp"
#include "../encoding/cryptopp/asn_ext.hpp"
#include "../encoding/buffer-stream.hpp"
#include "../util/concepts.hpp"
#include "../util/indented-stream.hpp"

#include <boost/algorithm/string/split.hpp>

namespace ndn {

BOOST_CONCEPT_ASSERT((WireEncodable<Certificate>));
BOOST_CONCEPT_ASSERT((WireDecodable<Certificate>));
static_assert(std::is_base_of<tlv::Error, Certificate::Error>::value,
              "Certificate::Error must inherit from tlv::Error");

Certificate::Certificate()
  : m_notBefore(time::system_clock::TimePoint::max())
  , m_notAfter(time::system_clock::TimePoint::min())
{
}

Certificate::Certificate(const Data& data)
  // Use the copy constructor.  It clones the signature object.
  : Data(data)
{
  decode();
}

Certificate::Certificate(const Block& block)
  : Data(block)
{
  decode();
}

Certificate::~Certificate()
{
}

void
Certificate::wireDecode(const Block& wire)
{
  Data::wireDecode(wire);
  decode();
}

bool
Certificate::isTooEarly()
{
  if (time::system_clock::now() < m_notBefore)
    return true;
  else
    return false;
}

bool
Certificate::isTooLate()
{
  if (time::system_clock::now() > m_notAfter)
    return true;
  else
    return false;
}

void
Certificate::encode()
{
  // Name
  //    <key_name>/ID-CERT/<id#>
  // Content
  // DER encoded idCert:
  //
  //    idCert ::= SEQUENCE {
  //        validity            Validity,
  //        subject             Name,
  //        subjectPubKeyInfo   SubjectPublicKeyInfo,
  //        extension           Extensions OPTIONAL   }
  //
  //    Validity ::= SEQUENCE {
  //        notBefore           Time,
  //        notAfter            Time   }
  //
  //    Name ::= CHOICE {
  //        RDNSequence   }
  //
  //    RDNSequence ::= SEQUENCE OF RelativeDistinguishedName
  //
  //    RelativeDistinguishedName ::=
  //        SET OF AttributeTypeAndValue
  //
  //    SubjectPublicKeyInfo ::= SEQUENCE {
  //        algorithm           AlgorithmIdentifier
  //        keybits             BIT STRING   }
  //
  //    Extensions ::= SEQUENCE SIZE (1..MAX) OF Extension
  //
  // (see http://www.ietf.org/rfc/rfc3280.txt for more detail)
  //
  // KeyLocator
  //    issuer’s certificate name
  // Signature

  using namespace CryptoPP;

  OBufferStream os;
  CryptoPP::FileSink sink(os);

  // idCert ::= SEQUENCE {
  //     validity            Validity,
  //     subject             Name,
  //     subjectPubKeyInfo   SubjectPublicKeyInfo,
  //     extension           Extensions OPTIONAL   }
  DERSequenceEncoder idCert(sink);
  {
    // Validity ::= SEQUENCE {
    //       notBefore           Time,
    //       notAfter            Time   }
    DERSequenceEncoder validity(idCert);
    {
      DEREncodeGeneralTime(validity, m_notBefore);
      DEREncodeGeneralTime(validity, m_notAfter);
    }
    validity.MessageEnd();

    // Name ::= CHOICE {
    //     RDNSequence   }
    //
    // RDNSequence ::= SEQUENCE OF RelativeDistinguishedName
    DERSequenceEncoder name(idCert);
    {
      for (SubjectDescriptionList::iterator it = m_subjectDescriptionList.begin();
           it != m_subjectDescriptionList.end(); ++it)
        {
          it->encode(name);
        }
    }
    name.MessageEnd();

    // SubjectPublicKeyInfo
    m_key.encode(idCert);

    // Extensions ::= SEQUENCE SIZE (1..MAX) OF Extension
    //
    // Extension ::= SEQUENCE {
    //        extnID      OBJECT IDENTIFIER,
    //        critical    BOOLEAN DEFAULT FALSE,
    //        extnValue   OCTET STRING  }
    if (!m_extensionList.empty())
      {
        DERSequenceEncoder extensions(idCert);
        {
          for (ExtensionList::iterator it = m_extensionList.begin();
               it != m_extensionList.end(); ++it)
            {
              it->encode(extensions);
            }
        }
        extensions.MessageEnd();
      }
  }

  idCert.MessageEnd();

  setContent(os.buf());
  setContentType(tlv::ContentType_Key);
}

void
Certificate::decode()
{
  using namespace CryptoPP;

  try {
    OBufferStream os;
    StringSource source(getContent().value(), getContent().value_size(), true);

    // idCert ::= SEQUENCE {
    //     validity            Validity,
    //     subject             Name,
    //     subjectPubKeyInfo   SubjectPublicKeyInfo,
    //     extension           Extensions OPTIONAL   }
    BERSequenceDecoder idCert(source);
    {
      // Validity ::= SEQUENCE {
      //       notBefore           Time,
      //       notAfter            Time   }
      BERSequenceDecoder validity(idCert);
      {
        BERDecodeTime(validity, m_notBefore);
        BERDecodeTime(validity, m_notAfter);
      }
      validity.MessageEnd();

      // Name ::= CHOICE {
      //     RDNSequence   }
      //
      // RDNSequence ::= SEQUENCE OF RelativeDistinguishedName
      m_subjectDescriptionList.clear();
      BERSequenceDecoder name(idCert);
      {
        while (!name.EndReached())
          {
            m_subjectDescriptionList.push_back(CertificateSubjectDescription(name));
          }
      }
      name.MessageEnd();

      // SubjectPublicKeyInfo ::= SEQUENCE {
      //     algorithm           AlgorithmIdentifier
      //     keybits             BIT STRING   }
      m_key.decode(idCert);

      // Extensions ::= SEQUENCE SIZE (1..MAX) OF Extension
      //
      // Extension ::= SEQUENCE {
      //        extnID      OBJECT IDENTIFIER,
      //        critical    BOOLEAN DEFAULT FALSE,
      //        extnValue   OCTET STRING  }
      m_extensionList.clear();
      if (!idCert.EndReached())
        {
          BERSequenceDecoder extensions(idCert);
          {
            while (!extensions.EndReached())
              {
                m_extensionList.push_back(CertificateExtension(extensions));
              }
          }
          extensions.MessageEnd();
        }
    }

    idCert.MessageEnd();
  }
  catch (CryptoPP::BERDecodeErr&) {
    BOOST_THROW_EXCEPTION(Error("Certificate Decoding Error"));
  }
}

void
Certificate::printCertificate(std::ostream& oss, const std::string& indent) const
{
  util::IndentedStream os(oss, indent);

  os << "Certificate name:\n";
  os << "  " << getName() << "\n";
  os << "Validity:\n";
  {
    os << "  NotBefore: " << time::toIsoString(m_notBefore) << "\n";
    os << "  NotAfter: "  << time::toIsoString(m_notAfter)  << "\n";
  }

  os << "Subject Description:\n";
  for (const auto& description : m_subjectDescriptionList)
    os << "  " << description.getOidString() << ": " << description.getValue() << "\n";

  os << "Public key bits: ";
  switch (m_key.getKeyType()) {
  case KEY_TYPE_RSA:
    os << "(RSA)";
    break;
  case KEY_TYPE_ECDSA:
    os << "(ECDSA)";
    break;
  default:
    os << "(Unknown key type)";
    break;
  }
  os << "\n";

  {
    util::IndentedStream os2(os, "  ");
    CryptoPP::Base64Encoder encoder(new CryptoPP::FileSink(os2), true, 64);
    m_key.encode(encoder);
  }

  os << "Signature Information:\n";
  {
    os << "  Signature Type: ";
    switch (getSignature().getType()) {
    case tlv::SignatureTypeValue::DigestSha256:
      os << "DigestSha256";
      break;
    case tlv::SignatureTypeValue::SignatureSha256WithRsa:
      os << "SignatureSha256WithRsa";
      break;
    case tlv::SignatureTypeValue::SignatureSha256WithEcdsa:
      os << "SignatureSha256WithEcdsa";
      break;
    default:
      os << "Unknown Signature Type";
    }
    os << "\n";

    if (getSignature().hasKeyLocator()) {
      const KeyLocator& keyLocator = getSignature().getKeyLocator();
      os << "  Key Locator: ";
      switch (keyLocator.getType()) {
      case KeyLocator::KeyLocator_Name:
        {
          const Name& signerName = keyLocator.getName();
          if (signerName.isPrefixOf(getName()))
            os << "(Self-Signed) " << keyLocator.getName();
          else
            os << "(Name) " << keyLocator.getName();
          break;
        }
      case KeyLocator::KeyLocator_KeyDigest:
        os << "(KeyDigest)";
        break;
      case KeyLocator::KeyLocator_None:
        os << "None";
        break;
      default:
        os << "Unknown";
      }
      os << "\n";
    }
  }
}

std::ostream&
operator<<(std::ostream& os, const Certificate& cert)
{
  cert.printCertificate(os);
  return os;
}


} // namespace ndn
