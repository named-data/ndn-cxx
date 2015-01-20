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

#ifndef NDN_TOOLS_NDNSEC_SIG_VERIFY_HPP
#define NDN_TOOLS_NDNSEC_SIG_VERIFY_HPP

#include "util.hpp"

// using namespace ndn;
// namespace po = boost::program_options;

// shared_ptr<IdentityCertificate>
// getCertificate(const std::string& certString)
// {
//   std::string decoded;
//   CryptoPP::StringSource ss2(reinterpret_cast<const unsigned char *>(certString.c_str()), certString.size(), true,
//                              new CryptoPP::Base64Decoder(new CryptoPP::StringSink(decoded)));

//   Data data;
//   data.wireDecode(Block(make_shared<Buffer>(decoded.begin(), decoded.end())));

//   shared_ptr<IdentityCertificate> identityCertificate = make_shared<IdentityCertificate>(data);

//   return identityCertificate;
// }

// bool
// verifySignature(shared_ptr<IdentityCertificate> certificate, bool isDataPacket)
// {
//   throw std::runtime_error("Not supported yet");
//   // if(isDataPacket)
//   //   {
//   //     std::string decoded;
//   //     CryptoPP::FileSource ss2(cin, true,
//   //                              new CryptoPP::Base64Decoder(new CryptoPP::StringSink(decoded)));

//   //     Data data;
//   //     data.wireDecode(make_shared<Buffer>(decoded.c_str(), decoded.size()));
//   //     return PolicyManager::verifySignature(data, certificate->getPublicKeyInfo());
//   //   }
//   // else
//   //   {
//   //     // The first two bytes indicates the boundary of the of the signed data and signature.
//   //     // for example, if the size of the signed data is 300, then the boundary should be 300, so the first two bytes should be: 0x01 0x2C
//   //     shared_ptr<Blob> input = shared_ptr<Blob>(new Blob ((istreambuf_iterator<char>(cin)), istreambuf_iterator<char>()));
//   //     size_t size = input->at(0);
//   //     size = ((size << 8) + input->at(1));

//   //     Blob signedBlob(input->buf()+2, size);
//   //     Blob signature(input->buf()+2+size, input->size()-2-size);

//   //     return PolicyManager::verifySignature(signedBlob, signature, certificate->getPublicKeyInfo());
//   //   }
// }

int
ndnsec_sig_verify(int argc, char** argv)
{
  std::cerr << "Not supported yet" << std::endl;
  return 1;
  // bool isDataPacket = false;
  // std::string certString;

  // po::options_description desc("General Usage\n  ndn-sig-verify [-h] [-d] certificate\nGeneral options");
  // desc.add_options()
  //   ("help,h", "produce help message")
  //   ("data,d", "if specified, input from stdin will be treated as a Data packet, otherwise binary data")
  //   ("certificate,c", po::value<std::string>(&certString), "the certificate bits")
  //   ;

  // po::positional_options_description p;
  // p.add("certificate", 1);

  // po::variables_map vm;
  // try
  //   {
  //     po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
  //     po::notify(vm);
  //   }
  // catch( const std::exception& e)
  //   {
  //     std::cerr << e.what() << std::endl;
  //     std::cerr << desc << std::endl;
  //     return 1;
  //   }

  // if (vm.count("help") || vm.count("certificate")==0)
  //   {
  //     std::cerr << desc << std::endl;
  //     return 1;
  //   }
  // if (vm.count("data"))
  //   isDataPacket = true;

  // try
  //   {
  //     shared_ptr<IdentityCertificate> certificate = getCertificate(certString);
  //     bool res = verifySignature(certificate, isDataPacket);
  //     return (res ? 0 : 1);
  //   }
  // catch(const std::exception &e)
  //   {
  //     std::cerr << "ERROR: " << e.what() << std::endl;
  //     return 1;
  //   }
}

#endif // NDN_TOOLS_NDNSEC_SIG_VERIFY_HPP
