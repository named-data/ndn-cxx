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

#ifndef NDN_INTEREST_H
#define NDN_INTEREST_H

#include <ndn-cpp/common.h>
#include <ndn-cpp/fields/name.h>
#include <ndn-cpp/fields/exclude.h>
#include <ndn-cpp/helpers/hash.h>

namespace ndn {

/**
 * @brief Class abstracting operations with Interests (constructing and getting access to Interest fields)
 */
class Interest
{
public:
  /**
   * @brief Default constructor, creates an interest for / prefix without any selectors
   */
  Interest ();

  /**
   * @brief Create an interest for the name
   * @param name name of the data to request
   */
  Interest (const Name &name);

  /**
   * @brief Copy constructor
   * @param interest interest to copy
   */
  Interest (const Interest &interest);

  /**
   * @brief Create an interest based on ccn_parsed_interest data structure
   * @param interest pointer to ccn_parsed_interest data structure
   *
   * This method will create an interest with empty name, since ccn_parsed_interest structure
   * has limited amount of information
   */
  Interest (const ccn_parsed_interest *interest);

  /**
   * @brief Set interest name
   * @param name name of the interest
   * @return reference to self (to allow method chaining)
   *
   * In some cases, a direct access to and manipulation of name using getName is more efficient
   */
  inline Interest &
  setName (const Name &name);

  /**
   * @brief Get interest name (const reference)
   * @returns name of the interest
   */
  inline const Name &
  getName () const;

  /**
   * @brief Get interest name (reference)
   * @returns name of the interest
   */
  inline Name &
  getName ();

  /**
   * @brief Set interest lifetime (time_duration)
   * @param interestLifetime interest lifetime specified as a time_duration value.
   *        Negative value means that InterestLifetime is not set.
   * @return reference to self (to allow method chaining)
   */
  inline Interest &
  setInterestLifetime (const TimeInterval &interestLifetime);

  /**
   * @brief Set interest lifetime (double)
   * @param interestLifetime interest lifetime expressed in seconds, with possible fractional seconds (double).
   *        Negative value means that InterestLifetime is not set.
   * @return reference to self (to allow method chaining)
   */
  inline Interest &
  setInterestLifetime (double interestLifetimeSeconds);

  /**
   * @brief Get interest lifetime
   * @return TimeInterval representing lifetime of the interest.
   *         Use time_duration::total_seconds () or time_duration::total_microseconds (),
   *         if you need interest lifetime as a plain number.
   *         @see http://www.boost.org/doc/libs/1_53_0/doc/html/date_time/posix_time.html
   */
  inline const TimeInterval &
  getInterestLifetime () const;

  /**
   * @brief Set intended interest scope
   * @param scope requested scope of the interest @see Scope
   * @return reference to self (to allow method chaining)
   */
  inline Interest &
  setScope (uint8_t scope);

  /**
   * @brief Get intended interest scope
   * @return intended interest scope @see Scope
   */
  inline uint8_t
  getScope () const;

  ///////////////////////////////////////////////////////////////////////
  //                          SELECTORS                                //
  ///////////////////////////////////////////////////////////////////////

  /**
   * @brief Enum defining constants for AnswerOriginKind selector field
   */
  enum AnswerOriginKind
  {
    AOK_CS = 0x1,      ///< @brief request item from the content store
    AOK_NEW = 0x2,     ///< @brief request item from the original producer
    AOK_DEFAULT = 0x3, ///< @brief default: either from content store or original producer
    AOK_STALE = 0x4,   ///< @brief Allow stale data
    AOK_EXPIRE = 0x10  ///< @brief Allow expired data (?)
  };

  /**
   * @brief Enum defining constants for ChildSelector field
   */
  enum ChildSelector
    {
      CHILD_LEFT = 0,   ///< @brief request left child
      CHILD_RIGHT = 1,  ///< @brief request right child
      CHILD_DEFAULT = 2 ///< @brief do not specify which child is requested
    };

