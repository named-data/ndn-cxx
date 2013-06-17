/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *                     Zhenkai Zhu
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 *         Zhenkai Zhu <zhenkai@cs.ucla.edu>
 */

#include "logging.h"

#ifdef HAVE_LOG4CXX

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
INIT_LOGGERS ()
{
  static bool configured = false;

  if (configured) return;

  if (access ("log4cxx.properties", R_OK)==0)
    PropertyConfigurator::configureAndWatch ("log4cxx.properties");
  else
    {
      PatternLayoutPtr   layout   (new PatternLayout ("%d{HH:mm:ss} %p %c{1} - %m%n"));
      ConsoleAppenderPtr appender (new ConsoleAppender (layout));

      BasicConfigurator::configure( appender );
      Logger::getRootLogger()->setLevel (log4cxx::Level::getInfo ());
    }

  configured = true;
}

#endif
