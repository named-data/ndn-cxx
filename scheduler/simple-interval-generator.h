/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *                     Zhenkai Zhu
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Zhenkai Zhu <zhenkai@cs.ucla.edu>
 *         Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef SIMPLE_INTERVAL_GENERATOR_H
#define SIMPLE_INTERVAL_GENERATOR_H

#include "interval-generator.h"

class SimpleIntervalGenerator : public IntervalGenerator
{
public:
  SimpleIntervalGenerator(double interval) : m_interval (interval) {}
  virtual ~SimpleIntervalGenerator() {}
  
  virtual double
  nextInterval() _OVERRIDE { return m_interval; }

private:
  double m_interval;
};

#endif // SIMPLE_INTERVAL_GENERATOR_H