  /**
   * @brief Enum defining constants for Scope field
   */
  enum Scope
    {
      NO_SCOPE = 255,        ///< @brief Interest scope is not defined
      SCOPE_LOCAL_CCND = 0,  ///< @brief Interest scope is only toward local NDN daemon
      SCOPE_LOCAL_HOST = 1,  ///< @brief Interest scope is within local host (any local application only)
      SCOPE_NEXT_HOST = 2    ///< @brief Interest scope is within local host and immediate neighboring node
    };

  /**
   * @brief Set interest selector for maximum suffix components
   * @param maxSuffixComponents maximum number of suffix components. If Interest::ncomps, then not restricted
   * @return reference to self (to allow method chaining)
   */
  inline Interest &
  setMaxSuffixComponents (uint32_t maxSuffixComponents);

  /**
   * \brief Get interest selector for maximum suffix components
   *
   * MaxSuffixComponents refer to the number of name components beyond those in the prefix,
   * and counting the implicit digest, that may occur in the matching ContentObject.
   * For more information, see http://www.ccnx.org/releases/latest/doc/technical/InterestMessage.html
   **/
  inline uint32_t
  getMaxSuffixComponents () const;

  /**
   * @brief Set interest selector for minimum suffix components
   * @param minSuffixComponents minimum number of suffix components. If Interest::ncomps, then not restricted
   * @return reference to self (to allow method chaining)
   */
  inline Interest &
  setMinSuffixComponents (uint32_t minSuffixComponents);

  /**
   * \brief Get interest selector for minimum suffix components
   *
   * MinSuffixComponents refer to the number of name components beyond those in the prefix,
   * and counting the implicit digest, that may occur in the matching ContentObject.
   * For more information, see http://www.ccnx.org/releases/latest/doc/technical/InterestMessage.html
   **/
  inline uint32_t
  getMinSuffixComponents () const;

  /**
   * @brief Set interest selector for answer origin kind
   * @param answerOriginKind type of answer @see AnswerOriginKind
   * @return reference to self (to allow method chaining)
   */
  inline Interest &
  setAnswerOriginKind (uint32_t answerOriginKind);

  /**
   * @brief Get interest selector for answer origin kind
   */
  inline uint32_t
  getAnswerOriginKind () const;

  /**
   * @brief Set interest selector for child selector
   * @param child child selector @see ChildSelector
   * @return reference to self (to allow method chaining)
   *
   * Often a given interest will match more than one ContentObject within a given content store.
   * The ChildSelector provides a way of expressing a preference for which of these should be returned.
   * If the value is false, the leftmost child is preferred. If true, the rightmost child is preferred.
   * \see http://www.ccnx.org/releases/latest/doc/technical/InterestMessage.html for more information.
   */
  inline Interest &
  setChildSelector (uint8_t child);

  /**
   * @brief Get interest selector for child selector
   */
  inline uint8_t
  getChildSelector () const;

  /**
   * @brief Set interest selector for publisher public key digest
   * @param digest publisher public key digest
   * @return reference to self (to allow method chaining)
   *
   * Currently, this method has no effect
   * @todo Implement PublisherPublicKeyDigest
   */
  inline Interest &
  setPublisherPublicKeyDigest(const Hash &digest);

  /**
   * @brief Get interest selector for publisher public key digest
   *
   * @todo Implement
   */
  inline const Hash&
  getPublisherPublicKeyDigest () const;

  /**
   * @brief Set exclude filter
   * @param exclude An exclude filter to set
   *
   * In some cases, a direct access to and manipulation of exclude filter using getExclude is more efficient
   */
  inline void
  setExclude (const Exclude &exclude);

  /**
   * @brief Get exclude filter (const reference)
   */
  inline const Exclude &
  getExclude () const;

  /**
   * @brief Get exclude filter (reference)
   */
  inline Exclude &
  getExclude ();

