/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_DATA_H
#define NDN_DATA_H

#include "ndn-cpp/fields/name.h"
#include "ndn-cpp/fields/content.h"
#include "ndn-cpp/fields/signature.h"
#include "ndn-cpp/fields/signed-blob.h"

namespace ndn {

/**
 * @brief Class implementing abstractions to work with NDN Data packets
 */
class Data
{
public:
  /**
   * @brief Create an empty Data with empty payload
   **/
  Data ();

  /**
   * @brief Destructor
   */
  ~Data ();

  /**
   * @brief Set data packet name
   * @param name name of the data packet
   * @return reference to self (to allow method chaining)
   *
   * In some cases, a direct access to and manipulation of name using getName is more efficient
   */
  inline Data &
  setName (const Name &name);

  /**
   * @brief Get data packet name (const reference)
   * @returns name of the data packet
   */
  inline const Name &
  getName () const;

  /**
   * @brief Get data packet name (reference)
   * @returns name of the data packet
   */
  inline Name &
  getName ();

  /**
   * @brief Get const smart pointer to signature object
   */
  inline ConstSignaturePtr
  getSignature () const;

  /**
   * @brief Get smart pointer to signature object
   */
  inline SignaturePtr
  getSignature ();

  /**
   * @brief Set signature object
   * @param signature smart pointer to a signature object
   */
  inline void
  setSignature (SignaturePtr sigature);

  /**
   * @brief Get const reference to content object (content info + actual content)
   */
  inline const Content &
  getContent () const;

  /**
   * @brief Get reference to content object (content info + actual content)
   */
  inline Content &
  getContent ();

  /**
   * @brief Set content object (content info + actual content)
   * @param content reference to a content object
   *
   * More efficient way (that avoids copying):
   * @code
   * Content content (...);
   * getContent ().swap (content);
   * @endcode
   */
  inline void
  setContent (const Content &content);

  /**
   * @brief A helper method to directly access actual content data (const reference)
   *
   * This method is equivalent to
   * @code
   * getContent ().getContent ()
   * @endcode
   */
  inline const Blob &
  content () const;

  /**
   * @brief A helper method to directly access actual content data (reference)
   *
   * This method is equivalent to
   * @code
   * getContent ().getContent ()
   * @endcode
   */
  inline Blob &
  content ();

private:
  Name m_name;
  SignaturePtr m_signature; // signature with its parameters "binds" name and content
  Content m_content;

  SignedBlobPtr m_wire;  
};

inline Data &
Data::setName (const Name &name)
{
  m_name = name;
  return *this;
}

inline const Name &
Data::getName () const
{
  return m_name;
}

inline Name &
Data::getName ()
{
  return m_name;
}

inline ConstSignaturePtr
Data::getSignature () const
{
  return m_signature;
}

inline SignaturePtr
Data::getSignature ()
{
  return m_signature;
}

inline void
Data::setSignature (SignaturePtr signature)
{
  m_signature = signature;
}

inline const Content &
Data::getContent () const
{
  return m_content;
}

inline Content &
Data::getContent ()
{
  return m_content;
}

inline void
Data::setContent (const Content &content)
{
  m_content = content;
}

inline const Blob &
Data::content () const
{
  return getContent ().getContent ();
}

inline Blob &
Data::content ()
{
  return getContent ().getContent ();
}

} // namespace ndn

#endif // NDN_DATA_H
