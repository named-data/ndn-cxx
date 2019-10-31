/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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

#ifndef NDN_TESTS_SECURITY_TPM_BACK_END_WRAPPER_OSX_HPP
#define NDN_TESTS_SECURITY_TPM_BACK_END_WRAPPER_OSX_HPP

#include "ndn-cxx/security/tpm/impl/back-end-osx.hpp"
#include "ndn-cxx/security/tpm/impl/key-handle-osx.hpp"

#include <cstdlib>

namespace ndn {
namespace security {
namespace tpm {
namespace tests {

/**
 * @brief A wrapper of tpm::BackEndOsx for unit test template.
 */
class BackEndWrapperOsx
{
public:
  BackEndWrapperOsx()
  {
    std::string oldHOME;
    if (std::getenv("OLD_HOME"))
      oldHOME = std::getenv("OLD_HOME");

    if (std::getenv("HOME"))
      m_HOME = std::getenv("HOME");

    if (!oldHOME.empty())
      setenv("HOME", oldHOME.data(), 1);
    else
      unsetenv("HOME");

    m_impl = make_unique<BackEndOsx>();
  }

  ~BackEndWrapperOsx()
  {
    if (!m_HOME.empty())
      setenv("HOME", m_HOME.data(), 1);
    else
      unsetenv("HOME");
  }

  BackEnd&
  getTpm()
  {
    return *m_impl;
  }

  std::string
  getScheme() const
  {
    return "tpm-osxkeychain";
  }

private:
  std::string m_HOME;
  unique_ptr<BackEnd> m_impl;
};

} // namespace tests
} // namespace tpm
} // namespace security
} // namespace ndn

#endif // NDN_TESTS_SECURITY_TPM_BACK_END_WRAPPER_OSX_HPP
