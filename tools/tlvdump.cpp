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
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 */

#include "face.hpp"
#include "encoding/block.hpp"

#include <iomanip>
#include <fstream>
#include <map>

namespace ndn {

std::map<uint32_t, std::string> TLV_DICT = {
  {tlv::Interest                     , "Interest"},
  {tlv::Data                         , "Data"},
  {tlv::Name                         , "Name"},
  {tlv::NameComponent                , "NameComponent"},
  {tlv::ImplicitSha256DigestComponent, "ImplicitSha256DigestComponent"},
  {tlv::Selectors                    , "Selectors"},
  {tlv::Nonce                        , "Nonce"},
  {tlv::InterestLifetime             , "InterestLifetime"},
  {tlv::MinSuffixComponents          , "MinSuffixComponents"},
  {tlv::MaxSuffixComponents          , "MaxSuffixComponents"},
  {tlv::PublisherPublicKeyLocator    , "PublisherPublicKeyLocator"},
  {tlv::Exclude                      , "Exclude"},
  {tlv::ChildSelector                , "ChildSelector"},
  {tlv::MustBeFresh                  , "MustBeFresh"},
  {tlv::Any                          , "Any"},
  {tlv::MetaInfo                     , "MetaInfo"},
  {tlv::Content                      , "Content"},
  {tlv::SignatureInfo                , "SignatureInfo"},
  {tlv::SignatureValue               , "SignatureValue"},
  {tlv::ContentType                  , "ContentType"},
  {tlv::FreshnessPeriod              , "FreshnessPeriod"},
  {tlv::FinalBlockId                 , "FinalBlockId"},
  {tlv::SignatureType                , "SignatureType"},
  {tlv::KeyLocator                   , "KeyLocator"},
  {tlv::KeyDigest                    , "KeyDigest"},
};

void
printTypeInfo(uint32_t type)
{
  std::cout << type << " (";

  if (TLV_DICT.count(type) != 0) {
    std::cout << TLV_DICT[type];
  }
  else if (type < tlv::AppPrivateBlock1) {
    std::cout << "RESERVED_1";
  }
  else if (tlv::AppPrivateBlock1 <= type && type < 253) {
    std::cout << "APP_TAG_1";
  }
  else if (253 <= type && type < tlv::AppPrivateBlock2) {
    std::cout << "RESERVED_3";
  }
  else {
    std::cout << "APP_TAG_3";
  }
  std::cout << ")";
}


void
BlockPrinter(const Block& block, const std::string& indent = "")
{
  std::cout << indent;
  printTypeInfo(block.type());
  std::cout << " (size: " << block.value_size() << ")";

  try {
    // if (block.type() != tlv::Content && block.type() != tlv::SignatureValue)
    block.parse();
  }
  catch (tlv::Error& e) {
    // pass (e.g., leaf block reached)

    // @todo: Figure how to deterministically figure out that value is not recursive TLV block
  }

  if (block.elements().empty())
    {
      std::cout << " [[";
      name::Component(block.value(), block.value_size()).toUri(std::cout);
      std::cout<< "]]";
    }
  std::cout << std::endl;

  for (Block::element_const_iterator i = block.elements_begin();
       i != block.elements_end();
       ++i)
    {
      BlockPrinter(*i, indent+"  ");
    }
}

void
HexPrinter(const Block& block, const std::string& indent = "")
{
  std::cout << indent;
  for (Buffer::const_iterator i = block.begin (); i != block.value_begin(); ++i)
    {
      std::cout << "0x";
      std::cout << std::noshowbase << std::hex << std::setw(2) <<
        std::setfill('0') << static_cast<int>(*i);
      std::cout << ", ";
    }
  std::cout << "\n";

  if (block.elements_size() == 0 && block.value_size() > 0)
    {
      std::cout << indent << "    ";
      for (Buffer::const_iterator i = block.value_begin (); i != block.value_end(); ++i)
      {
        std::cout << "0x";
        std::cout << std::noshowbase << std::hex << std::setw(2) <<
          std::setfill('0') << static_cast<int>(*i);
        std::cout << ", ";
      }
      std::cout << "\n";
    }
  else
    {
      for (Block::element_const_iterator i = block.elements_begin();
           i != block.elements_end();
           ++i)
        {
          HexPrinter(*i, indent+"    ");
        }
    }
}

void
parseBlocksFromStream(std::istream& is)
{
  while (is.peek() != std::char_traits<char>::eof()) {
    try {
      Block block = Block::fromStream(is);
      BlockPrinter(block, "");
      // HexPrinter(block, "");
    }
    catch (std::exception& e) {
      std::cerr << "ERROR: " << e.what() << std::endl;
    }
  }

}

} // namespace ndn

int main(int argc, const char *argv[])
{
  if (argc == 1 ||
      (argc == 2 && std::string(argv[1]) == "-"))
    {
      ndn::parseBlocksFromStream(std::cin);
    }
  else
    {
      std::ifstream file(argv[1]);
      ndn::parseBlocksFromStream(file);
    }

  return 0;
}
