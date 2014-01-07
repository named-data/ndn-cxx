/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <ndn-cpp/face.hpp>
#include <ndn-cpp/encoding/block.hpp>

#include <iomanip>
#include <fstream>

const int TLV_DICT_SIZE = 25;

const std::string TLV_DICT[TLV_DICT_SIZE] = {
  "RESERVED", //      = 0
  "Interest", //      = 1,
  "Data", //          = 2,
  "Name", //          = 3,
  "NameComponent", // = 4,
  "Selectors", //     = 5,
  "Nonce", //         = 6,
  "Scope", //         = 7,
  "InterestLifetime", //          = 8,
  "MinSuffixComponents", //       = 9,
  "MaxSuffixComponents", //       = 10,
  "PublisherPublicKeyLocator", // = 11,
  "Exclude", //       = 12,
  "ChildSelector", // = 13,
  "MustBeFresh", //   = 14,
  "Any", //           = 15,
  "MetaInfo", //      = 16,
  "Content", //       = 17,
  "SignatureInfo", // = 18,
  "SignatureValue", // = 19,
  "ContentType", //   = 20,
  "FreshnessPeriod", // = 21,
  "SignatureType", // = 22,
  "KeyLocator", //    = 23,
  "KeyLocatorDigest", // = 24
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
BlockPrinter(ndn::Block &block, const std::string &indent="")
{
  std::cout << indent;
  printTypeInfo(block.type());
  std::cout << " (size: " << block.value_size() << ")";

  try {
    // if (block.type() != ndn::Tlv::Content && block.type() != ndn::Tlv::SignatureValue)
    block.parse();
  }
  catch(ndn::Tlv::Error &e) {
    // pass (e.g., leaf block reached)

    // @todo: Figure how to deterministically figure out that value is not recursive TLV block
  }

  if (block.getAll().empty())
    {
      std::cout << " [[";
      ndn::Name::toEscapedString(block.value(), block.value_size(), std::cout);
      std::cout<< "]]";
    }
  std::cout << std::endl;
  
  for(ndn::Block::element_iterator i = block.getAll().begin();
      i != block.getAll().end();
      ++i)
    {
      BlockPrinter(*i, indent+"  ");
    }
}

void
HexPrinter(ndn::Block &block, const std::string &indent="")
{
  std::cout << indent;
  for (ndn::Buffer::const_iterator i = block.begin (); i != block.value_begin(); ++i)
    {
      std::cout << "0x" << std::noshowbase << std::hex << std::setw(2) << std::setfill('0') << (int)*i;
      std::cout << ", ";
    }
  std::cout << "\n";
  
  if (block.getAll().size() == 0 && block.value_size() > 0)
    {
      std::cout << indent << "    ";
      for (ndn::Buffer::const_iterator i = block.value_begin (); i != block.value_end(); ++i)
      {
        std::cout << "0x" << std::noshowbase << std::hex << std::setw(2) << std::setfill('0') << (int)*i;
        std::cout << ", ";
      }
      std::cout << "\n";
    }
  else
    {
      for(ndn::Block::element_iterator i = block.getAll().begin();
          i != block.getAll().end();
          ++i)
        {
          HexPrinter(*i, indent+"    ");
        }
    }
}

int main(int argc, const char *argv[])
{
  unsigned char buf[9000];
  std::streamsize s = 0;
  if (argc == 1 ||
      (argc == 2 && std::string(argv[1]) == "-"))
    {
      std::cin.read(reinterpret_cast<char*>(buf), 9000);
      s = std::cin.gcount();
    }
  else
    {
      std::ifstream file(argv[1]);
      file.read(reinterpret_cast<char*>(buf), 9000);
      s = file.gcount();
    }

  try {
    ndn::Block block(buf, s);
    BlockPrinter(block, "");
    // HexPrinter(block, "");
  }
  catch(std::exception &e) {
    std::cerr << "ERROR: "<< e.what() << std::endl;
  }
  
  return 0;
}
