/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_EXCLUDE_H
#define NDN_EXCLUDE_H

#include "ndn-cpp/fields/name-component.h"

#include <map>

namespace ndn {

/**
 * @brief Class to represent Exclude component in NDN interests
 */
class Exclude
{
public:
  typedef std::map< name::Component, bool /*any*/, std::greater<name::Component> > exclude_type;

  typedef exclude_type::iterator iterator;
  typedef exclude_type::const_iterator const_iterator;
  typedef exclude_type::reverse_iterator reverse_iterator;
  typedef exclude_type::const_reverse_iterator const_reverse_iterator;

  /**
   * @brief Default constructor an empty exclude
   */
  Exclude ();

  /**
   * @brief Check if name component is excluded
   * @param comp Name component to check against exclude filter
   */
  bool
  isExcluded (const name::Component &comp) const;

  /**
   * @brief Exclude specific name component
   * @param comp component to exclude
   * @returns *this to allow chaining
   */
  Exclude &
  excludeOne (const name::Component &comp);

  /**
   * @brief Exclude components from range [from, to]
   * @param from first element of the range
   * @param to last element of the range
   * @returns *this to allow chaining
   */
  Exclude &
  excludeRange (const name::Component &from, const name::Component &to);

  /**
   * @brief Exclude all components from range [/, to]
   * @param to last element of the range
   * @returns *this to allow chaining
   */
  inline Exclude &
  excludeBefore (const name::Component &to);

  /**
   * @brief Exclude all components from range [from, +Inf]
   * @param to last element of the range
   * @returns *this to allow chaining
   */
  Exclude &
  excludeAfter (const name::Component &from);

  /**
   * @brief Method to directly append exclude element
   * @param name excluded name component
   * @param any  flag indicating if there is a postfix ANY component after the name
   *
   * This method is used during conversion from wire format of exclude filter
   *
   * If there is an error with ranges (e.g., order of components is wrong) an exception is thrown
   */
  void
  appendExclude (const name::Component &name, bool any);

  /**
   * @brief Get number of exclude terms
   */
  inline size_t
  size () const;

  /**
   * @brief Get begin iterator of the exclude terms
   */
  inline const_iterator
  begin () const;

  /**
   * @brief Get end iterator of the exclude terms
   */
  inline const_iterator
  end () const;

  /**
   * @brief Get begin iterator of the exclude terms
   */
  inline const_reverse_iterator
  rbegin () const;

  /**
   * @brief Get end iterator of the exclude terms
   */
  inline const_reverse_iterator
  rend () const;

private:
  Exclude &
  excludeRange (iterator fromLowerBound, iterator toLowerBound);

private:
  exclude_type m_exclude;
};

std::ostream&
operator << (std::ostream &os, const Exclude &name);

inline Exclude &
Exclude::excludeBefore (const name::Component &to)
{
  return excludeRange (name::Component (), to);
}

inline size_t
Exclude::size () const
{
  return m_exclude.size ();
}

inline Exclude::const_iterator
Exclude::begin () const
{
  return m_exclude.begin ();
}

inline Exclude::const_iterator
Exclude::end () const
{
  return m_exclude.end ();
}

inline Exclude::const_reverse_iterator
Exclude::rbegin () const
{
  return m_exclude.rbegin ();
}

inline Exclude::const_reverse_iterator
Exclude::rend () const
{
  return m_exclude.rend ();
}

} // ndn

#endif // NDN_EXCLUDE_H
