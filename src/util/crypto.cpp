/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2017 Regents of the University of California.
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
#include "../security/transform/buffer-source.hpp"
#include "../security/transform/digest-filter.hpp"
#include "../security/transform/stream-sink.hpp"

namespace ndn {
namespace crypto {

ConstBufferPtr
computeSha256Digest(const uint8_t* data, size_t dataLength)
{
  namespace tr = security::transform;
  try {
    OBufferStream os;
    tr::bufferSource(data, dataLength) >> tr::digestFilter(DigestAlgorithm::SHA256)
                                       >> tr::streamSink(os);
    return os.buf();
  }
  catch (const tr::Error&) {
    return nullptr;
  }
}

} // namespace crypto
} // namespace ndn
