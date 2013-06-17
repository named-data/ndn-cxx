/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_KEYCHAIN_H
#define NDN_KEYCHAIN_H

// #include "ndn-cpp/fields/blob.h"
// #include "ndn-cpp/fields/name.h"

#include "identity.h"
#include "certificate.h"

#include <iostream>

namespace ndn {

/**
 * @brief Interface for a keychain operations
 *
 * Keychain has the following set of operations:
 *
 * --- interface to manage certificates and identities
 *     - identities are permanently stored
 *     - certificates can be cached (or stored permanently, if user is willing to)
 * --- interface to sign and encrypt data
 *
 */
class Keychain
{
public:
  /**
   * @brief Virtual destructor
   */
  virtual
  ~Keychain ();

  /////////////////////////////////////////////////////
  // interface to manage certificates and identities //
  /////////////////////////////////////////////////////

  /**
   * @brief Get default identity
   */
  virtual Ptr<const Identity>
  getDefaultIdentity () = 0;

  /**
   * @brief Get identity by name
   * @param identityName name of the requested identity
   */
  virtual Ptr<const Identity>
  getIdentity (const Name &identityName) = 0;

  /**
   * @brief Create a self-certified identity
   * @param identityName name of the identity to create
   */
  virtual Ptr<const Identity>
  generateIdentity (const Name &identityName) = 0;

  /**
   * @brief Create identity certification request
   * @param identity identity for which create the request
   * @param os output stream which will receive the request
   */
  virtual void
  requestIdentityCertificate (const Identity &identity, std::ostream &os) = 0;

  /**
   * @brief Issue a certificate using parameters from the input stream (formatted as request)
   * @param identity Identity which will be used to issue the certificate
   * @param is input stream from which to read parameters of the certificate
   *
   * @returns smart pointer to a signed certificate
   */
  virtual Ptr<const Certificate>
  issueCertificate (const Identity &identity, std::istream &is) = 0;

  /**
   * @brief Issue a certificate using parameters from the input stream (formatted as request)
   *
   * Same as another version, but using the default identity
   *
   * @returns smart pointer to a signed certificate
   */
  virtual Ptr<const Certificate>
  issueCertificate (std::istream &is) = 0;

  /**
   * @brief Install identity certificate
   * @param cert certificate to install
   */
  virtual void
  installIdentityCertificate (const Certificate &cert) = 0;

public:
  /////////////////////////////////////////////////////
  //       interface to sign and encrypt data        //
  /////////////////////////////////////////////////////

  /**
   * @brief Sign data using specified identity
   * @param identity selected identity to sign data
   * @param buffer pointer to the data to sign
   * @param size length of data to sign
   *
   * @return pointer to base class of a signature object (depending on identity,
   *         different types signature can be produced)
   */
  virtual Ptr<Signature>
  sign (const Identity &identity, const void *buffer, size_t size) = 0;

  // TBD
  // /**
  //  * @brief Decrypt data using the specified identity
  //  */
  // virtual ?
  // decrypt (Ptr<Identity> identity, const void *buffer, size_t size, ?) = 0;
};

} // ndn

#endif // NDN_KEYCHAIN_H
