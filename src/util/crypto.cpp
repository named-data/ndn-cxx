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
