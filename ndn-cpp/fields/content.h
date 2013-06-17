/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_CONTENT_H
#define NDN_CONTENT_H

#include "ndn-cpp/common.h"
#include "ndn-cpp/fields/blob.h"
#include "ndn-cpp/fields/name-component.h"

namespace ndn {

/**
 * @brief Class providing an interface to work with content NDN data packets
 *
 * Content of data packets consists of two parts: information about content
 * (timestamp, freshness, type, etc.) and raw content itself
 *
 * @code
 * Content ::= ContentInfo
 *             ContentData
 *
 * ContentInfo ::= Timestamp
 *                 Type?
 *                 Freshness?
 *                 FinalBlockID?
 * @endcode
 */
class Content
{
public:
  /**
   * @brief Enum of content types
   */
  enum Type
    {
      DATA = 0, ///< @brief No semantics is defined for the content
      ENCR,     ///< @brief Indicate that the content is encrypted
      GONE,     ///< @brief ?
      KEY,      ///< @brief Content is a key object
      LINK,     ///< @brief Content contains a LINK object
      NACK      ///< @brief Negative acknowledgment by the content producer, indicating that there is no data with the requested name
    };

  /**
   * @brief Create an empty content
   */
  Content ();

  /**
   * @brief Create a content from a memory buffer
   * @param buffer pointer to first byte of the memory buffer
   * @param size size of the memory buffer
   * @param timestamp content generation timestamp
   * @param type type of content (default is Content::DATA)
   * @param freshness amount of time the content is considered "fresh" (default is 2147 seconds, maximum possible value for CCNx)
   * @param finalBlock name of the final DATA
   *
   * Use the other version of the constructor, if timestamp needs to be automatically generated
   */
  Content (const void *buffer, size_t size,
           const Time &timestamp,
           Type type = DATA,
           const TimeInterval &freshness = maxFreshness,
           const name::Component &finalBlock = noFinalBlock);

  /**
   * @brief Create a content from a memory buffer
   * @param buffer pointer to first byte of the memory buffer
   * @param size size of the memory buffer
   * @param type type of content (default is Content::DATA)
   * @param freshness amount of time the content is considered "fresh" (default is 2147 seconds, maximum possible value for CCNx)
   * @param finalBlock name of the final DATA
   *
   * This method automatically sets timestamp of the created content to the current time (UTC clock)
   */
  Content (const void *buffer, size_t size,
           Type type = DATA,
           const TimeInterval &freshness = maxFreshness,
           const name::Component &finalBlock = noFinalBlock);

  /**
   * @brief Get content timestamp (const reference)
   */
  inline const Time &
  getTimestamp () const;

  /**
   * @brief Get content timestamp (reference)
   */
  inline Time &
  getTimestamp ();

  /**
   * @brief Set content timestamp
   * @param timestamp content timestamp (default is empty ptime object)
   *
   * If parameter is omitted, then the current time (UTC clock) is used
   */
  inline void
  setTimeStamp (const Time &timestamp = Time ());

  /**
   * @brief Get type of content
   */
  inline Type
  getType () const;

  /**
   * @brief Set type of content
   * @param type content type @see Content::Type
   */
  inline void
  setType (Type type);

  /**
   * @brief Get content freshness (const reference)
   */
  inline const TimeInterval &
  getFreshness () const;

  /**
   * @brief Get content freshness (reference)
   */
  inline TimeInterval &
  getFreshness ();

  /**
   * @brief Set content freshness
   * @param freshness content freshness (default value is Content::maxFreshness = 2147 seconds)
   */
  inline void
  setFreshness (const TimeInterval &freshness = maxFreshness);

  /**
   * @brief Get final block ID of the content (const reference)
   */
  inline const name::Component &
  getFinalBlockId () const;

  /**
   * @brief Get final block ID of the content (reference)
   */
  inline name::Component &
  getFinalBlockId ();

  /**
   * @brief Set final block ID of the content
   * @param finalBlock component name of the final block
   */
  inline void
  setFinalBlockId (const name::Component &finalBlock);

  /**
   * @brief Get const reference to content bits
   */
  inline const Blob &
  getContent () const;

  /**
   * @brief Get reference to content bits
   */
  inline Blob &
  getContent ();

  /**
   * @brief Set content bits from blob
   * @param content blob that holds content bits
   *
   * In certain cases, getContent ().swap (content); is more appropriate,
   * since it would avoid object copying
   */
  inline void
  setContent (const Blob &content);

  /**
   * @brief Set content bits from memory buffer
   * @param buf pointer to first byte of memory buffer
   * @param length size of memory buffer
   */
  inline void
  setContent (const void *buf, size_t length);

public:
  static const name::Component noFinalBlock; ///< @brief Not a final block == name::Component ()
  static const TimeInterval noFreshness; ///< @brief Minimum freshness == seconds (0)
  static const TimeInterval maxFreshness; ///< @brief Maximum freshnes == seconds (2147)

private:
  // ContentInfo
  Time m_timestamp;
  Type m_type;
  TimeInterval m_freshness;
  name::Component m_finalBlockId;

  // ContentData
  Blob m_content;
};

inline const Time &
Content::getTimestamp () const
{
  return m_timestamp;
}

inline Time &
Content::getTimestamp ()
{
  return m_timestamp;
}

inline void
Content::setTimeStamp (const Time &timestamp/* = Time ()*/)
{
  if (timestamp != Time ())
    {
      m_timestamp = timestamp;
    }
  else
    {
      m_timestamp = time::Now ();
    }
}

inline Content::Type
Content::getType () const
{
  return m_type;
}

inline void
Content::setType (Content::Type type)
{
  m_type = type;
}

inline const TimeInterval &
Content::getFreshness () const
{
  return m_freshness;
}

inline TimeInterval &
Content::getFreshness ()
{
  return m_freshness;
}

inline void
Content::setFreshness (const TimeInterval &freshness/* = Content::maxFreshness*/)
{
  m_freshness = freshness;
}

inline const name::Component &
Content::getFinalBlockId () const
{
  return m_finalBlockId;
}

inline name::Component &
Content::getFinalBlockId ()
{
  return m_finalBlockId;
}

inline void
Content::setFinalBlockId (const name::Component &finalBlock)
{
  m_finalBlockId = finalBlock;
}

inline const Blob &
Content::getContent () const
{
  return m_content;
}

inline Blob &
Content::getContent ()
{
  return m_content;
}

inline void
Content::setContent (const Blob &content)
{
  m_content = content;
}

inline void
Content::setContent (const void *buf, size_t length)
{
  Blob (buf, length).swap (m_content);
}

} // ndn

#endif // NDN_SIGNATURE_H
