/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *                     Zhenkai Zhu
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 *         Zhenkai Zhu <zhenkai@cs.ucla.edu>
 */

#ifndef NDN_NAME_H
#define NDN_NAME_H

#include "ndn-cpp/fields/name-component.h"
#include "ndn-cpp/common.h"

namespace ndn {

/**
 * @brief Class for NDN Name
 */
class Name
{
public:
  typedef std::vector<name::Component>::iterator iterator;
  typedef std::vector<name::Component>::const_iterator const_iterator;
  typedef std::vector<name::Component>::reverse_iterator reverse_iterator;
  typedef std::vector<name::Component>::const_reverse_iterator const_reverse_iterator;
  typedef std::vector<name::Component>::reference reference;
  typedef std::vector<name::Component>::const_reference const_reference;

  typedef name::Component partial_type;
  
  ///////////////////////////////////////////////////////////////////////////////
  //                              CONSTRUCTORS                                 //
  ///////////////////////////////////////////////////////////////////////////////

  /**
   * @brief Default constructor to create an empty name (zero components, or "/")
   */
  Name ();

  /**
   * @brief Copy constructor
   *
   * @param other reference to a NDN name object
   */
  Name (const Name &other);

  /**
   * @brief Create a name from URL string
   *
   * @param url URI-represented name
   */
  Name (const std::string &url);

  /**
   * @brief Create a name from a container of elements [begin, end)
   *
   * @param begin begin iterator of the container
   * @param end end iterator of the container
   */
  template<class Iterator>
  Name (Iterator begin, Iterator end);

  /**
   * @brief Assignment operator
   */
  Name &
  operator= (const Name &other);


  ///////////////////////////////////////////////////////////////////////////////
  //                                SETTERS                                    //
  ///////////////////////////////////////////////////////////////////////////////

  /**
   * @brief Append a binary blob as a name component
   *
   * @param comp a binary blob
   * @returns reference to self (to allow chaining of append methods)
   */
  inline Name &
  append (const name::Component &comp);

  /**
   * @brief Append a binary blob as a name component
   * @param comp a binary blob
   *
   * This version is a little bit more efficient, since it swaps contents of comp and newly added component
   *
   * Attention!!! This method has an intended side effect: content of comp becomes empty
   */
  inline Name &
  appendBySwap (name::Component &comp);
  
  /**
   * @brief Append components a container of elements [begin, end)
   *
   * @param begin begin iterator of the container
   * @param end end iterator of the container
   * @returns reference to self (to allow chaining of append methods)
   */
  template<class Iterator>
  inline Name &
  append (Iterator begin, Iterator end);

  /**
   * @brief Append components from another ndn::Name object
   *
   * @param comp reference to Name object
   * @returns reference to self (to allow chaining of append methods)
   */
  inline Name &
  append (const Name &comp);

  /**
   * @brief Append a string as a name component
   *
   * @param compStr a string
   * @returns reference to self (to allow chaining of append methods)
   *
   * No conversions will be done to the string.  The string is included in raw form,
   * without any leading '\0' symbols.
   */
  inline Name &
  append (const std::string &compStr);

  /**
   * @brief Append a binary blob as a name component
   *
   * @param buf pointer to the first byte of the binary blob
   * @param size length of the binary blob
   * @returns reference to self (to allow chaining of append methods)
   */
  inline Name &
  append (const void *buf, size_t size);

  /**
   * @brief Append network-ordered numeric component to the name
   *
   * @param number number to be encoded and added as a component
   *
   * Number is encoded and added in network order. Tail zero-bytes are not included.
   * For example, if the number is 1, then 1-byte binary blob will be added  0x01.
   * If the number is 256, then 2 binary blob will be added: 0x01 0x01
   *
   * If the number is zero, an empty component will be added
   */
  inline Name &
  appendNumber (uint64_t number);

  /**
   * @brief Append network-ordered numeric component to the name with marker
   *
   * @param number number to be encoded and added as a component
   * @param marker byte marker, specified by the desired naming convention
   *
   * Currently defined naming conventions of the marker:
   * - 0x00  sequence number
   * - 0xC1  control number
   * - 0xFB  block id
   * - 0xFD  version number
   *
   * This version is almost exactly as appendNumber, with exception that it adds initial marker.
   * The number is formatted in the exactly the same way.
   *
   * @see appendNumber
   */
  inline Name &
  appendNumberWithMarker (uint64_t number, unsigned char marker);

  /**
   * @brief Helper method to add sequence number to the name (marker = 0x00)
   * @param seqno sequence number
   * @see appendNumberWithMarker
   */
  inline Name &
  appendSeqNum (uint64_t seqno);

