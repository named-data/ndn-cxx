/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2023 Regents of the University of California.
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

#ifndef NDN_CXX_MGMT_NFD_COMMAND_OPTIONS_HPP
#define NDN_CXX_MGMT_NFD_COMMAND_OPTIONS_HPP

#include "ndn-cxx/security/signing-info.hpp"

namespace ndn::nfd {

/**
 * \ingroup management
 * \brief Contains options for ControlCommand execution.
 * \note This type is intentionally copyable.
 */
class CommandOptions
{
public:
  /**
   * \brief Constructs CommandOptions.
   * \post getTimeout() == DEFAULT_TIMEOUT
   * \post getPrefix() == DEFAULT_PREFIX
   * \post getSigningInfo().getSignerType() == SIGNER_TYPE_NULL
   */
  CommandOptions() = default;

  /**
   * \brief Returns the command timeout.
   */
  time::milliseconds
  getTimeout() const
  {
    return m_timeout;
  }

  /**
   * \brief Sets the command timeout.
   * \param timeout the new command timeout, must be positive
   * \return self
   * \throw std::out_of_range if timeout is non-positive
   */
  CommandOptions&
  setTimeout(time::milliseconds timeout);

  /**
   * \brief Returns the command prefix.
   */
  const Name&
  getPrefix() const
  {
    return m_prefix;
  }

  /**
   * \brief Sets the command prefix.
   * \return self
   */
  CommandOptions&
  setPrefix(Name prefix);

  /**
   * \brief Returns the signing parameters.
   */
  const security::SigningInfo&
  getSigningInfo() const
  {
    return m_signingInfo;
  }

  /**
   * \brief Sets the signing parameters.
   * \return self
   */
  CommandOptions&
  setSigningInfo(security::SigningInfo signingInfo);

public:
  /// The default command timeout: 10 seconds.
  static constexpr time::milliseconds DEFAULT_TIMEOUT = 10_s;

  /// The default command prefix: `/localhost/nfd`.
  static inline const Name DEFAULT_PREFIX{"/localhost/nfd"};

private:
  time::milliseconds m_timeout = DEFAULT_TIMEOUT;
  Name m_prefix = DEFAULT_PREFIX;
  security::SigningInfo m_signingInfo;
};

} // namespace ndn::nfd

#endif // NDN_CXX_MGMT_NFD_COMMAND_OPTIONS_HPP
