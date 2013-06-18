/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_SIGNATURE_H
#define NDN_SIGNATURE_H

#include <iostream>
#include <boost/shared_ptr.hpp>

namespace ndn {

namespace wire { class Base; }

class Signature;
typedef boost::shared_ptr<Signature> SignaturePtr;
typedef boost::shared_ptr<const Signature> ConstSignaturePtr;

/**
 * @brief Pure virtual class providing an interface to work with signatures for NDN data packets
 */
class Signature
{
public:
  /**
   * @brief Virtual destructor
   */
  virtual
  ~Signature () { }

  /**
   * @brief A double dispatch pattern to call the right wireFormatter method to format signature
   * @param os reference to output stream
   * @param wireFormatter a reference to a wireFormatter object
   * @param userData any user-specific data
   */
  virtual void
  doubleDispatch (std::ostream &os, wire::Base &wireFormatter, void *userData) const = 0;
};

} // ndn

#endif // NDN_SIGNATURE_H
