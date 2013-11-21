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

  // Note: The copy constructor works because publicKeyName_ has a copy constructor.

  /**
   * Create an IdentityCertificate from the content in the data packet.
   * @param data The data packet with the content to decode.
   */
  IdentityCertificate(const Data& data);
  
  /**
   * The copy constructor. 
   */
  IdentityCertificate(const IdentityCertificate& identityCertificate);
  
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

  Name 
  getPublicKeyName () const { return publicKeyName_; }

  static bool
  isIdentityCertificate(const Certificate& certificate);

  /**
   * Get the public key name from the full certificate name.
   * @param certificateName The full certificate name.
   * @return The related public key name.
   */
  static Name
  certificateNameToPublicKeyName(const Name& certificateName);
  
private:
  static bool
  isCorrectName(const Name& name);
  
  void
  setPublicKeyName();
    
protected:
  Name publicKeyName_;
};

}

#endif
