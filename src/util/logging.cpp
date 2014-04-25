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

#include "common.hpp"

#include "logging.hpp"

#ifdef NDN_CXX_HAVE_LOG4CXX

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/level.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/defaultconfigurator.h>
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;
using namespace log4cxx::helpers;

#include <unistd.h>

void
INIT_LOGGERS()
{
  static bool configured = false;

  if (configured) return;

  if (access("log4cxx.properties", R_OK)==0)
    PropertyConfigurator::configureAndWatch("log4cxx.properties");
  else
    {
      PatternLayoutPtr   layout  (new PatternLayout("%d{HH:mm:ss} %p %c{1} - %m%n"));
      ConsoleAppenderPtr appender(new ConsoleAppender(layout));

      BasicConfigurator::configure( appender );
      Logger::getRootLogger()->setLevel(log4cxx::Level::getInfo());
    }

  configured = true;
}

#endif // NDN_CXX_HAVE_LOG4CXX
