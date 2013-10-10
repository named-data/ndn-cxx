/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "../c/util/blob.h"
#include <ndn-cpp/util/blob.hpp>

using namespace std;

namespace ndn {

Blob::Blob(const struct ndn_Blob& blobStruct)
  : ptr_lib::shared_ptr<const std::vector<uint8_t> >(new std::vector<uint8_t>(blobStruct.value, blobStruct.value + blobStruct.length))
{
}

void 
Blob::get(struct ndn_Blob& blobStruct) const 
{
  blobStruct.length = size(); 
  if (size() > 0)
    blobStruct.value = (uint8_t*)buf();
  else
    blobStruct.value = 0;
}

}
