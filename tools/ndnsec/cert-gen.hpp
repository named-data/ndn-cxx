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

#ifndef NDN_TOOLS_NDNSEC_CERT_GEN_HPP
#define NDN_TOOLS_NDNSEC_CERT_GEN_HPP

#include "util.hpp"

int
ndnsec_cert_gen(int argc, char** argv)
{
  using boost::tokenizer;
  using boost::escaped_list_separator;

  using namespace ndn;
  using namespace ndn::time;
  namespace po = boost::program_options;

  KeyChain keyChain;

  std::string notBeforeStr;
  std::string notAfterStr;
  std::string subjectName;
  std::string requestFile("-");
  Name signId;
  std::string subjectInfo;
  std::vector<std::string> signedInfo;
  Name certPrefix = KeyChain::DEFAULT_PREFIX; // to avoid displaying the default value

  po::options_description description(
    "General Usage\n"
    "  ndnsec cert-gen [-h] [-S date] [-E date] [-N subject-name] [-I subject-info] "
        "[-s sign-id] [-p cert-prefix] request\n"
    "General options");

  description.add_options()
    ("help,h", "produce help message")
    ("not-before,S",   po::value<std::string>(&notBeforeStr),
                       "certificate starting date, YYYYMMDDhhmmss (default: now)")
    ("not-after,E",    po::value<std::string>(&notAfterStr),
                       "certificate ending date, YYYYMMDDhhmmss (default: now + 365 days)")
    ("subject-name,N", po::value<std::string>(&subjectName),
                       "subject name")
    ("subject-info,I", po::value<std::string>(&subjectInfo),
                       "(deprecated, uses 'signed-info') subject info, pairs of OID and string "
                       " description: \"2.5.4.10 'University of California, Los Angeles'\"")
    ("signed-info",    po::value<std::vector<std::string> >(&signedInfo),
                       "a pair of OID and string (must be separated by a single space), e.g., "
                       "\"2.5.4.10 University of California, Los Angeles\". "
                       "May be repeated multiple times")
    ("sign-id,s",      po::value<Name>(&signId)->default_value(keyChain.getDefaultIdentity()),
                       "signing identity")
    ("cert-prefix,p",  po::value<Name>(&certPrefix),
                       "cert prefix, which is the part of certificate name before "
                       "KEY component")
    ("request,r",      po::value<std::string>(&requestFile)->default_value("-"),
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
      std::cout << description << std::endl;
      return 0;
    }

  if (vm.count("subject-name") == 0)
    {
      std::cerr << "ERROR: subject name must be specified" << std::endl
                << std::endl
                << description << std::endl;
      return 1;
    }

  std::vector<CertificateSubjectDescription> subjectDescription;
  subjectDescription.push_back(CertificateSubjectDescription(oid::ATTRIBUTE_NAME, subjectName));

  // 'subjectInfo' is deprecated and the following block will be removed eventually
  tokenizer<escaped_list_separator<char> > subjectInfoItems
    (subjectInfo, escaped_list_separator<char>("\\", " \t", "'\""));

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

      subjectDescription.push_back(CertificateSubjectDescription(OID(oid), value));

      it++;
    }

  // new 'signedInfo' processing
  for (std::vector<std::string>::const_iterator info = signedInfo.begin();
       info != signedInfo.end(); ++info) {
    size_t pos = info->find(" ");
    if (pos == std::string::npos) {
      std::cerr << "ERROR: incorrectly formatted signed info block [" << *info << "]" << std::endl;
      return 1;
    }
    OID oid(info->substr(0, pos));
    std::string value = info->substr(pos + 1);

    subjectDescription.push_back(CertificateSubjectDescription(oid, value));
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
          std::cerr << "ERROR: not-before cannot be later than not-after" << std::endl
                    << std::endl
                    << description << std::endl;
          return 1;
        }
    }

  if (vm.count("request") == 0)
    {
      std::cerr << "ERROR: request file must be specified" << std::endl
                << std::endl
                << description << std::endl;
      return 1;
    }

  shared_ptr<IdentityCertificate> selfSignedCertificate
    = getIdentityCertificate(requestFile);

  if (!static_cast<bool>(selfSignedCertificate))
    {
      std::cerr << "ERROR: input error" << std::endl;
      return 1;
    }

  Name keyName = selfSignedCertificate->getPublicKeyName();

  shared_ptr<IdentityCertificate> certificate =
    keyChain.prepareUnsignedIdentityCertificate(keyName, selfSignedCertificate->getPublicKeyInfo(),
                                                signId, notBefore, notAfter,
                                                subjectDescription, certPrefix);

  if (!static_cast<bool>(certificate))
    {
      std::cerr << "ERROR: key name is not formated correctly or does not match certificate name"
                << std::endl;
      return 1;
    }

  keyChain.createIdentity(signId);
  Name signingCertificateName = keyChain.getDefaultCertificateNameForIdentity(signId);
  keyChain.sign(*certificate,
                security::SigningInfo(security::SigningInfo::SIGNER_TYPE_CERT,
                                      signingCertificateName));

  Block wire = certificate->wireEncode();

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

#endif // NDN_TOOLS_NDNSEC_CERT_GEN_HPP
