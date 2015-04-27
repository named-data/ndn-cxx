/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#ifndef NDN_SECURITY_PIB_HPP
#define NDN_SECURITY_PIB_HPP

#include "../common.hpp"

namespace ndn {
namespace security {

/**
 * @brief represents the PIB
 *
 * The PIB (Public Information Base) stores the public portion of a user's cryptography keys.
 * The format and location of stored information is indicated by the PibLocator.
 * The PIB is designed to work with a TPM (Trusted Platform Module) which stores private keys.
 * There is a one-to-one association between PIB and TPM, and therefore the TpmLocator is recorded
 * by the PIB to enforce this association and prevent one from operating on mismatched PIB and TPM.
 *
 * Information in the PIB is organized in a hierarchy of Identity-Key-Certificate. At the top level,
 * the Pib class provides access to identities, and allows setting a default identity. Properties of
 * an identity can be accessed after obtaining an Identity object.
 *
 * @throw PibImpl::Error when underlying implementation has non-semantic error.
 */
class Pib : noncopyable
{
public:
  /// @brief represents a semantic error
  class Error : public std::runtime_error
  {
  public:
    explicit
    Error(const std::string& what)
      : std::runtime_error(what)
    {
    }
  };

  //TODO: Add Pib interfaces
};

} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_PIB_HPP
