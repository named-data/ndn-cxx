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
  using namespace ndn::time;
  namespace po = boost::program_options;

  std::string notBeforeStr;
  std::string notAfterStr;
  std::string subjectName;
  std::string requestFile("-");
  std::string signId;
  std::string subjectInfo;
  bool hasSignId = false;
  bool isNack = false;

  po::options_description description("General Usage\n  ndnsec cert-gen [-h] [-S date] [-E date] [-N subject-name] [-I subject-info] [-s sign-id] request\nGeneral options");
  description.add_options()
    ("help,h", "produce help message")
    ("not-before,S", po::value<std::string>(&notBeforeStr), "certificate starting date, YYYYMMDDhhmmss")
    ("not-after,E", po::value<std::string>(&notAfterStr), "certificate ending date, YYYYMMDDhhmmss")
    ("subject-name,N", po::value<std::string>(&subjectName), "subject name")
    ("subject-info,I", po::value<std::string>(&subjectInfo), "subject info, pairs of OID and string description: \"2.5.4.10 'University of California, Los Angeles'\"")
    ("nack", "Generate revocation certificate (NACK)")
    ("sign-id,s", po::value<std::string>(&signId), "signing Identity, system default identity if not specified")
    ("request,r", po::value<std::string>(&requestFile), "request file name, - for stdin")
    ;

  po::positional_options_description p;
  p.add("request", 1);

  po::variables_map vm;
  try
    {
      po::store(po::command_line_parser(argc, argv).options(description).positional(p).run(),
                vm);
      po::notify(vm);
    }
  catch (const std::exception& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl;
      return 1;
    }

  if (vm.count("help") != 0)
    {
      std::cerr << description << std::endl;
      return 0;
    }

  if (vm.count("sign-id") != 0)
    {
      hasSignId = true;
    }

  if (vm.count("nack") != 0)
    {
      isNack = true;
    }

  std::vector<CertificateSubjectDescription> otherSubDescrypt;
  tokenizer<escaped_list_separator<char> > subjectInfoItems
    (subjectInfo, escaped_list_separator<char> ("\\", " \t", "'\""));

  tokenizer<escaped_list_separator<char> >::iterator it =
    subjectInfoItems.begin();

  while (it != subjectInfoItems.end())
    {
      std::string oid = *it;

      it++;
      if (it == subjectInfoItems.end())
        {
          std::cerr << "ERROR: unmatched info for oid [" << oid << "]" << std::endl;
          return 1;
        }

      std::string value = *it;

      otherSubDescrypt.push_back(CertificateSubjectDescription(oid, value));

      it++;
    }

  system_clock::TimePoint notBefore;
  system_clock::TimePoint notAfter;

  if (vm.count("not-before") == 0)
    {
      notBefore = system_clock::now();
    }
  else
    {
      notBefore = fromIsoString(notBeforeStr.substr(0, 8) + "T" +
                                notBeforeStr.substr(8, 6));
    }

  if (vm.count("not-after") == 0)
    {
      notAfter = notBefore + days(365);
    }
  else
    {
      notAfter = fromIsoString(notAfterStr.substr(0, 8) + "T" +
                               notAfterStr.substr(8, 6));

      if (notAfter < notBefore)
        {
          std::cerr << "not-before is later than not-after" << std::endl;
          return 1;
        }
    }

  if (vm.count("request") == 0)
    {
      std::cerr << "request file must be specified" << std::endl;
      return 1;
    }

  shared_ptr<IdentityCertificate> selfSignedCertificate
    = getIdentityCertificate(requestFile);

  if (!static_cast<bool>(selfSignedCertificate))
    {
      std::cerr << "ERROR: input error" << std::endl;
      return 1;
    }

  KeyChain keyChain;

  Name keyName = selfSignedCertificate->getPublicKeyName();
  Name signIdName;
  Name certName;

  if (!hasSignId)
    signIdName = keyChain.getDefaultIdentity();
  else
    signIdName = Name(signId);


  if (signIdName.isPrefixOf(keyName))
    {
      // if signee's namespace is a sub-namespace of signer, for example, signer's namespace is
      // /ndn/test, signee's namespace is /ndn/test/alice, the generated certificate name is
      // /ndn/test/KEY/alice/ksk-1234/ID-CERT/%01%02
      certName.append(signIdName)
        .append("KEY")
        .append(keyName.getSubName(signIdName.size()))
        .append("ID-CERT")
        .appendVersion();
    }
  else
    {
      // if signee's namespace is not a sub-namespace of signer, for example, signer's namespace is
      // /ndn/test, signee's namespace is /ndn/ucla/bob, the generated certificate name is
      // /ndn/ucla/bob/KEY/ksk-1234/ID-CERT/%01%02
      certName.append(keyName.getPrefix(-1))
        .append("KEY")
        .append(keyName.get(-1))
        .append("ID-CERT")
        .appendVersion();
    }

  Block wire;

  if (!isNack)
    {
      if (vm.count("subject-name") == 0)
        {
          std::cerr << "subject_name must be specified" << std::endl;
          return 1;
        }

      CertificateSubjectDescription subDescryptName("2.5.4.41", subjectName);
      IdentityCertificate certificate;
      certificate.setName(certName);
      certificate.setNotBefore(notBefore);
      certificate.setNotAfter(notAfter);
      certificate.setPublicKeyInfo(selfSignedCertificate->getPublicKeyInfo());
      certificate.addSubjectDescription(subDescryptName);
      for (size_t i = 0; i < otherSubDescrypt.size(); i++)
        certificate.addSubjectDescription(otherSubDescrypt[i]);
      certificate.encode();

      keyChain.createIdentity(signIdName);
      Name signingCertificateName = keyChain.getDefaultCertificateNameForIdentity(signIdName);
      keyChain.sign(certificate, signingCertificateName);

      wire = certificate.wireEncode();
    }
  else
    {
      Data revocationCert;
      // revocationCert.setContent(void*, 0); // empty content
      revocationCert.setName(certName);

      keyChain.createIdentity(signIdName);
      Name signingCertificateName = keyChain.getDefaultCertificateNameForIdentity(signIdName);
      keyChain.sign(revocationCert, signingCertificateName);

      wire = revocationCert.wireEncode();
    }

  try
    {
      using namespace CryptoPP;
      StringSource ss(wire.wire(), wire.size(), true,
                      new Base64Encoder(new FileSink(std::cout), true, 64));
    }
  catch (const CryptoPP::Exception& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl;
      return 1;
    }

  return 0;
}

#endif //NDNSEC_CERT_GEN_HPP
