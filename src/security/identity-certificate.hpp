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
  struct Error : public std::runtime_error { Error(const std::string &what) : std::runtime_error(what) {} };

  /**
   * The default constructor.
   */
  inline
  IdentityCertificate();

  // Note: The copy constructor works because publicKeyName_ has a copy constructor.

  /**
   * Create an IdentityCertificate from the content in the data packet.
   * @param data The data packet with the content to decode.
   */
  inline
  IdentityCertificate(const Data& data);
  
  /**
   * The virtual destructor.
   */
  inline virtual 
  ~IdentityCertificate();
  
  inline void
  wireDecode(const Block &wire);

  inline void
  setName(const Name &name);
  
  inline const Name &
  getPublicKeyName () const;

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

inline
IdentityCertificate::IdentityCertificate()
{
}

inline
IdentityCertificate::IdentityCertificate(const Data& data)
  : Certificate(data)
{
  setPublicKeyName();
}
  
inline
IdentityCertificate::~IdentityCertificate()
{
}

inline void
IdentityCertificate::wireDecode(const Block &wire)
{
  Certificate::wireDecode(wire);
  setPublicKeyName();
}

inline void
IdentityCertificate::setName(const Name &name)
{
  Certificate::setName(name);
  setPublicKeyName();
}

inline const Name &
IdentityCertificate::getPublicKeyName () const
{
  return publicKeyName_;
}

}

#endif
