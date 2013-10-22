/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "c/publisher-public-key-digest.h"
#include <ndn-cpp/publisher-public-key-digest.hpp>

using namespace std;

namespace ndn {

void 
PublisherPublicKeyDigest::get(struct ndn_PublisherPublicKeyDigest& publisherPublicKeyDigestStruct) const 
{
  publisherPublicKeyDigest_.get(publisherPublicKeyDigestStruct.publisherPublicKeyDigest);
}

void 
PublisherPublicKeyDigest::set(const struct ndn_PublisherPublicKeyDigest& publisherPublicKeyDigestStruct) 
{
  publisherPublicKeyDigest_ = Blob(publisherPublicKeyDigestStruct.publisherPublicKeyDigest);
}

}
