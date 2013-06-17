/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_WIRE_BASE_H
#define NDN_WIRE_BASE_H

#include "ndn-cpp/fields/signature-sha256-with-rsa.h"

namespace ndn {
namespace wire {

/**
 * @brief Class defining interface for double dispatch pattern (=visitor pattern)
 *        to format variable fields in wire format
 */
class Base
{
public:
  virtual void
  appendSignature (std::ostream &os, const signature::Sha256WithRsa &signature, void *userData) = 0;
};

} // wire
} // ndn

#endif // NDN_WIRE_BASE_H