  /**
   * @brief Helper method to add control number to the name (marker = 0xC1)
   * @param control control number
   * @see appendNumberWithMarker
   */
  inline Name &
  appendControlNum (uint64_t control);

  /**
   * @brief Helper method to add block ID to the name (marker = 0xFB)
   * @param blkid block ID
   * @see appendNumberWithMarker
   */
  inline Name &
  appendBlkId (uint64_t blkid);

  /**
   * @brief Helper method to add version to the name (marker = 0xFD)
   * @param version fully formatted version in a desired format (e.g., timestamp).
   *                If version is Name::nversion, then the version number is automatically
   *                assigned based on UTC timestamp
   * @see appendNumberWithMarker
   */
  Name &
  appendVersion (uint64_t version = Name::nversion);

  ///////////////////////////////////////////////////////////////////////////////
  //                                GETTERS                                    //
  ///////////////////////////////////////////////////////////////////////////////

  /**
   * @brief Get number of the name components
   * @return number of name components
   */
  inline size_t
  size () const;

  /**
   * @brief Get binary blob of name component
   * @param index index of the name component.  If less than 0, then getting component from the back:
   *              get(-1) getting the last component, get(-2) is getting second component from back, etc.
   * @returns const reference to binary blob of the requested name component
   *
   * If index is out of range, an exception will be thrown
   */
  const name::Component &
  get (int index) const;

  /**
   * @brief Get binary blob of name component
   * @param index index of the name component.  If less than 0, then getting component from the back
   * @returns reference to binary blob of the requested name component
   *
   * If index is out of range, an exception will be thrown
   */
  name::Component &
  get (int index);

  /////
  ///// Iterator interface to name components
  /////
  inline Name::const_iterator
  begin () const;           ///< @brief Begin iterator (const)

  inline Name::iterator
  begin ();                 ///< @brief Begin iterator

  inline Name::const_iterator
  end () const;             ///< @brief End iterator (const)

  inline Name::iterator
  end ();                   ///< @brief End iterator

  inline Name::const_reverse_iterator
  rbegin () const;          ///< @brief Reverse begin iterator (const)

  inline Name::reverse_iterator
  rbegin ();                ///< @brief Reverse begin iterator

  inline Name::const_reverse_iterator
  rend () const;            ///< @brief Reverse end iterator (const)

  inline Name::reverse_iterator
  rend ();                  ///< @brief Reverse end iterator


  /////
  ///// Static helpers to convert name component to appropriate value
  /////

  /**
   * @brief Get a new name, constructed as a subset of components
   * @param pos Position of the first component to be copied to the subname
   * @param len Number of components to be copied. Value Name::npos indicates that all components till the end of the name.
   */
  Name
  getSubName (size_t pos = 0, size_t len = npos) const;

  /**
   * @brief Get prefix of the name
   * @param len length of the prefix
   * @param skip number of components to skip from beginning of the name
   */
  inline Name
  getPrefix (size_t len, size_t skip = 0) const;

  /**
   * @brief Get postfix of the name
   * @param len length of the postfix
   * @param skip number of components to skip from end of the name
   */
  inline Name
  getPostfix (size_t len, size_t skip = 0) const;

  /**
   * @brief Get text representation of the name (URI)
   */
  std::string
  toUri () const;

  /**
   * @brief Write name as URI to the specified output stream
   * @param os output stream
   */
  void
  toUri (std::ostream &os) const;

  /////////////////////////////////////////////////
  // Helpers and compatibility wrappers
  /////////////////////////////////////////////////

  /**
   * @brief Compare two names, using canonical ordering for each component
   * @return 0  They compare equal
   *         <0 If *this comes before other in the canonical ordering
   *         >0 If *this comes after in the canonical ordering
   */
  int
  compare (const Name &name) const;

  /**
   * @brief Check if to Name objects are equal (have the same number of components with the same binary data)
   */
  inline bool
  operator == (const Name &name) const;

  /**
   * @brief Check if two Name objects are not equal
   */
  inline bool
  operator != (const Name &name) const;

  /**
   * @brief Less or equal comparison of two name objects
   */
  inline bool
  operator <= (const Name &name) const;

  /**
   * @brief Less comparison of two name objects
   */
  inline bool
  operator < (const Name &name) const;

  /**
   * @brief Great or equal comparison of two name objects
   */
  inline bool
  operator >= (const Name &name) const;

  /**
   * @brief Great comparison of two name objects
   */
  inline bool
  operator > (const Name &name) const;

  /**
   * @brief Operator [] to simplify access to name components
   * @see get
   */
  inline name::Component &
  operator [] (int index);

  /**
   * @brief Operator [] to simplify access to name components
   * @see get
   */
  inline const name::Component &
  operator [] (int index) const;

  /**
   * @brief Create a new Name object, by copying components from first and second name
   */
  Name
  operator + (const Name &name) const;

