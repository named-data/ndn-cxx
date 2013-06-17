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

#ifndef NDN_CONTENT_OBJECT_H
#define NDN_CONTENT_OBJECT_H

#include "ndn.cxx/wrapper.h"
#include "ndn.cxx/common.h"
#include "ndn.cxx/fields/name.h"
#include "ndn.cxx/helpers/hash.h"

namespace ndn {

// class Cert;
// typedef boost::shared_ptr<Cert> CertPtr;

class ParsedContentObject
{
public:
  enum Type
  {
    DATA,
    KEY,
    OTHER
  };
  ParsedContentObject(const unsigned char *data, size_t len, bool verified = false);
  ParsedContentObject(const unsigned char *data, const ccn_parsed_ContentObject &pco, bool verified = false);
  ParsedContentObject(const Bytes &bytes, bool verified = false);
  ParsedContentObject(const ParsedContentObject &other, bool verified = false);
  virtual ~ParsedContentObject();

  Bytes
  content() const;

  BytesPtr
  contentPtr() const;

  Name
  name() const;

  Name
  keyName() const;

  HashPtr
  publisherPublicKeyDigest() const;

  Type
  type() const;

  inline const Bytes &
  buf () const;

  bool
  verified() const { return m_verified; }

  // void
  // verifySignature(const CertPtr &cert);

  const unsigned char *
  msg() const { return head(m_bytes); }

  const ccn_parsed_ContentObject *
  pco() const { return &m_pco; }

private:
  void
  init(const unsigned char *data, size_t len);

protected:
  ccn_parsed_ContentObject m_pco;
  ccn_indexbuf *m_comps;
  Bytes m_bytes;
  bool m_verified;
  bool m_integrityChecked;
};

typedef boost::shared_ptr<ParsedContentObject> PcoPtr;

namespace Error {
struct MisformedContentObject : virtual boost::exception, virtual std::exception { };
}

const Bytes &
ParsedContentObject::buf () const
{
  return m_bytes;
}


}

#endif // NDN_CONTENT_OBJECT_H
