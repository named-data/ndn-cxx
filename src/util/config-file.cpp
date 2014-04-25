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

#include "config-file.hpp"

#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem.hpp>

namespace ndn {

ConfigFile::ConfigFile()
  : m_path(findConfigFile())
{
  if (open())
    {
      parse();
      close();
    }
}

ConfigFile::~ConfigFile()
{
  if (m_input.is_open())
    {
      m_input.close();
    }
}

boost::filesystem::path
ConfigFile::findConfigFile()
{
  using namespace boost::filesystem;

#ifdef NDN_CXX_HAVE_TESTS
  if (std::getenv("TEST_HOME"))
    {
      path testHome(std::getenv("TEST_HOME"));
      testHome /= ".ndn/client.conf";
      if (exists(testHome))
        {
          return absolute(testHome);
        }
    }
#endif // NDN_CXX_HAVE_TESTS

  if (std::getenv("HOME"))
    {
      path home(std::getenv("HOME"));
      home /= ".ndn/client.conf";
      if (exists(home))
        {
          return absolute(home);
        }
    }

#ifdef NDN_CXX_SYSCONFDIR
  path sysconfdir(NDN_CXX_SYSCONFDIR);
  sysconfdir /= "ndn/client.conf";

  if (exists(sysconfdir))
    {
      return absolute(sysconfdir);
    }
#endif // NDN_CXX_SYSCONFDIR

  path etc("/etc/ndn/client.conf");
  if (exists(etc))
    {
      return absolute(etc);
    }

  return path();
}



bool
ConfigFile::open()
{
  if (m_path.empty())
    {
      return false;
    }

  m_input.open(m_path.c_str());
  if (!m_input.good() || !m_input.is_open())
    {
      return false;
    }
  return true;
}

void
ConfigFile::close()
{
  if (m_input.is_open())
    {
      m_input.close();
    }
}


const ConfigFile::Parsed&
ConfigFile::parse()
{
  if (m_path.empty())
    {
      throw Error("Failed to locate configuration file for parsing");
    }
  else if (!m_input.is_open() && !open())
    {
      throw Error("Failed to open configuration file for parsing");
    }

  try
    {
      boost::property_tree::read_ini(m_input, m_config);
    }
  catch (boost::property_tree::ini_parser_error& error)
    {
      std::stringstream msg;
      msg << "Failed to parse configuration file";
      msg << " " << m_path;
      msg << " " << error.message() << " line " << error.line();
      throw Error(msg.str());
    }
  return m_config;
}

}
