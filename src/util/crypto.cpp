/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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

#include "crypto.hpp"
#include "../encoding/buffer-stream.hpp"

#include "../security/v1/cryptopp.hpp"

namespace ndn {
namespace crypto {

ConstBufferPtr
computeSha256Digest(const uint8_t* data, size_t dataLength)
{
  try {
    CryptoPP::SHA256 hash;
    OBufferStream os;
    CryptoPP::StringSource(data, dataLength, true,
      new CryptoPP::HashFilter(hash, new CryptoPP::FileSink(os)));
    return os.buf();
  }
  catch (CryptoPP::Exception& e) {
    return ConstBufferPtr();
  }
}

} // namespace crypto
} // namespace ndn
