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

#ifndef NDN_TESTS_SECURITY_TPM_BACK_END_WRAPPER_FILE_HPP
#define NDN_TESTS_SECURITY_TPM_BACK_END_WRAPPER_FILE_HPP

#include "security/tpm/back-end-file.hpp"
#include <boost/filesystem.hpp>

namespace ndn {
namespace security {
namespace tpm {
namespace tests {

/**
 * @brief A wrapper of tpm::BackEndFile for unit test template.
 */
class BackEndWrapperFile
{
public:
  BackEndWrapperFile()
    : m_tmpPath(boost::filesystem::path(UNIT_TEST_CONFIG_PATH) / "TpmFileTest")
    , m_impl(make_unique<BackEndFile>(m_tmpPath.string()))
  {
  }

  ~BackEndWrapperFile()
  {
    boost::filesystem::remove_all(m_tmpPath);
  }

  BackEnd&
  getTpm()
  {
    return *m_impl;
  }

  std::string
  getScheme()
  {
    return "tpm-file";
  }

private:
  const boost::filesystem::path m_tmpPath;
  const unique_ptr<BackEnd> m_impl;
};

} // namespace tests
} // namespace tpm
} // namespace security
} // namespace ndn

#endif // NDN_TESTS_SECURITY_TPM_BACK_END_WRAPPER_FILE_HPP
