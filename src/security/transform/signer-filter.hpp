/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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

#ifndef NDN_CXX_SECURITY_TRANSFORM_SIGNER_FILTER_HPP
#define NDN_CXX_SECURITY_TRANSFORM_SIGNER_FILTER_HPP

#include "transform-base.hpp"
#include "../security-common.hpp"

namespace ndn {
namespace security {
namespace transform {

class PrivateKey;

/**
 * @brief The module to sign data.
 */
class SignerFilter : public Transform
{
public:
  /**
   * @brief Create a module to sign using digest algorithm @p algo and private key @p key
   */
  SignerFilter(DigestAlgorithm algo, const PrivateKey& key);

  ~SignerFilter();

private:
  /**
   * @brief Write data @p buf into signer
   *
   * @return The number of bytes that are actually accepted
   */
  size_t
  convert(const uint8_t* buf, size_t size) final;

  /**
   * @brief Finalize signing and write the signature into next module.
   */
  void
  finalize() final;

private:
  class Impl;
  const unique_ptr<Impl> m_impl;
};

unique_ptr<Transform>
signerFilter(DigestAlgorithm algo, const PrivateKey& key);

} // namespace transform
} // namespace security
} // namespace ndn

#endif // NDN_CXX_SECURITY_TRANSFORM_SIGNER_FILTER_HPP
