/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_KEY_LOCATOR_H
#define NDN_KEY_LOCATOR_H

#include "ndn-cpp/fields/blob.h"
#include "ndn-cpp/fields/name.h"

namespace ndn {

/**
 * @brief Class providing an interface to work with key locators in NDN data packets
 */
class KeyLocator
{
public:
  /**
   * @brief Key locator type
   *
   * Key locator can be only of the defined types, i.e., it cannot contain key bits and key name
   */
  enum Type
    {
      NOTSET=-1,   ///< @brief Unset key locator type, any attempt to use KeyLocator of NOTSET type will cause an exception
      KEY,         ///< @brief Key locator contains key bits
      CERTIFICATE, ///< @brief Key locator contains certificate bits
      KEYNAME      ///< @brief key locator contains name of the key
    };

  /**
   * @brief Default constructor
   */
  KeyLocator ();

  /**
   * @brief Copy constructor
   */
  KeyLocator (const KeyLocator &keyLocator);
  
  /**
   * @brief Destructor
   */
  ~KeyLocator ();

  /**
   * @brief Copy operator
   */
  KeyLocator &
  operator = (const KeyLocator &keyLocator);
  
  /**
   * @brief Set type of the key locator
   * @param type key locator type, @see Type
   *
   * If type of the key locator changes, setType will delete any previously allocated
   * data, allocate appropriate type and store it in m_data
   */
  void
  setType (Type type);

  /**
   * @brief Get type of the key locator
   */
  inline Type
  getType () const;

  /**
   * @brief Get const reference to key bits, associated with key locator
   *
   * If key locator type is not KEY, then an exception will be thrown
   */
  const Blob &
  getKey () const;

  /**
   * @brief Get reference to key bits, associated with key locator
   *
   * If key locator type is not KEY, then an exception will be thrown
   */
  Blob &
  getKey ();

  /**
   * @brief Set key bits, associated with key locator
   * @param key const reference to key bits
   *
   * If key locator type is not KEY, then an exception will be thrown
   */
  void
  setKey (const Blob &key);

  /**
   * @brief Get const reference to certificated bits, associated with key locator
   *
   * If key locator type is not CERTIFICATE, then an exception will be thrown
   */
  const Blob &
  getCertificate () const;

  /**
   * @brief Get reference to certificated bits, associated with key locator
   *
   * If key locator type is not CERTIFICATE, then an exception will be thrown
   */
  Blob &
  getCertificate ();

  /**
   * @brief Set certificated bits, associated with key locator
   * @param certificate const reference to certificate bits
   *
   * If key locator type is not CERTIFICATE, then an exception will be thrown
   */
  void
  setCertificate (const Blob &certificate);

  /**
   * @brief Get const reference to key name, associated with key locator
   *
   * If key locator type is not KEYNAME, then an exception will be thrown
   */
  const Name &
  getKeyName () const;

  /**
   * @brief Get reference to key name, associated with key locator
   *
   * If key locator type is not KEYNAME, then an exception will be thrown
   */
  Name &
  getKeyName ();

  /**
   * @brief Set key name, associated with key locator
   * @param name const reference to key name
   *
   * If key locator type is not KEYNAME, then an exception will be thrown
   */
  void
  setKeyName (const Name &name);

private:
  void
  deleteData ();
  
private:
  Type m_type;
  void *m_data;
};

inline KeyLocator::Type
KeyLocator::getType () const
{
  return m_type;
}


} // ndn

#endif // NDN_KEY_LOCATOR_H
