/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 *
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 */

#include "face.hpp"
#include "encoding/block.hpp"

#include <iomanip>
#include <fstream>

const uint32_t TLV_DICT_SIZE = 30;

const std::string TLV_DICT[TLV_DICT_SIZE] = {
  "RESERVED", //      = 0
  "RESERVED", //      = 1
  "RESERVED", //      = 2
  "RESERVED", //      = 3
  "RESERVED", //      = 4
  "Interest", //      = 5,
  "Data", //          = 6,
  "Name", //          = 7,
  "NameComponent", // = 8,
  "Selectors", //     = 9,
  "Nonce", //         = 10,
  "Scope", //         = 11,
  "InterestLifetime", //          = 12,
  "MinSuffixComponents", //       = 13,
  "MaxSuffixComponents", //       = 14,
  "PublisherPublicKeyLocator", // = 15,
  "Exclude", //       = 16,
  "ChildSelector", // = 17,
  "MustBeFresh", //   = 18,
  "Any", //           = 19,
  "MetaInfo", //      = 20,
  "Content", //       = 21,
  "SignatureInfo", // = 22,
  "SignatureValue", // = 23,
  "ContentType", //   = 24,
  "FreshnessPeriod", // = 25,
  "FinalBlockId" // = 26
  "SignatureType", // = 27,
  "KeyLocator", //    = 28,
  "KeyLocatorDigest", // = 29
};

void
printTypeInfo(uint32_t type)
{
  std::cout << type << " (";

  if (type < TLV_DICT_SIZE) {
    std::cout << TLV_DICT[type];
  }
  else if (TLV_DICT_SIZE <= type && type < 128) {
    std::cout << "RESERVED_1";
  }
  else if (128 <= type && type < 253) {
    std::cout << "APP_TAG_1";
  }
  else if (253 <= type && type < 32767) {
    std::cout << "RESERVED_3";
  }
  else {
    std::cout << "APP_TAG_3";
  }
  std::cout << ")";
}


void
BlockPrinter(const ndn::Block& block, const std::string& indent = "")
{
  std::cout << indent;
  printTypeInfo(block.type());
  std::cout << " (size: " << block.value_size() << ")";

  try {
    // if (block.type() != ndn::Tlv::Content && block.type() != ndn::Tlv::SignatureValue)
    block.parse();
  }
  catch (ndn::Tlv::Error& e) {
    // pass (e.g., leaf block reached)

    // @todo: Figure how to deterministically figure out that value is not recursive TLV block
  }

  if (block.elements().empty())
    {
      std::cout << " [[";
      ndn::name::Component(block.value(), block.value_size()).toUri(std::cout);
      std::cout<< "]]";
    }
  std::cout << std::endl;

  for (ndn::Block::element_const_iterator i = block.elements_begin();
       i != block.elements_end();
       ++i)
    {
      BlockPrinter(*i, indent+"  ");
    }
}

void
HexPrinter(const ndn::Block& block, const std::string& indent = "")
{
  std::cout << indent;
  for (ndn::Buffer::const_iterator i = block.begin (); i != block.value_begin(); ++i)
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
      for (ndn::Buffer::const_iterator i = block.value_begin (); i != block.value_end(); ++i)
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
      for (ndn::Block::element_const_iterator i = block.elements_begin();
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
      ndn::Block block = ndn::Block::fromStream(is);
      BlockPrinter(block, "");
      // HexPrinter(block, "");
    }
    catch (std::exception& e) {
      std::cerr << "ERROR: " << e.what() << std::endl;
    }
  }

}

int main(int argc, const char *argv[])
{
  if (argc == 1 ||
      (argc == 2 && std::string(argv[1]) == "-"))
    {
      parseBlocksFromStream(std::cin);
    }
  else
    {
      std::ifstream file(argv[1]);
      parseBlocksFromStream(file);
    }

  return 0;
}
