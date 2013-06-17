/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_VERIFIER_H
#define NDN_VERIFIER_H

namespace ndn {

/**
 * @brief Interface for NDN DATA packet verification (trust model)
 *
 * --- interface to verify DATA packets
 *     - application selects required implementation
 *     - at least two build-in models:
 *       = hierarchical (default)
 *       = simple chain (without authorization)
 *     - uses NDN keychain as a backend to cache certificates
 */
class Verifier
{
public:
  /**
   * @brief Virtual destructor
   */
  virtual
  ~Verifier ();
};

} // ndn

#endif // NDN_VERIFIER_H
