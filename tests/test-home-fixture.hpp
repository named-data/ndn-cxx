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

#ifndef NDN_CXX_TESTS_TEST_HOME_FIXTURE_HPP
#define NDN_CXX_TESTS_TEST_HOME_FIXTURE_HPP

#include "ndn-cxx/security/key-chain.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <initializer_list>

#include <boost/algorithm/string/replace.hpp>

namespace ndn::tests {

/**
 * @brief Fixture to adjust/restore NDN_CLIENT_PIB and NDN_CLIENT_TPM paths
 *
 * Note that the specified PATH will be removed after fixture is destroyed.
 * **Do not specify non-temporary paths.**
 */
template<class Path>
class PibDirFixture
{
public:
  PibDirFixture()
  {
    if (const char* envPib = std::getenv("NDN_CLIENT_PIB"); envPib != nullptr) {
      m_oldPib = envPib;
    }
    if (const char* envTpm = std::getenv("NDN_CLIENT_TPM"); envTpm != nullptr) {
      m_oldTpm = envTpm;
    }

    /// @todo Consider change to an in-memory PIB/TPM
    setenv("NDN_CLIENT_PIB", ("pib-sqlite3:" + m_pibDir).c_str(), true);
    setenv("NDN_CLIENT_TPM", ("tpm-file:" + m_pibDir).c_str(), true);
  }

  ~PibDirFixture()
  {
    if (!m_oldPib.empty()) {
      setenv("NDN_CLIENT_PIB", m_oldPib.data(), true);
    }
    else {
      unsetenv("NDN_CLIENT_PIB");
    }

    if (!m_oldTpm.empty()) {
      setenv("NDN_CLIENT_TPM", m_oldTpm.data(), true);
    }
    else {
      unsetenv("NDN_CLIENT_TPM");
    }

    std::filesystem::remove_all(m_pibDir);
    KeyChain::resetDefaultLocators();
  }

protected:
  const std::string m_pibDir{Path::PATH};

private:
  std::string m_oldPib;
  std::string m_oldTpm;
};

/**
 * @brief Extension of PibDirFixture to set TEST_HOME variable and allow config file creation
 */
template<class Path>
class TestHomeFixture : public PibDirFixture<Path>
{
public:
  TestHomeFixture()
  {
    setenv("TEST_HOME", this->m_pibDir.c_str(), true);
  }

  ~TestHomeFixture()
  {
    unsetenv("TEST_HOME");
  }

  void
  createClientConf(std::initializer_list<std::string> lines) const
  {
    auto ndnDir = std::filesystem::path(this->m_pibDir) / ".ndn";
    std::filesystem::create_directories(ndnDir);
    std::ofstream of(ndnDir / "client.conf");
    for (auto line : lines) {
      boost::replace_all(line, "%PATH%", this->m_pibDir);
      of << line << std::endl;
    }
  }
};

struct DefaultPibDir
{
  static constexpr std::string_view PATH{"build/keys"};
};

} // namespace ndn::tests

#endif // NDN_CXX_TESTS_TEST_HOME_FIXTURE_HPP
