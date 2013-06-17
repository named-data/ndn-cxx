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

#ifndef RANDOM_INTERVAL_GENERATOR_H
#define RANDOM_INTERVAL_GENERATOR_H

#include "interval-generator.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

// generates intervals with uniform distribution
class RandomIntervalGenerator : public IntervalGenerator
{
public:
  typedef enum
  {
    UP = 1,
    DOWN = 2,
    EVEN = 3
  } Direction;

public:
  // percent is random-range/interval; e.g. if interval is 10 and you wish the random-range to be 2
  // e.g. 9 ~ 11, percent = 0.2
  // direction shifts the random range; e.g. in the above example, UP would produce a range of
  // 10 ~ 12, DOWN of 8 ~ 10, and EVEN of 9 ~ 11
  RandomIntervalGenerator(double interval, double percent, Direction direction = EVEN)
  // : m_rng(time(NULL))
    : m_rng (static_cast<int> (boost::posix_time::microsec_clock::local_time().time_of_day ().total_nanoseconds ()))
  , m_dist(0.0, fractional(percent))
  , m_random(m_rng, m_dist)
  , m_direction(direction)
  , m_percent(percent)
  , m_interval(interval)
  { }

  virtual ~RandomIntervalGenerator(){}

  virtual double
  nextInterval() _OVERRIDE
  {
    double percent = m_random();
    double interval = m_interval;
    switch (m_direction)
      {
      case UP: interval = m_interval * (1.0 + percent); break;
      case DOWN: interval = m_interval * (1.0 - percent); break;
      case EVEN: interval = m_interval * (1.0 - m_percent/2.0 + percent); break;
      default: break;
      }

    return interval;
  }

private:
  inline double fractional(double x) { double dummy; return abs(modf(x, &dummy)); }

private:
  typedef boost::mt19937 RNG_TYPE;
  RNG_TYPE m_rng;
  boost::uniform_real<> m_dist;
  boost::variate_generator<RNG_TYPE &, boost::uniform_real<> > m_random;
  Direction m_direction;
  double m_percent;
  double m_interval;

};
#endif // RANDOM_INTERVAL_GENERATOR_H
