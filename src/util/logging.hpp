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
 *
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 * @author Zhenkai Zhu <http://irl.cs.ucla.edu/~zhenkai/>
 */

#ifndef NDN_LOGGING_HPP
#define NDN_LOGGING_HPP

#include "../common.hpp"

#ifdef NDN_CXX_HAVE_LOG4CXX

#include <log4cxx/logger.h>

#define MEMBER_LOGGER                           \
  static log4cxx::LoggerPtr staticModuleLogger

#define INIT_MEMBER_LOGGER(className,name)          \
  log4cxx::LoggerPtr className::staticModuleLogger =  log4cxx::Logger::getLogger(name)

#define INIT_LOGGER(name) \
  static log4cxx::LoggerPtr staticModuleLogger = log4cxx::Logger::getLogger(name)

#define _LOG_DEBUG(x) \
  LOG4CXX_DEBUG(staticModuleLogger, x)

#define _LOG_TRACE(x) \
  LOG4CXX_TRACE(staticModuleLogger, x)

#define _LOG_FUNCTION(x) \
  LOG4CXX_TRACE(staticModuleLogger, __FUNCTION__ << "(" << x << ")")

#define _LOG_FUNCTION_NOARGS \
  LOG4CXX_TRACE(staticModuleLogger, __FUNCTION__ << "()")

#define _LOG_ERROR(x) \
  LOG4CXX_ERROR(staticModuleLogger, x)

#define _LOG_ERROR_COND(cond,x) \
  if (cond) { _LOG_ERROR(x) }

#define _LOG_DEBUG_COND(cond,x) \
  if (cond) { _LOG_DEBUG(x) }

void
INIT_LOGGERS()

#else // else NDN_CXX_HAVE_LOG4CXX

#define INIT_LOGGER(name) struct LOGGING_DISABLED
#define _LOG_FUNCTION(x)
#define _LOG_FUNCTION_NOARGS
#define _LOG_TRACE(x)
#define INIT_LOGGERS(x)
#define _LOG_ERROR(x)
#define _LOG_ERROR_COND(cond,x)
#define _LOG_DEBUG_COND(cond,x)

#define MEMBER_LOGGER
#define INIT_MEMBER_LOGGER(className,name)

#ifdef _DEBUG

#include <iostream>

#define _LOG_DEBUG(x) \
  { std::clog << x << std::endl; }

#else
#define _LOG_DEBUG(x)
#endif

#endif // NDN_CXX_HAVE_LOG4CXX

#endif
