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
