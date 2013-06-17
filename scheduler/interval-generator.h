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

#ifndef INTERVAL_GENERATOR_H
#define INTERVAL_GENERATOR_H

#include <boost/shared_ptr.hpp>

using namespace std;

class IntervalGenerator;
typedef boost::shared_ptr<IntervalGenerator> IntervalGeneratorPtr;

class IntervalGenerator
{
public:
  virtual ~IntervalGenerator () { }
  
  virtual double
  nextInterval() = 0;
};


#endif // INTERVAL_GENERATOR_H
