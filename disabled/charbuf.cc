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

#include "charbuf.h"

using namespace std;

namespace ndn {

void
Charbuf::init(ccn_charbuf *buf)
{
  if (buf != NULL)
  {
    m_buf = ccn_charbuf_create();
    ccn_charbuf_reserve(m_buf, buf->length);
    memcpy(m_buf->buf, buf->buf, buf->length);
    m_buf->length = buf->length;
  }
}

Charbuf::Charbuf()
            : m_buf(NULL)
{
  m_buf = ccn_charbuf_create();
}

Charbuf::Charbuf(ccn_charbuf *buf)
            : m_buf(NULL)
{
  init(buf);
}

Charbuf::Charbuf(const Charbuf &other)
            : m_buf (NULL)
{
  init(other.m_buf);
}

Charbuf::Charbuf(const void *buf, size_t length)
{
  m_buf = ccn_charbuf_create ();
  ccn_charbuf_reserve (m_buf, length);
  memcpy (m_buf->buf, buf, length);
  m_buf->length = length;
}

Charbuf::~Charbuf()
{
  ccn_charbuf_destroy (&m_buf);
}

namespace iostreams
{

charbuf_append_device::charbuf_append_device (Charbuf& cnt)
  : container (cnt)
{
}

std::streamsize
charbuf_append_device::write (const char_type* s, std::streamsize n)
{
  ccn_charbuf_append (container.getBuf (), s, n);
  return n;
}

} // iostreams

} // ndn
