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

#ifndef NDN_BLOB_H
#define NDN_BLOB_H

#include <vector>
#include <cstddef>

namespace ndn {

/**
 * @brief Class representing a general-use binary blob
 */
class Blob : public std::vector<char>
{
public:
  /**
   * @brief Creates an empty blob
   */
  Blob ()
  {
  }

  Blob (const void *buf, size_t length)
    : std::vector<char> (reinterpret_cast<const char*> (buf), reinterpret_cast<const char*> (buf) + length)
  {
  }
  
  /**
   * @brief Get pointer to the first byte of the binary blob
   */
  inline char*
  buf ()
  {
    return &front ();
  }

  /**
   * @brief Get const pointer to the first byte of the binary blob
   */
  inline const char*
  buf () const
  {
    return &front ();
  }
};

} // ndn

#endif // NDN_BLOB_H
