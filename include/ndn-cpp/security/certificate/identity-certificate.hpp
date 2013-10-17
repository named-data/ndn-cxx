/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_IDENTITY_CERTIFICATE_HPP
#define NDN_IDENTITY_CERTIFICATE_HPP

#include "certificate.hpp"

namespace ndn {

class IdentityCertificate : public Certificate
{
public:
  /**
   * The default constructor.
   */
  IdentityCertificate()
  {
  }

  /**
   * Create an IdentityCertificate from the content in the data packet.
   * @param data The data packet with the content to decode.
   */
  IdentityCertificate(const Data& data);
  
  /**
   * The virtual destructor.
   */
  virtual 
  ~IdentityCertificate();
  
  /**
   * Override the base class method to check that the name is a valid identity certificate name.
   * @param name The identity certificate name which is copied.
   * @return This Data so that you can chain calls to update values.
   */
  virtual Data &
  setName(const Name& name);

  virtual Name 
  getPublicKeyName() const;

  static bool
  isIdentityCertificate(const Certificate& certificate);

private:
  static bool
  isCorrectName(const Name& name);
};

}

#endif
