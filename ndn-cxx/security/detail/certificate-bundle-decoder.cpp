/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
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

#include "ndn-cxx/security/detail/certificate-bundle-decoder.hpp"
#include "ndn-cxx/util/scope.hpp"

namespace ndn {
namespace security {
namespace detail {

void
CertificateBundleDecoder::append(const Block& segment)
{
  if (m_hasError) {
    NDN_THROW(tlv::Error("Unrecoverable decoding error"));
  }

  m_bufferedData.insert(m_bufferedData.end(), segment.value_begin(), segment.value_end());
  decode();
}

void
CertificateBundleDecoder::decode()
{
  auto onThrow = make_scope_fail([this] { m_hasError = true; });

  while (!m_bufferedData.empty()) {
    bool isOk;
    Block element;
    std::tie(isOk, element) = Block::fromBuffer(m_bufferedData);
    if (!isOk) {
      return;
    }

    m_bufferedData.erase(m_bufferedData.begin(), m_bufferedData.begin() + element.size());

    if (element.type() == tlv::Data) {
      onCertDecoded(Certificate(element));
    }
    else if (tlv::isCriticalType(element.type())) {
      NDN_THROW(tlv::Error("Unrecognized element of critical type " + to_string(element.type())));
    }
    // unrecognized non-critical elements are silently skipped
  }
}

} // namespace detail
} // namespace security
} // namespace ndn
