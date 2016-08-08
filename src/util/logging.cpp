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

#include "logging.hpp"
#include "logger.hpp"

#include <boost/log/expressions.hpp>
#include <cstdlib>
#include <fstream>

namespace ndn {
namespace util {

static const LogLevel INITIAL_DEFAULT_LEVEL = LogLevel::NONE;

Logging&
Logging::get()
{
  // Initialization of block-scope variables with static storage duration is thread-safe.
  // See ISO C++ standard [stmt.dcl]/4
  static Logging instance;
  return instance;
}

Logging::Logging()
{
  this->setDestinationImpl(shared_ptr<std::ostream>(&std::clog, bind([]{})));

  const char* environ = std::getenv("NDN_LOG");
  if (environ != nullptr) {
    this->setLevelImpl(environ);
  }
}

void
Logging::addLoggerImpl(Logger& logger)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  const std::string& moduleName = logger.getModuleName();
  m_loggers.insert({moduleName, &logger});

  auto levelIt = m_enabledLevel.find(moduleName);
  if (levelIt == m_enabledLevel.end()) {
    levelIt = m_enabledLevel.find("*");
  }
  LogLevel level = levelIt == m_enabledLevel.end() ? INITIAL_DEFAULT_LEVEL : levelIt->second;
  logger.setLevel(level);
}

#ifdef NDN_CXX_HAVE_TESTS
bool
Logging::removeLogger(Logger& logger)
{
  const std::string& moduleName = logger.getModuleName();
  auto range = m_loggers.equal_range(moduleName);
  for (auto i = range.first; i != range.second; ++i) {
    if (i->second == &logger) {
      m_loggers.erase(i);
      return true;
    }
  }
  return false;
}
#endif // NDN_CXX_HAVE_TESTS

void
Logging::setLevelImpl(const std::string& moduleName, LogLevel level)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  if (moduleName == "*") {
    this->setDefaultLevel(level);
    return;
  }

  m_enabledLevel[moduleName] = level;
  auto range = m_loggers.equal_range(moduleName);
  for (auto i = range.first; i != range.second; ++i) {
    i->second->setLevel(level);
  }
}

void
Logging::setDefaultLevel(LogLevel level)
{
  m_enabledLevel.clear();
  m_enabledLevel["*"] = level;

  for (auto i = m_loggers.begin(); i != m_loggers.end(); ++i) {
    i->second->setLevel(level);
  }
}

void
Logging::setLevelImpl(const std::string& config)
{
  std::stringstream ss(config);
  std::string configModule;
  while (std::getline(ss, configModule, ':')) {
    size_t ind = configModule.find('=');
    if (ind == std::string::npos) {
      BOOST_THROW_EXCEPTION(std::invalid_argument("malformed logging config: '=' is missing"));
    }

    std::string moduleName = configModule.substr(0, ind);
    LogLevel level = parseLogLevel(configModule.substr(ind+1));

    this->setLevelImpl(moduleName, level);
  }
}

#ifdef NDN_CXX_HAVE_TESTS
std::string
Logging::getLevels() const
{
  std::ostringstream os;

  auto defaultLevelIt = m_enabledLevel.find("*");
  if (defaultLevelIt != m_enabledLevel.end()) {
    os << "*=" << defaultLevelIt->second << ':';
  }

  for (auto it = m_enabledLevel.begin(); it != m_enabledLevel.end(); ++it) {
    if (it->first == "*") {
      continue;
    }
    os << it->first << '=' << it->second << ':';
  }

  std::string s = os.str();
  if (!s.empty()) {
    s.pop_back(); // delete last ':'
  }
  return s;
}
#endif // NDN_CXX_HAVE_TESTS

#ifdef NDN_CXX_HAVE_TESTS
void
Logging::resetLevels()
{
  this->setDefaultLevel(INITIAL_DEFAULT_LEVEL);
  m_enabledLevel.clear();
}
#endif // NDN_CXX_HAVE_TESTS

void
Logging::setDestination(std::ostream& os)
{
  setDestination(shared_ptr<std::ostream>(&os, bind([]{})));
}

void
Logging::setDestinationImpl(shared_ptr<std::ostream> os)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  m_destination = os;

  auto backend = boost::make_shared<boost::log::sinks::text_ostream_backend>();
  backend->auto_flush(true);
  backend->add_stream(boost::shared_ptr<std::ostream>(os.get(), bind([]{})));

  if (m_sink != nullptr) {
    boost::log::core::get()->remove_sink(m_sink);
    m_sink->flush();
    m_sink.reset();
  }

  m_sink = boost::make_shared<Sink>(backend);
  m_sink->set_formatter(boost::log::expressions::stream << boost::log::expressions::message);
  boost::log::core::get()->add_sink(m_sink);
}

#ifdef NDN_CXX_HAVE_TESTS
shared_ptr<std::ostream>
Logging::getDestination()
{
  return m_destination;
}
#endif // NDN_CXX_HAVE_TESTS

void
Logging::flushImpl()
{
  m_sink->flush();
}

} // namespace util
} // namespace ndn
