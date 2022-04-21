/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2020 Regents of the University of California.
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

#ifndef NDN_CXX_SECURITY_DETAIL_CERTIFICATE_BUNDLE_DECODER_HPP
#define NDN_CXX_SECURITY_DETAIL_CERTIFICATE_BUNDLE_DECODER_HPP

#include "ndn-cxx/security/certificate.hpp"
#include "ndn-cxx/util/signal.hpp"

namespace ndn {
namespace security {
namespace detail {

/** @brief Helper class to decode a certificate bundle.
 */
class CertificateBundleDecoder
{
public:
  /**
   * @brief Append a bundle segment to the internal decoding buffer and trigger decoding.
   * @param block      Content element of the segment to be appended
   * @throw tlv::Error When the decoder encounters an unrecognized element with critical type.
   *                   No longer accepts segments after throwing.
   * @warning Must not be called from a handler of #onCertDecoded.
   * @warning Segments must be appended in order, otherwise the behavior is undefined.
   */
  void
  append(const Block& block);

  /**
   * @brief Whether the decoder has encountered an unrecoverable error.
   */
  bool
  hasError() const noexcept
  {
    return m_hasError;
  }

public:
  /**
   * @brief Emitted every time a certificate is successfully decoded.
   */
  util::Signal<CertificateBundleDecoder, Certificate> onCertDecoded;

private:
  void
  decode();

private:
  std::vector<uint8_t> m_bufferedData;
  bool m_hasError = false;
};

} // namespace detail
} // namespace security
} // namespace ndn

#endif // NDN_CXX_SECURITY_DETAIL_CERTIFICATE_BUNDLE_DECODER_HPP
