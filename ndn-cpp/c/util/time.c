/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <sys/time.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "time.h"

ndn_MillisecondsSince1970 
ndn_getNowMilliseconds()
{
  struct timeval t;
  gettimeofday(&t, 0);
  return t.tv_sec * 1000.0 + t.tv_usec / 1000.0;
}

void
ndn_toIsoString(ndn_MillisecondsSince1970 time, char *isoString)
{
  double secondsSince1970 = time / 1000.0;
  char fractionBuffer[10];
  sprintf(fractionBuffer, "%.06lf", fmod(secondsSince1970, 1.0));
  const char *fraction = strchr(fractionBuffer, '.');
  if (!fraction)
    // Don't expect this to happen.
    fraction = ".000000";
    
  time_t seconds = secondsSince1970;
  struct tm* gmt = gmtime(&seconds);
  sprintf(isoString, "%04d%02d%02dT%02d%02d%02d%s", 1900 + gmt->tm_year, gmt->tm_mon + 1, gmt->tm_mday,
    gmt->tm_hour, gmt->tm_min, gmt->tm_sec, fraction);
}

ndn_MillisecondsSince1970
ndn_fromIsoString(const char* isoString)
{
  // Initialize time zone, etc.
  time_t dummyTime = 0;
  struct tm time = *gmtime(&dummyTime);
  
  sscanf(isoString, "%4d%2d%2dT%2d%2d", &time.tm_year, &time.tm_mon, &time.tm_mday, &time.tm_hour, &time.tm_min);
  // Skip the time past minutes and get the float seconds.
  double seconds;
  sscanf(isoString + (4 + 2 + 2 + 1 + 2 + 2), "%lf", &seconds);
  
  // tm_year starts from 1900.
  time.tm_year -= 1900;
  // tm_mon starts from 0, not 1.
  time.tm_mon -= 1;
  time.tm_sec = 0;
  return (timegm(&time) + seconds) * 1000.0;  
}