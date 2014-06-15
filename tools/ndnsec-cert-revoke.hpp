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

#ifndef NDNSEC_CERT_REVOKE_HPP
#define NDNSEC_CERT_REVOKE_HPP

#include "ndnsec-util.hpp"

int
ndnsec_cert_revoke(int argc, char** argv)
{
  using namespace ndn;
  namespace po = boost::program_options;

  std::string requestFile("-");

  po::options_description description("General Usage\n  ndnsec cert-revoke [-h] request\nGeneral options");
  description.add_options()
    ("help,h", "produce help message")
    ("request,r", po::value<std::string>(&requestFile), "file name of the certificate to revoke, - for stdin")
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

  KeyChain keyChain;
  Block wire;

  try
    {
      Name certName = revokedCertificate->getName();
      certName.append("REVOKED");

      Data revocationCert;
      revocationCert.setName(certName);

      Name keyName;

      const Signature& signature = revokedCertificate->getSignature();
      if (signature.getType() == Signature::Sha256WithRsa)
        {
          SignatureSha256WithRsa sigSha256Rsa(signature);
          Name keyLocatorName = sigSha256Rsa.getKeyLocator().getName();

          keyName = IdentityCertificate::certificateNameToPublicKeyName(keyLocatorName);
        }
      else
        {
          std::cerr << "ERROR: Unsupported Signature Type!" << std::endl;
          return 1;
        }

      if (keyChain.doesPublicKeyExist(keyName))
        {
          Name signingCertificateName = keyChain.getDefaultCertificateNameForKey(keyName);
          keyChain.sign(revocationCert, signingCertificateName);
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

  try
    {
      using namespace CryptoPP;
      // StringSource ss(wire.wire(), wire.size(), true,
      //                 new Base64Encoder(new FileSink(std::cout), true, 64));
      StringSource ss(wire.wire(), wire.size(), true,
                      new FileSink(std::cout));
    }
  catch (const CryptoPP::Exception& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl;
      return 1;
    }

  return 0;
}

#endif //NDNSEC_CERT_REVOKE_HPP
