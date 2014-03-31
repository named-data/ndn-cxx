/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include "../common.hpp"

#include "crypto.hpp"
#include "../security/cryptopp.hpp"

namespace ndn {

void ndn_digestSha256(const uint8_t *data, size_t dataLength, uint8_t *digest)
{
  try
    {
      using namespace CryptoPP;
      
      CryptoPP::SHA256 hash;
      OBufferStream os;
      StringSource(data, dataLength, true, new HashFilter(hash, new ArraySink(digest, crypto::SHA256_DIGEST_SIZE)));
    }
  catch(CryptoPP::Exception& e)
    {
      return;
    }

}

namespace crypto {

ConstBufferPtr
sha256(const uint8_t *data, size_t dataLength)
{
  try
    {
      using namespace CryptoPP;
      
      SHA256 hash;
      OBufferStream os;
      StringSource(data, dataLength, true, new HashFilter(hash, new FileSink(os)));
      return os.buf();
    }
  catch(CryptoPP::Exception& e)
    {
      return ConstBufferPtr();
    }
}

} // namespace crypto

} // namespace ndn
