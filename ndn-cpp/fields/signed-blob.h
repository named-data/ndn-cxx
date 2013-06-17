/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *                     Zhenkai Zhu
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Zhenkai Zhu <zhenkai@cs.ucla.edu>
 *         Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_SIGNED_BLOB_H
#define NDN_SIGNED_BLOB_H

#include "blob.h"

namespace ndn {

/**
 * @brief Class representing a blob, which has a signed portion (e.g., bytes of DATA packet)
 */
class SignedBlob : public Blob
{
public:
  /**
   * @brief Set signed portion of the blob
   * @param offset An offset from the beginning of the blob
   * @param size Size of the signed portion of the blob
   */
  inline void
  setSignedPortion (size_t offset, size_t size);
  
  /**
   * @brief Get begin iterator to a signed portion of the blob
   */
  inline const_iterator
  signed_begin () const;

  /**
   * @brief Get end iterator of a signed portion of the blob
   */
  inline const_iterator
  signed_end () const;

  /**
   * @brief Get pointer to a first byte of the signed blob
   */
  inline const char*
  signed_buf () const;

  /**
   * @brief Get size of the signed blob
   */
  inline size_t
  signed_size () const;

private:
  const_iterator m_signedBegin;
  const_iterator m_signedEnd;
};


inline void
SignedBlob::setSignedPortion (size_t offset, size_t size)
{
  m_signedBegin = begin () + offset;
  m_signedEnd = begin () + offset + size;
}
  
inline SignedBlob::const_iterator
SignedBlob::signed_begin () const
{
  return m_signedBegin;
}

inline SignedBlob::const_iterator
SignedBlob::signed_end () const
{
  return m_signedEnd;
}

inline const char*
SignedBlob::signed_buf () const
{
  return &*m_signedBegin;
}

inline size_t
SignedBlob::signed_size () const
{
  return m_signedEnd - m_signedBegin;
}


} // ndn

#endif // NDN_SIGNED_BLOB_H
