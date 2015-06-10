/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#ifndef NDN_TOOLS_NDNSEC_CERT_REVOKE_HPP
#define NDN_TOOLS_NDNSEC_CERT_REVOKE_HPP

#include "util.hpp"

int
ndnsec_cert_revoke(int argc, char** argv)
{
  using namespace ndn;
  namespace po = boost::program_options;

  KeyChain keyChain;

  std::string requestFile("-");
  Name signId = keyChain.getDefaultIdentity();
  bool hasSignId = false;
  Name certPrefix = KeyChain::DEFAULT_PREFIX;

  po::options_description description("General Usage\n  ndnsec cert-revoke [-h] request\n"
                                      "General options");
  description.add_options()
    ("help,h", "produce help message")
    ("sign-id,s",     po::value<Name>(&signId),
                      "signing identity (default: use the same as in the revoked certificate)")
    ("cert-prefix,p", po::value<Name>(&certPrefix),
                      "cert prefix, which is the part of certificate name before "
                      "KEY component (default: use the same as in the revoked certificate)")
    ("request,r",     po::value<std::string>(&requestFile)->default_value("-"),
                      "request file name, - for stdin")
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

  hasSignId = (vm.count("sign-id") != 0);

  if (vm.count("request") == 0)
    {
      std::cerr << "request file must be specified" << std::endl;
      return 1;
    }

  shared_ptr<IdentityCertificate> revokedCertificate
    = getIdentityCertificate(requestFile);

  if (!static_cast<bool>(revokedCertificate))
    {
      std::cerr << "ERROR: input error" << std::endl;
      return 1;
    }

  Block wire;

  try
    {
      Name keyName;

      if (hasSignId) {
        keyName = keyChain.getDefaultKeyNameForIdentity(signId);
      }
      else {
        const Signature& signature = revokedCertificate->getSignature();
        if (!signature.hasKeyLocator() ||
            signature.getKeyLocator().getType() != KeyLocator::KeyLocator_Name)
          {
            std::cerr << "ERROR: Invalid certificate to revoke" << std::endl;
            return 1;
          }

        keyName = IdentityCertificate::certificateNameToPublicKeyName(
                    signature.getKeyLocator().getName());
      }

      Name certName;
      if (certPrefix == KeyChain::DEFAULT_PREFIX) {
        certName = revokedCertificate->getName().getPrefix(-1);
      }
      else {
        Name revokedKeyName = revokedCertificate->getPublicKeyName();

        if (certPrefix.isPrefixOf(revokedKeyName) && certPrefix != revokedKeyName) {
          certName.append(certPrefix)
            .append("KEY")
            .append(revokedKeyName.getSubName(certPrefix.size()))
            .append("ID-CERT");
        }
        else {
          std::cerr << "ERROR: certificate prefix does not match the revoked certificate"
                    << std::endl;
          return 1;
        }
      }
      certName
        .appendVersion()
        .append("REVOKED");

      Data revocationCert;
      revocationCert.setName(certName);

      if (keyChain.doesPublicKeyExist(keyName))
        {
          Name signingCertificateName = keyChain.getDefaultCertificateNameForKey(keyName);
          keyChain.sign(revocationCert,
                        security::SigningInfo(security::SigningInfo::SIGNER_TYPE_CERT,
                                              signingCertificateName));
        }
      else
        {
          std::cerr << "ERROR: Cannot find the signing key!" << std::endl;
          return 1;
        }

      wire = revocationCert.wireEncode();
    }
  catch (Signature::Error& e)
    {
      std::cerr << "ERROR: No valid signature!" << std::endl;
      return 1;
    }
  catch (KeyLocator::Error& e)
    {
      std::cerr << "ERROR: No valid KeyLocator!" << std::endl;
      return 1;
    }
  catch (IdentityCertificate::Error& e)
    {
      std::cerr << "ERROR: Cannot determine the signing key!" << std::endl;
      return 1;
    }
  catch (SecPublicInfo::Error& e)
    {
      std::cerr << "ERROR: Incomplete or corrupted PIB (" << e.what() << ")" << std::endl;
      return 1;
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

#endif // NDN_TOOLS_NDNSEC_CERT_REVOKE_HPP
