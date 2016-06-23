/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
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

#ifndef NDN_MANAGEMENT_CONFIG_FILE_HPP
#define NDN_MANAGEMENT_CONFIG_FILE_HPP

#include "../common.hpp"

#include <fstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem.hpp>

namespace ndn {

/**
 * @brief System configuration file for NDN platform
 *
 * The config file controls the default transport to connect to NDN forwarder, type and
 * location of PIB and TPM.
 *
 * Looking for the configuration file in these well-known locations (in order):
 *
 * - `$HOME/.ndn/client.conf`
 * - `@SYSCONFDIR@/ndn/client.conf`
 * - `/etc/ndn/client.conf`
 *
 * @sa Manpage of ndn-client.conf
 */
class ConfigFile : noncopyable
{
public:

  class Error : public std::runtime_error
  {
  public:
    Error(const std::string& what)
      : std::runtime_error(what)
    {

    }
  };

  typedef boost::property_tree::ptree Parsed;

  /**
   * @brief Locate, open, and parse a library configuration file.
   *
   * @throws ConfigFile::Error on parse error
   */
  ConfigFile();

  ~ConfigFile();

  const boost::filesystem::path&
  getPath() const;

  const Parsed&
  getParsedConfiguration() const;

private:

  bool
  open();

  void
  close();

  /**
   * @brief Parse a previously discovered and opened configuration file.
   *
   * For convenience this method will attempt to open the file
   * if it has previously been located, but open() has not been called.
   *
   * @throws ConfigFile::Error on parse error
   * @throws ConfigFile::Error on failure to open previously un-open configuration file
   * @throws ConfigFile::Error if no configuration file was previously located
   */
  const Parsed&
  parse();

  /**
   * @brief Find the configuration file in well-known locations
   *
   * The well-known locations include (in order):
   *
   * - `$HOME/.ndn/client.conf`
   * - `@SYSCONFDIR@/ndn/client.conf`
   * - `/etc/ndn/client.conf`
   *
   * @return path to preferred configuration (according to above order) or empty path on failure
   */

  boost::filesystem::path
  findConfigFile();

private:
  boost::filesystem::path m_path; // absolute path to active configuration file (if any)
  std::ifstream m_input;
  Parsed m_config;
};

inline const boost::filesystem::path&
ConfigFile::getPath() const
{
  return m_path;
}

inline const ConfigFile::Parsed&
ConfigFile::getParsedConfiguration() const
{
  return m_config;
}

} // namespace ndn


#endif // NDN_MANAGEMENT_CONFIG_FILE_HPP
