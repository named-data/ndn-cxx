/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 */

#ifndef NDN_MANAGEMENT_CONFIG_FILE_HPP
#define NDN_MANAGEMENT_CONFIG_FILE_HPP

#include "../common.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem.hpp>

namespace ndn {

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
   * Locate, open, and parse a library configuration file.
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
   * Parse a previously discovered and opened configuration file.
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
   * Looking for the configuration file in these well-known locations:
   *
   * 1. $HOME/.ndn/client.conf
   * 2. @SYSCONFDIR@/ndn/client.conf
   * 3. /etc/ndn/client.conf
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
