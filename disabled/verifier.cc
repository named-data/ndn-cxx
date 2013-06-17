/* -*- Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013 University of California, Los Angeles
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Zhenkai Zhu <zhenkai@cs.ucla.edu>
 *         Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#include "verifier.h"
#include "ndn.cxx/wrapper.h"
#include "logging.h"

INIT_LOGGER ("ndn.Verifier");
namespace ndn {

static const size_t ROOT_KEY_DIGEST_LEN = 32;  // SHA-256
static const unsigned char ROOT_KEY_DIGEST[ROOT_KEY_DIGEST_LEN] = {0xa7, 0xd9, 0x8b, 0x81, 0xde, 0x13, 0xfc,
0x56, 0xc5, 0xa6, 0x92, 0xb4, 0x44, 0x93, 0x6e, 0x56, 0x70, 0x9d, 0x52, 0x6f, 0x70,
0xed, 0x39, 0xef, 0xb5, 0xe2, 0x3, 0x29, 0xa5, 0x53, 0x3e, 0x68};

Verifier::Verifier(Wrapper *ccnx)
         : m_ccnx(ccnx)
         , m_rootKeyDigest(ROOT_KEY_DIGEST, ROOT_KEY_DIGEST_LEN)
{
}

Verifier::~Verifier()
{
}

bool
Verifier::verify(PcoPtr pco, double maxWait)
{
  _LOG_TRACE("Verifying content [" << pco->name() << "]");
  HashPtr publisherPublicKeyDigest = pco->publisherPublicKeyDigest();

  {
    UniqueRecLock lock(m_cacheLock);
    CertCache::iterator it = m_certCache.find(*publisherPublicKeyDigest);
    if (it != m_certCache.end())
    {
      CertPtr cert = it->second;
      if (cert->validity() == Cert::WITHIN_VALID_TIME_SPAN)
      {
        pco->verifySignature(cert);
        return pco->verified();
      }
      else
      {
        // delete the invalid cert cache
        m_certCache.erase(it);
      }
    }
  }

  // keyName is the name specified in key locator, i.e. without version and segment
  Name keyName = pco->keyName();
  int keyNameSize = keyName.size();

  if (keyNameSize < 2)
  {
    _LOG_ERROR("Key name is empty or has too few components.");
    return false;
  }

  // for keys, we have to make sure key name is strictly prefix of the content name
  if (pco->type() == ParsedContentObject::KEY)
  {
    Name contentName = pco->name();
    // when checking for prefix, do not include the hash in the key name (which is the last component)
    Name keyNamePrefix = keyName.getPrefix (keyNameSize - 1);
    if (keyNamePrefix.size() >= contentName.size() || contentName.getPrefix (keyNamePrefix.size()) != keyNamePrefix)
    {
      _LOG_ERROR("Key name prefix [" << keyNamePrefix << "] is not the prefix of content name [" << contentName << "]");
      return false;
    }
  }
  else
  {
    // for now, user can assign any data using his key
  }

  Name metaName;
  metaName
    .append (keyName.getPrefix (keyNameSize - 1))
    .append ("info")
    .append (keyName.getSubName (keyNameSize - 1));

  Interest interest;
  interest.setChildSelector (Interest::CHILD_RIGHT)
    .setInterestLifetime(maxWait);

  PcoPtr keyObject = m_ccnx->get(Interest (interest).setName (keyName), maxWait);
  PcoPtr metaObject = m_ccnx->get(Interest (interest).setName (metaName), maxWait);
  if (!keyObject || !metaObject )
  {
    _LOG_ERROR("can not fetch key or meta");
    return false;
  }

  HashPtr publisherKeyHashInKeyObject = keyObject->publisherPublicKeyDigest();
  HashPtr publisherKeyHashInMetaObject = metaObject->publisherPublicKeyDigest();

  // make sure key and meta are signed using the same key
  if (publisherKeyHashInKeyObject->IsZero() || ! (*publisherKeyHashInKeyObject == *publisherKeyHashInMetaObject))
  {
    _LOG_ERROR("Key and Meta not signed by the same publisher");
    return false;
  }

  CertPtr cert = boost::make_shared<Cert>(keyObject, metaObject);
  if (cert->validity() != Cert::WITHIN_VALID_TIME_SPAN)
  {
    _LOG_ERROR("Certificate is not valid, validity status is : " << cert->validity());
    return false;
  }

  // check pco is actually signed by this key (i.e. we don't trust the publisherPublicKeyDigest given by ccnx c lib)
  if (! (*pco->publisherPublicKeyDigest() == cert->keyDigest()))
  {
    _LOG_ERROR("key digest does not match the publisher public key digest of the content object");
    return false;
  }

  // now we only need to make sure the key is trustworthy
  if (cert->keyDigest() == m_rootKeyDigest)
  {
    // the key is the root key
    // do nothing now
  }
  else
  {
    // can not verify key or can not verify meta
    if (!verify(keyObject, maxWait) || !verify(metaObject, maxWait))
    {
      _LOG_ERROR("Can not verify key or meta");
      return false;
    }
  }

  // ok, keyObject verified, because metaObject is signed by the same parent key and integrity checked
  // so metaObject is also verified
  {
    UniqueRecLock lock(m_cacheLock);
    m_certCache.insert(std::make_pair(cert->keyDigest(), cert));
  }

  pco->verifySignature(cert);
  if (pco->verified())
  {
    _LOG_TRACE("[" << pco->name() << "] VERIFIED.");
  }
  else
  {
    _LOG_ERROR("[" << pco->name() << "] CANNOT BE VERIFIED.");
  }
  return pco->verified();
}

} // ndn
