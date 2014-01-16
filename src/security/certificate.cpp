/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreorder"
#pragma clang diagnostic ignored "-Wtautological-compare"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-function"
#elif __GNUC__
#pragma GCC diagnostic ignored "-Wreorder"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

#include <ndn-cpp/common.hpp>

#include <ndn-cpp/security/certificate.hpp>

#include "../util/logging.hpp"
#include "../util/time.hpp"

#include <cryptopp/asn.h>
#include <cryptopp/base64.h>
#include <cryptopp/files.h>

#include "../encoding/cryptopp/asn_ext.hpp"

INIT_LOGGER("ndn.security.Certificate");

using namespace std;

namespace ndn {

Certificate::Certificate()
  : notBefore_(std::numeric_limits<MillisecondsSince1970>::max())
  , notAfter_(std::numeric_limits<MillisecondsSince1970>::min())
{}

Certificate::Certificate(const Data& data)
// Use the copy constructor.  It clones the signature object.
: Data(data)
{
  // _LOG_DEBUG("Finish local copy: " << getContent().getContent().size());

  decode();
}

Certificate::~Certificate()
{
  //TODO:
}

bool
Certificate::isTooEarly()
{
  MillisecondsSince1970 now = ndn_getNowMilliseconds();
  if(now < notBefore_)
    return true;
  else
    return false;
}

bool 
Certificate::isTooLate()
{
  MillisecondsSince1970 now = ndn_getNowMilliseconds();
  if(now > notAfter_)
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
  // 	idCert ::= SEQUENCE {
  //        validity            Validity,
  // 	    subject             Name,
  // 	    subjectPubKeyInfo   SubjectPublicKeyInfo,
  // 	    extension           Extensions OPTIONAL   }
  //
  // 	Validity ::= SEQUENCE {
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
  // 	SubjectPublicKeyInfo ::= SEQUENCE {
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
      DEREncodeGeneralTime(validity, notBefore_);
      DEREncodeGeneralTime(validity, notAfter_);
    }
    validity.MessageEnd();

    // Name ::= CHOICE {
    //     RDNSequence   }
    // 
    // RDNSequence ::= SEQUENCE OF RelativeDistinguishedName
    DERSequenceEncoder name(idCert);
    {
      for(SubjectDescriptionList::iterator it = subjectDescriptionList_.begin();
          it != subjectDescriptionList_.end(); ++it)
        {
          it->encode(name);
        }
    }
    name.MessageEnd();
  
    // SubjectPublicKeyInfo
    key_.encode(idCert);

    // Extensions ::= SEQUENCE SIZE (1..MAX) OF Extension
    //
    // Extension ::= SEQUENCE {
    //        extnID      OBJECT IDENTIFIER,
    //        critical    BOOLEAN DEFAULT FALSE,
    //        extnValue   OCTET STRING  }
    if(!extensionList_.empty())
      {
        DERSequenceEncoder extensions(idCert);
        {
          
          for(ExtensionList::iterator it = extensionList_.begin();
              it != extensionList_.end(); ++it)
            {
              it->encode(extensions);
            }
        }
        extensions.MessageEnd();
      }
  }

  idCert.MessageEnd();

  setContent(os.buf());
  setContentType(MetaInfo::TYPE_KEY);
}

void 
Certificate::decode()
{
  using namespace CryptoPP;

  OBufferStream os;
  CryptoPP::StringSource source(getContent().value(), getContent().value_size(), true);
  
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
      BERDecodeTime(validity, notBefore_);
      BERDecodeTime(validity, notAfter_);
    }
    validity.MessageEnd();

    // Name ::= CHOICE {
    //     RDNSequence   }
    // 
    // RDNSequence ::= SEQUENCE OF RelativeDistinguishedName
    subjectDescriptionList_.clear();
    BERSequenceDecoder name(idCert);
    {
      while(!name.EndReached())
        {
          subjectDescriptionList_.push_back(CertificateSubjectDescription(name));
        }
    }
    name.MessageEnd();
  
    // SubjectPublicKeyInfo ::= SEQUENCE {
    //     algorithm           AlgorithmIdentifier
    //     keybits             BIT STRING   }
    key_.decode(idCert);

    // Extensions ::= SEQUENCE SIZE (1..MAX) OF Extension
    //
    // Extension ::= SEQUENCE {
    //        extnID      OBJECT IDENTIFIER,
    //        critical    BOOLEAN DEFAULT FALSE,
    //        extnValue   OCTET STRING  }
    extensionList_.clear();
    if(!idCert.EndReached())
      {
        BERSequenceDecoder extensions(idCert);
        {
          while(!extensions.EndReached())
            {
              extensionList_.push_back(CertificateExtension(extensions));
            }
        }
        extensions.MessageEnd();
      }
  }

  idCert.MessageEnd();
}

void 
Certificate::printCertificate(std::ostream &os) const
{
  os << "Certificate name:" << endl;
  os << "  " << getName() << endl;
  os << "Validity:" << endl;
  {
    os << "  NotBefore: " << toIsoString(notBefore_) << endl;
    os << "  NotAfter: "  << toIsoString(notAfter_)  << endl;
  }

  os << "Subject Description:" << endl;  
  for(SubjectDescriptionList::const_iterator it = subjectDescriptionList_.begin();
      it != subjectDescriptionList_.end(); ++it)
    {
      os << "  " << it->getOidString() << ": " << it->getValue() << endl;
    }

  os << "Public key bits:" << endl;
  CryptoPP::Base64Encoder encoder(new CryptoPP::FileSink(os), true, 64);
  key_.encode(encoder);
  
  // ndnboost::iostreams::stream<ndnboost::iostreams::array_source> is((const char*)key_.getKeyDer().buf(), key_.getKeyDer().size());

  // ptr_lib::shared_ptr<der::DerNode> keyRoot = der::DerNode::parse(reinterpret_cast<der::InputIterator&> (is));

  // der::PrintVisitor printVisitor;
  // keyRoot->accept(printVisitor, string(""));
}

}
