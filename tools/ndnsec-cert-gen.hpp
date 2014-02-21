/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 * BSD license, See the LICENSE file for more information
 * Author: Yingdi Yu <yingdi@cs.ucla.edu>
 */

#ifndef NDNSEC_CERT_GEN_HPP
#define NDNSEC_CERT_GEN_HPP

#include "ndnsec-util.hpp"

int 
ndnsec_cert_gen(int argc, char** argv)
{
  using boost::tokenizer;
  using boost::escaped_list_separator;

  using namespace ndn;
  namespace po = boost::program_options;

  typedef boost::posix_time::ptime Time;
  
  std::string notBeforeStr;
  std::string notAfterStr;
  std::string sName;
  std::string reqFile;
  std::string signId;
  std::string subInfo;
  bool isSelfSigned = false;
  bool nack = false;

  po::options_description desc("General Usage\n  ndnsec cert-gen [-h] [-S date] [-E date] [-N subject-name] [-I subject-info] [-s sign-id] request\nGeneral options");
  desc.add_options()
    ("help,h", "produce help message")
    ("not-before,S", po::value<std::string>(&notBeforeStr), "certificate starting date, YYYYMMDDhhmmss")
    ("not-after,E", po::value<std::string>(&notAfterStr), "certificate ending date, YYYYMMDDhhmmss")
    ("subject-name,N", po::value<std::string>(&sName), "subject name")
    ("subject-info,I", po::value<std::string>(&subInfo), "subject info, pairs of OID and string description: \"2.5.4.10 'University of California, Los Angeles'\"")
    ("nack", "Generate revocation certificate (NACK)")
    ("sign-id,s", po::value<std::string>(&signId), "signing Identity, self-signed if not specified")
    ("request,r", po::value<std::string>(&reqFile), "request file name, - for stdin")
    ;

  po::positional_options_description p;
  p.add("request", 1);

  po::variables_map vm;
  try
    {
      po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
      po::notify(vm);
    }
  catch (std::exception &e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl;
      return 1;
    }

  if (vm.count("help"))
    {
      std::cerr << desc << std::endl;
      return 0;
    }

  if (0 == vm.count("sign-id"))
    {     
      isSelfSigned = true;
    }

  if (vm.count("nack"))
    {
      nack = true;
    }

  std::vector<CertificateSubjectDescription> otherSubDescrypt;
  tokenizer<escaped_list_separator<char> > subInfoItems(subInfo, escaped_list_separator<char> ("\\", " \t", "'\""));

  tokenizer<escaped_list_separator<char> >::iterator it = subInfoItems.begin();
  try
    {
      while (it != subInfoItems.end())
        {
          std::string oid = *it;

          it++;
          if (it == subInfoItems.end ())
            {
              std::cerr << "ERROR: unmatched info for oid [" << oid << "]" << std::endl;
              return 1;
            }

          std::string value = *it;

          otherSubDescrypt.push_back (CertificateSubjectDescription(oid, value));

          it++;
        }
    }
  catch (std::exception &e)
    {
      std::cerr << "error in parsing subject info" << std::endl;
      return 1;
    }

  Time notBefore;
  Time notAfter;
  try{
    if (0 == vm.count("not-before"))
      {
        notBefore = boost::posix_time::second_clock::universal_time();
      }
    else
      {
        notBefore = boost::posix_time::from_iso_string(notBeforeStr.substr(0, 8) + "T" + notBeforeStr.substr(8, 6));
      }


    if (0 == vm.count("not-after"))
      {
        notAfter = notBefore + boost::posix_time::hours(24*365);
      }
    else
      {
        notAfter = boost::posix_time::from_iso_string(notAfterStr.substr(0, 8) + "T" + notAfterStr.substr(8, 6));
        if(notAfter < notBefore)
          {
            std::cerr << "not-before is later than not-after" << std::endl;
            return 1;
          }
      }
  }catch(std::exception & e){
    std::cerr << "Error in converting validity timestamp!" << std::endl;
    return 1;
  }

  if (0 == vm.count("request"))
    {
      std::cerr << "request file must be specified" << std::endl;
      return 1;
    }

  shared_ptr<IdentityCertificate> selfSignedCertificate = getIdentityCertificate(reqFile);
  if(!static_cast<bool>(selfSignedCertificate))
    {
      std::cerr << "ERROR: input error" << std::endl;
      return 1;
    }

  Name keyName = selfSignedCertificate->getPublicKeyName();
  Name signIdName;
  Name certName;

  if(isSelfSigned)
    {
      certName = keyName.getPrefix(keyName.size()-1);
      certName.append("KEY").append(keyName.get(-1)).append("ID-CERT").appendVersion();
    }
  else
    {
      signIdName = Name(signId);
  
      Name::const_iterator i = keyName.begin();
      Name::const_iterator j = signIdName.begin();
      int count = 0;
      for(; i != keyName.end() && j != signIdName.end(); i++, j++, count++)
        {
          if(*i != *j)
            break;
        }
      
      if(j != signIdName.end() || i == keyName.end())
        {
          std::cerr << "wrong signing identity!" << std::endl;
          return 1;
        }

      certName = keyName.getSubName(0, count);
      certName.append("KEY").append(keyName.getSubName(count, keyName.size()-count));
      certName.append("ID-CERT").appendVersion ();
    }

  Block wire;

  if (!nack)
    {
      if (0 == vm.count("subject-name"))
        {
          std::cerr << "subject_name must be specified" << std::endl;
          return 1;
        }

      try
        {
          CertificateSubjectDescription subDescryptName("2.5.4.41", sName);
          IdentityCertificate certificate;
          certificate.setName(certName);
          certificate.setNotBefore((notBefore-ndn::UNIX_EPOCH_TIME).total_milliseconds());
          certificate.setNotAfter((notAfter-ndn::UNIX_EPOCH_TIME).total_milliseconds());
          certificate.setPublicKeyInfo(selfSignedCertificate->getPublicKeyInfo());
          certificate.addSubjectDescription(subDescryptName);
          for(int i = 0; i < otherSubDescrypt.size(); i++)
            certificate.addSubjectDescription(otherSubDescrypt[i]);
          certificate.encode();

          KeyChain keyChain;

          if(isSelfSigned)
            keyChain.selfSign(certificate);
          else
            {
              Name signingCertificateName = keyChain.getDefaultCertificateNameForIdentity(Name(signId));
              
              keyChain.sign(certificate, signingCertificateName);
            }
          wire = certificate.wireEncode();
        }
      catch(SecPublicInfo::Error& e)
        {
          std::cerr << "ERROR: " << e.what() << std::endl;
          return 1;
        }
      catch(SecTpm::Error& e)
        {
          std::cerr << "ERROR: " << e.what() << std::endl;
          return 1;
        }
    }
  else
    {
      Data revocationCert;
      // revocationCert.setContent(void*, 0); // empty content
      revocationCert.setName(certName);

      try
        {
          KeyChain keyChain;
          
          Name signingCertificateName = keyChain.getDefaultCertificateNameForIdentity(Name(signId));
          
          keyChain.sign (revocationCert, signingCertificateName);
          wire = revocationCert.wireEncode();
        }
      catch(SecPublicInfo::Error& e)
        {
          std::cerr << "ERROR: " << e.what() << std::endl;
          return 1;
        }
      catch(SecTpm::Error& e)
        {
          std::cerr << "ERROR: " << e.what() << std::endl;
          return 1;
        }
    }

  try
    {
      using namespace CryptoPP;
      StringSource ss(wire.wire(), wire.size(), true,
                      new Base64Encoder(new FileSink(std::cout), true, 64));
    }
  catch(CryptoPP::Exception& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl;
      return 1;
    }

  return 0;
}

#endif //NDNSEC_CERT_GEN_HPP