  ///////////////////////////////////////////////////////////////////////
  //                           HELPERS                                 //
  ///////////////////////////////////////////////////////////////////////

  /**
   * @brief Compare equality of two interests
   */
  bool
  operator== (const Interest &interest);

public:
  // Data Members (public):
  /// @brief Value indicating that number of components parameter is invalid
  const static uint32_t ncomps = static_cast<uint32_t> (-1);

private:
  Name m_name;
  uint32_t m_maxSuffixComponents;
  uint32_t m_minSuffixComponents;
  uint32_t m_answerOriginKind;
  TimeInterval m_interestLifetime; // lifetime in seconds

  uint8_t m_scope;
  uint8_t m_childSelector;
  // not used now
  Hash m_publisherPublicKeyDigest;
  Exclude m_exclude;

  Ptr<Blob> m_wire;
};

typedef boost::shared_ptr<Interest> InterestPtr;

namespace Error
{
/**
 * @brief Exception that is thrown in case of error during interest construction or parsing
 */
struct Interest:
    virtual boost::exception, virtual std::exception {};
}



inline Interest &
Interest::setName (const Name &name)
{
  m_name = name;
  return *this;
}

inline const Name &
Interest::getName () const
{
  return m_name;
}

inline Name &
Interest::getName ()
{
  return m_name;
}

inline Interest &
Interest::setInterestLifetime (const TimeInterval &interestLifetime)
{
  m_interestLifetime = interestLifetime;
  return *this;
}

inline Interest &
Interest::setInterestLifetime (double interestLifetimeSeconds)
{
  m_interestLifetime = time::Seconds (interestLifetimeSeconds);
  return *this;
}

inline const TimeInterval &
Interest::getInterestLifetime () const
{
  return m_interestLifetime;
}

inline Interest &
Interest::setScope (uint8_t scope)
{
  m_scope = scope;
  return *this;
}

inline uint8_t
Interest::getScope () const
{
  return m_scope;
}

///////////////////////////////////////////////////////////////////////
//                          SELECTORS                                //
///////////////////////////////////////////////////////////////////////


inline Interest &
Interest::setMaxSuffixComponents (uint32_t maxSuffixComponents)
{
  m_maxSuffixComponents = maxSuffixComponents;
  return *this;
}

inline uint32_t
Interest::getMaxSuffixComponents () const
{
  return m_maxSuffixComponents;
}

inline Interest &
Interest::setMinSuffixComponents (uint32_t minSuffixComponents)
{
  m_minSuffixComponents = minSuffixComponents;
  return *this;
}

inline uint32_t
Interest::getMinSuffixComponents () const
{
  return m_minSuffixComponents;
}

inline Interest &
Interest::setAnswerOriginKind (uint32_t answerOriginKind)
{
  m_answerOriginKind = answerOriginKind;
  return *this;
}

inline uint32_t
Interest::getAnswerOriginKind () const
{
  return m_answerOriginKind;
}

inline Interest &
Interest::setChildSelector (uint8_t childSelector)
{
  m_childSelector = childSelector;
  return *this;
}

inline uint8_t
Interest::getChildSelector () const
{
  return m_childSelector;
}

inline Interest &
Interest::setPublisherPublicKeyDigest(const Hash &publisherPublicKeyDigest)
{
  m_publisherPublicKeyDigest = publisherPublicKeyDigest;
  return *this;
}

inline const Hash&
Interest::getPublisherPublicKeyDigest () const
{
  return m_publisherPublicKeyDigest;
}

inline void
Interest::setExclude (const Exclude &exclude)
{
  m_exclude = exclude;
}

/**
 * @brief Get exclude filter (const reference)
 */
inline const Exclude &
Interest::getExclude () const
{
  return m_exclude;
}

/**
 * @brief Get exclude filter (reference)
 */
inline Exclude &
Interest::getExclude ()
{
  return m_exclude;
}


} // ndn

#endif // NDN_INTEREST_H
