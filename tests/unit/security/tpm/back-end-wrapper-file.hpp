/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2024 Regents of the University of California.
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

#ifndef NDN_CXX_TESTS_UNIT_SECURITY_TPM_BACK_END_WRAPPER_FILE_HPP
#define NDN_CXX_TESTS_UNIT_SECURITY_TPM_BACK_END_WRAPPER_FILE_HPP

#include "ndn-cxx/security/tpm/impl/back-end-file.hpp"

#include <filesystem>

namespace ndn::tests {

/**
 * @brief A wrapper of tpm::BackEndFile for unit test template.
 */
class BackEndWrapperFile
{
public:
  BackEndWrapperFile()
    : m_tmpPath(std::filesystem::path(UNIT_TESTS_TMPDIR) / "TpmBackEndFile")
    , m_impl(make_unique<security::tpm::BackEndFile>(m_tmpPath))
  {
  }

  ~BackEndWrapperFile()
  {
    std::filesystem::remove_all(m_tmpPath);
  }

  security::tpm::BackEnd&
  getTpm()
  {
    return *m_impl;
  }

  std::string
  getScheme() const
  {
    return "tpm-file";
  }

private:
  const std::filesystem::path m_tmpPath;
  const unique_ptr<security::tpm::BackEnd> m_impl;
};

} // namespace ndn::tests

#endif // NDN_CXX_TESTS_UNIT_SECURITY_TPM_BACK_END_WRAPPER_FILE_HPP
