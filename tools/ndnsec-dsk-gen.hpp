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

#ifndef NDNSEC_DSK_GEN_HPP
#define NDNSEC_DSK_GEN_HPP

#include "ndnsec-util.hpp"

int
ndnsec_dsk_gen(int argc, char** argv)
{
  using namespace ndn;
  namespace po = boost::program_options;

  std::string identityName;
  char keyType = 'r';
  int keySize = 2048;

  po::options_description description("General Usage\n  ndnsec dsk-gen [-h] identity\nGeneral options");
  description.add_options()
    ("help,h", "produce help message")
    ("identity,i", po::value<std::string>(&identityName), "identity name, for example, /ndn/ucla.edu/alice")
    // ("type,t", po::value<char>(&keyType)->default_value('r'), "optional, key type, r for RSA key (default)")
    // ("size,s", po::value<int>(&keySize)->default_value(2048), "optional, key size, 2048 (default)")
    ;

  po::positional_options_description p;
  p.add("identity", 1);

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
      std::cerr << description << std::endl;
      return 1;
    }

  if (vm.count("help") != 0)
    {
      std::cerr << description << std::endl;
      return 0;
    }

  if (vm.count("identity") == 0)
    {
      std::cerr << "identity must be specified" << std::endl;
      std::cerr << description << std::endl;
      return 1;
    }

  shared_ptr<IdentityCertificate> kskCert;
  Name signingCertName;

  KeyChain keyChain;

  Name defaultCertName = keyChain.getDefaultCertificateNameForIdentity(identityName);
  bool isDefaultDsk = false;
  if (defaultCertName.get(-3).toUri().substr(0,4) == "dsk-")
    isDefaultDsk = true;

  if (isDefaultDsk)
    {
      shared_ptr<IdentityCertificate> dskCert = keyChain.getCertificate(defaultCertName);
      SignatureSha256WithRsa sha256sig(dskCert->getSignature());

      Name keyLocatorName = sha256sig.getKeyLocator().getName(); // will throw exception if keylocator is absent or it is not a name

      Name kskName = IdentityCertificate::certificateNameToPublicKeyName(keyLocatorName);
      Name kskCertName = keyChain.getDefaultCertificateNameForKey(kskName);
      signingCertName = kskCertName;
      kskCert = keyChain.getCertificate(kskCertName);
    }
  else
    {
      signingCertName = defaultCertName;
      kskCert = keyChain.getCertificate(defaultCertName);
    }

  if (!static_cast<bool>(kskCert))
    {
      std::cerr << "ERROR: no KSK certificate." << std::endl;
      return 1;
    }

  Name newKeyName;
  switch (keyType)
    {
    case 'r':
      {
        newKeyName = keyChain.generateRsaKeyPair(Name(identityName), false, keySize);
        if (0 == newKeyName.size())
          {
            std::cerr << "fail to generate key!" << std::endl;
            return 1;
          }
        break;
      }
    default:
      std::cerr << "Unrecongized key type" << "\n";
      std::cerr << description << std::endl;
      return 1;
    }

  Name certName = newKeyName.getPrefix(-1);
  certName.append("KEY")
    .append(newKeyName.get(-1))
    .append("ID-CERT")
    .appendVersion();

  shared_ptr<IdentityCertificate> certificate = make_shared<IdentityCertificate>();
  certificate->setName(certName);
  certificate->setNotBefore(kskCert->getNotBefore());
  certificate->setNotAfter(kskCert->getNotAfter());

  certificate->setPublicKeyInfo(*keyChain.getPublicKey(newKeyName));

  const std::vector<CertificateSubjectDescription>& subList =
    kskCert->getSubjectDescriptionList();

  for (std::vector<CertificateSubjectDescription>::const_iterator it = subList.begin();
       it != subList.end(); it++)
    certificate->addSubjectDescription(*it);

  certificate->encode();

  keyChain.sign(*certificate, signingCertName);

  keyChain.addCertificateAsIdentityDefault(*certificate);

  return 0;
}

#endif //NDNSEC_DSK_GEN_HPP
