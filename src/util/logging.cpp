/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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

#include "logging.hpp"
#include "logger.hpp"

#include <boost/log/expressions.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/iterator_range.hpp>

#include <cstdlib>
#include <iostream>
#include <sstream>

// suppress warning caused by <boost/log/sinks/text_ostream_backend.hpp>
#ifdef __clang__
#pragma clang diagnostic ignored "-Wundefined-func-template"
#endif

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
  m_loggers.emplace(moduleName, &logger);

  LogLevel level = findLevel(moduleName);
  logger.setLevel(level);
}

std::set<std::string>
Logging::getLoggerNamesImpl() const
{
  std::lock_guard<std::mutex> lock(m_mutex);

  std::set<std::string> loggerNames;
  boost::copy(m_loggers | boost::adaptors::map_keys, std::inserter(loggerNames, loggerNames.end()));
  return loggerNames;
}

LogLevel
Logging::findLevel(const std::string& moduleName) const
{
  std::string mn = moduleName;
  while (!mn.empty()) {
    auto it = m_enabledLevel.find(mn);
    if (it != m_enabledLevel.end()) {
      return it->second;
    }
    size_t pos = mn.find_last_of('.');
    if (pos < mn.size() - 1) {
      mn = mn.substr(0, pos + 1);
    }
    else if (pos == mn.size() - 1) {
      mn.pop_back();
      pos = mn.find_last_of('.');
      if (pos != std::string::npos) {
        mn = mn.substr(0, pos + 1);
      }
      else {
        mn = "";
      }
    }
    else {
      mn = "";
    }
  }
  auto it = m_enabledLevel.find(mn);
  if (it != m_enabledLevel.end()) {
    return it->second;
  }
  else {
    return INITIAL_DEFAULT_LEVEL;
  }
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
Logging::setLevelImpl(const std::string& prefix, LogLevel level)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  if (prefix.empty() || prefix.back() == '*') {
    std::string p = prefix;
    if (!p.empty()) {
      p.pop_back();
    }

    for (auto i = m_enabledLevel.begin(); i != m_enabledLevel.end();) {
      if (i->first.compare(0, p.size(), p) == 0) {
        i = m_enabledLevel.erase(i);
      }
      else {
        ++i;
      }
    }
    m_enabledLevel[p] = level;

    for (auto&& it : m_loggers) {
      if (it.first.compare(0, p.size(), p) == 0) {
        it.second->setLevel(level);
      }
    }
  }
  else {
    m_enabledLevel[prefix] = level;
    auto range = boost::make_iterator_range(m_loggers.equal_range(prefix));
    for (auto&& it : range) {
      it.second->setLevel(level);
    }
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
    LogLevel level = parseLogLevel(configModule.substr(ind + 1));
    this->setLevelImpl(moduleName, level);
  }
}

#ifdef NDN_CXX_HAVE_TESTS
void
Logging::resetLevels()
{
  this->setLevelImpl("*", INITIAL_DEFAULT_LEVEL);
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

  m_destination = std::move(os);

  auto backend = boost::make_shared<boost::log::sinks::text_ostream_backend>();
  backend->auto_flush(true);
  backend->add_stream(boost::shared_ptr<std::ostream>(m_destination.get(), bind([]{})));

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
Logging::getDestination() const
{
  return m_destination;
}

void
Logging::setLevelImpl(const std::unordered_map<std::string, LogLevel>& prefixRules)
{
  resetLevels();
  for (const auto& rule : prefixRules) {
    setLevelImpl(rule.first, rule.second);
  }
}

const std::unordered_map<std::string, LogLevel>&
Logging::getLevels() const
{
  return m_enabledLevel;
}
#endif // NDN_CXX_HAVE_TESTS

void
Logging::flushImpl()
{
  m_sink->flush();
}

} // namespace util
} // namespace ndn
