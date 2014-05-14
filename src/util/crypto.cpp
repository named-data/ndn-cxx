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
 */

#include "../common.hpp"

#include "crypto.hpp"
#include "../encoding/buffer-stream.hpp"
#include "../security/cryptopp.hpp"

namespace ndn {

void ndn_digestSha256(const uint8_t* data, size_t dataLength, uint8_t* digest)
{
  try
    {
      using namespace CryptoPP;

      CryptoPP::SHA256 hash;
      OBufferStream os;
      StringSource(data, dataLength, true,
                   new HashFilter(hash, new ArraySink(digest, crypto::SHA256_DIGEST_SIZE)));
    }
  catch (CryptoPP::Exception& e)
    {
      return;
    }

}

namespace crypto {

ConstBufferPtr
sha256(const uint8_t* data, size_t dataLength)
{
  try
    {
      using namespace CryptoPP;

      SHA256 hash;
      OBufferStream os;
      StringSource(data, dataLength, true, new HashFilter(hash, new FileSink(os)));
      return os.buf();
    }
  catch (CryptoPP::Exception& e)
    {
      return ConstBufferPtr();
    }
}

} // namespace crypto

} // namespace ndn
