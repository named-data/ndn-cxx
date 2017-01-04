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

#ifndef NDN_TESTS_KEY_CHAIN_FIXTURE_HPP
#define NDN_TESTS_KEY_CHAIN_FIXTURE_HPP

#include "security/v2/key-chain.hpp"

#include "boost-test.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>

namespace ndn {
namespace tests {

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
    : m_pibDir(Path().PATH)
  {
    if (getenv("NDN_CLIENT_PIB") != nullptr) {
      m_oldPib = getenv("NDN_CLIENT_PIB");
    }
    if (getenv("NDN_CLIENT_TPM") != nullptr) {
      m_oldTpm = getenv("NDN_CLIENT_TPM");
    }

    /// @todo Consider change to an in-memory PIB/TPM
    setenv("NDN_CLIENT_PIB", ("pib-sqlite3:" + m_pibDir).c_str(), true);
    setenv("NDN_CLIENT_TPM", ("tpm-file:" + m_pibDir).c_str(), true);
  }

  ~PibDirFixture()
  {
    if (!m_oldPib.empty()) {
      setenv("NDN_CLIENT_PIB", m_oldPib.c_str(), true);
    }
    else {
      unsetenv("NDN_CLIENT_PIB");
    }

    if (!m_oldTpm.empty()) {
      setenv("NDN_CLIENT_TPM", m_oldTpm.c_str(), true);
    }
    else {
      unsetenv("NDN_CLIENT_TPM");
    }

    boost::filesystem::remove_all(m_pibDir);
    const_cast<std::string&>(security::v2::KeyChain::getDefaultPibLocator()).clear();
    const_cast<std::string&>(security::v2::KeyChain::getDefaultTpmLocator()).clear();
  }

protected:
  const std::string m_pibDir;

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
  createClientConf(std::initializer_list<std::string> lines)
  {
    boost::filesystem::create_directories(boost::filesystem::path(this->m_pibDir) / ".ndn");
    std::ofstream of((boost::filesystem::path(this->m_pibDir) / ".ndn" / "client.conf").c_str());
    for (auto line : lines) {
      boost::replace_all(line, "%PATH%", this->m_pibDir);
      of << line << std::endl;
    }
  }
};

struct DefaultPibDir
{
  const std::string PATH = "build/keys";
};

} // namespace tests
} // namespace ndn

#endif // NDN_TESTS_KEY_CHAIN_FIXTURE_HPP