  /**
   * @brief A wrapper for append method
   */
  template<class T>
  inline void
  push_back (const T &comp);

public:
  // Data Members (public):
  ///  Value returned by various member functions when they fail.
  const static size_t npos = static_cast<size_t> (-1);
  const static uint64_t nversion = static_cast<uint64_t> (-1);

private:
  std::vector<name::Component> m_comps;
};

typedef boost::shared_ptr<Name> NamePtr;

inline std::ostream &
operator << (std::ostream &os, const Name &name)
{
  name.toUri (os);
  return os;
}


/////////////////////////////////////////////////////////////////////////////////////
// Definition of inline methods
/////////////////////////////////////////////////////////////////////////////////////

template<class Iterator>
Name::Name (Iterator begin, Iterator end)
{
  append (begin, end);
}

inline Name &
Name::append (const name::Component &comp)
{
  if (comp.size () != 0)
    m_comps.push_back (comp);
  return *this;
}

inline Name &
Name::appendBySwap (name::Component &comp)
{
  if (comp.size () != 0)
    {
      Name::iterator newComp = m_comps.insert (end (), name::Component ());
      newComp->swap (comp);
    }
  return *this;
}

template<class Iterator>
inline Name &
Name::append (Iterator begin, Iterator end)
{
  for (Iterator i = begin; i != end; i++)
    {
      append (*i);
    }
  return *this;
}

Name &
Name::append (const Name &comp)
{
  if (this == &comp)
    {
      // have to double-copy if the object is self, otherwise results very frustrating (because we use vector...)
      return append (Name (comp.begin (), comp.end ()));
    }
  return append (comp.begin (), comp.end ());
}

Name &
Name::append (const std::string &compStr)
{
  name::Component comp (compStr);
  return appendBySwap (comp);
}

Name &
Name::append (const void *buf, size_t size)
{
  name::Component comp (buf, size);
  return appendBySwap (comp);
}

Name &
Name::appendNumber (uint64_t number)
{
  name::Component comp;
  name::Component::fromNumber (number).swap (comp);
  return appendBySwap (comp);
}

Name &
Name::appendNumberWithMarker (uint64_t number, unsigned char marker)
{
  name::Component comp;
  name::Component::fromNumberWithMarker (number, marker).swap (comp);
  return appendBySwap (comp);
}

inline Name &
Name::appendSeqNum (uint64_t seqno)
{
  return appendNumberWithMarker (seqno, 0x00);
}

inline Name &
Name::appendControlNum (uint64_t control)
{
  return appendNumberWithMarker (control, 0xC1);
}

inline Name &
Name::appendBlkId (uint64_t blkid)
{
  return appendNumberWithMarker (blkid, 0xFB);
}

inline size_t
Name::size () const
{
  return m_comps.size ();
}

/////
///// Iterator interface to name components
/////
inline Name::const_iterator
Name::begin () const
{
  return m_comps.begin ();
}

inline Name::iterator
Name::begin ()
{
  return m_comps.begin ();
}

inline Name::const_iterator
Name::end () const
{
  return m_comps.end ();
}

inline Name::iterator
Name::end ()
{
  return m_comps.end ();
}

inline Name::const_reverse_iterator
Name::rbegin () const
{
  return m_comps.rbegin ();
}

inline Name::reverse_iterator
Name::rbegin ()
{
  return m_comps.rbegin ();
}

inline Name::const_reverse_iterator
Name::rend () const
{
  return m_comps.rend ();
}


inline Name::reverse_iterator
Name::rend ()
{
  return m_comps.rend ();
}


//// helpers


inline Name
Name::getPrefix (size_t len, size_t skip/* = 0*/) const
{
  return getSubName (skip, len);
}

inline Name
Name::getPostfix (size_t len, size_t skip/* = 0*/) const
{
  return getSubName (size () - len - skip, len);
}


template<class T>
inline void
Name::push_back (const T &comp)
{
  append (comp);
}

inline bool
Name::operator ==(const Name &name) const
{
  return (compare (name) == 0);
}

inline bool
Name::operator !=(const Name &name) const
{
  return (compare (name) != 0);
}

inline bool
Name::operator <= (const Name &name) const
{
  return (compare (name) <= 0);
}

inline bool
Name::operator < (const Name &name) const
{
  return (compare (name) < 0);
}

inline bool
Name::operator >= (const Name &name) const
{
  return (compare (name) >= 0);
}

inline bool
Name::operator > (const Name &name) const
{
  return (compare (name) > 0);
}

inline name::Component &
Name::operator [] (int index)
{
  return get (index);
}

inline const name::Component &
Name::operator [] (int index) const
{
  return get (index);
}

} // ndn

#endif
