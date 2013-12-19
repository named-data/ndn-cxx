/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include "c/name.h"
#include "c/interest.h"
#include "c/util/crypto.h"
#include "c/util/time.h"
#include "c/encoding/binary-xml.h"
#include "encoding/binary-xml-decoder.hpp"
#include <ndn-cpp/forwarding-entry.hpp>
#include <ndn-cpp/security/key-chain.hpp>
#include <ndn-cpp/sha256-with-rsa-signature.hpp>
#include <ndn-cpp/node.hpp>

using namespace std;

namespace ndn {

static uint8_t SELFREG_PUBLIC_KEY_DER[] = {
0x30, 0x81, 0x9F, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x81,
0x8D, 0x00, 0x30, 0x81, 0x89, 0x02, 0x81, 0x81, 0x00, 0xE1, 0x7D, 0x30, 0xA7, 0xD8, 0x28, 0xAB, 0x1B, 0x84, 0x0B, 0x17,
0x54, 0x2D, 0xCA, 0xF6, 0x20, 0x7A, 0xFD, 0x22, 0x1E, 0x08, 0x6B, 0x2A, 0x60, 0xD1, 0x6C, 0xB7, 0xF5, 0x44, 0x48, 0xBA,
0x9F, 0x3F, 0x08, 0xBC, 0xD0, 0x99, 0xDB, 0x21, 0xDD, 0x16, 0x2A, 0x77, 0x9E, 0x61, 0xAA, 0x89, 0xEE, 0xE5, 0x54, 0xD3,
0xA4, 0x7D, 0xE2, 0x30, 0xBC, 0x7A, 0xC5, 0x90, 0xD5, 0x24, 0x06, 0x7C, 0x38, 0x98, 0xBB, 0xA6, 0xF5, 0xDC, 0x43, 0x60,
0xB8, 0x45, 0xED, 0xA4, 0x8C, 0xBD, 0x9C, 0xF1, 0x26, 0xA7, 0x23, 0x44, 0x5F, 0x0E, 0x19, 0x52, 0xD7, 0x32, 0x5A, 0x75,
0xFA, 0xF5, 0x56, 0x14, 0x4F, 0x9A, 0x98, 0xAF, 0x71, 0x86, 0xB0, 0x27, 0x86, 0x85, 0xB8, 0xE2, 0xC0, 0x8B, 0xEA, 0x87,
0x17, 0x1B, 0x4D, 0xEE, 0x58, 0x5C, 0x18, 0x28, 0x29, 0x5B, 0x53, 0x95, 0xEB, 0x4A, 0x17, 0x77, 0x9F, 0x02, 0x03, 0x01,
0x00, 01
};

static uint8_t SELFREG_PRIVATE_KEY_DER[] = {
0x30, 0x82, 0x02, 0x5d, 0x02, 0x01, 0x00, 0x02, 0x81, 0x81, 0x00, 0xe1, 0x7d, 0x30, 0xa7, 0xd8, 0x28, 0xab, 0x1b, 0x84,
0x0b, 0x17, 0x54, 0x2d, 0xca, 0xf6, 0x20, 0x7a, 0xfd, 0x22, 0x1e, 0x08, 0x6b, 0x2a, 0x60, 0xd1, 0x6c, 0xb7, 0xf5, 0x44,
0x48, 0xba, 0x9f, 0x3f, 0x08, 0xbc, 0xd0, 0x99, 0xdb, 0x21, 0xdd, 0x16, 0x2a, 0x77, 0x9e, 0x61, 0xaa, 0x89, 0xee, 0xe5,
0x54, 0xd3, 0xa4, 0x7d, 0xe2, 0x30, 0xbc, 0x7a, 0xc5, 0x90, 0xd5, 0x24, 0x06, 0x7c, 0x38, 0x98, 0xbb, 0xa6, 0xf5, 0xdc,
0x43, 0x60, 0xb8, 0x45, 0xed, 0xa4, 0x8c, 0xbd, 0x9c, 0xf1, 0x26, 0xa7, 0x23, 0x44, 0x5f, 0x0e, 0x19, 0x52, 0xd7, 0x32,
0x5a, 0x75, 0xfa, 0xf5, 0x56, 0x14, 0x4f, 0x9a, 0x98, 0xaf, 0x71, 0x86, 0xb0, 0x27, 0x86, 0x85, 0xb8, 0xe2, 0xc0, 0x8b,
0xea, 0x87, 0x17, 0x1b, 0x4d, 0xee, 0x58, 0x5c, 0x18, 0x28, 0x29, 0x5b, 0x53, 0x95, 0xeb, 0x4a, 0x17, 0x77, 0x9f, 0x02,
0x03, 0x01, 0x00, 0x01, 0x02, 0x81, 0x80, 0x1a, 0x4b, 0xfa, 0x4f, 0xa8, 0xc2, 0xdd, 0x69, 0xa1, 0x15, 0x96, 0x0b, 0xe8,
0x27, 0x42, 0x5a, 0xf9, 0x5c, 0xea, 0x0c, 0xac, 0x98, 0xaa, 0xe1, 0x8d, 0xaa, 0xeb, 0x2d, 0x3c, 0x60, 0x6a, 0xfb, 0x45,
0x63, 0xa4, 0x79, 0x83, 0x67, 0xed, 0xe4, 0x15, 0xc0, 0xb0, 0x20, 0x95, 0x6d, 0x49, 0x16, 0xc6, 0x42, 0x05, 0x48, 0xaa,
0xb1, 0xa5, 0x53, 0x65, 0xd2, 0x02, 0x99, 0x08, 0xd1, 0x84, 0xcc, 0xf0, 0xcd, 0xea, 0x61, 0xc9, 0x39, 0x02, 0x3f, 0x87,
0x4a, 0xe5, 0xc4, 0xd2, 0x07, 0x02, 0xe1, 0x9f, 0xa0, 0x06, 0xc2, 0xcc, 0x02, 0xe7, 0xaa, 0x6c, 0x99, 0x8a, 0xf8, 0x49,
0x00, 0xf1, 0xa2, 0x8c, 0x0c, 0x8a, 0xb9, 0x4f, 0x6d, 0x73, 0x3b, 0x2c, 0xb7, 0x9f, 0x8a, 0xa6, 0x7f, 0x9b, 0x9f, 0xb7,
0xa1, 0xcc, 0x74, 0x2e, 0x8f, 0xb8, 0xb0, 0x26, 0x89, 0xd2, 0xe5, 0x66, 0xe8, 0x8e, 0xa1, 0x02, 0x41, 0x00, 0xfc, 0xe7,
0x52, 0xbc, 0x4e, 0x95, 0xb6, 0x1a, 0xb4, 0x62, 0xcc, 0xd8, 0x06, 0xe1, 0xdc, 0x7a, 0xa2, 0xb6, 0x71, 0x01, 0xaa, 0x27,
0xfc, 0x99, 0xe5, 0xf2, 0x54, 0xbb, 0xb2, 0x85, 0xe1, 0x96, 0x54, 0x2d, 0xcb, 0xba, 0x86, 0xfa, 0x80, 0xdf, 0xcf, 0x39,
0xe6, 0x74, 0xcb, 0x22, 0xce, 0x70, 0xaa, 0x10, 0x00, 0x73, 0x1d, 0x45, 0x0a, 0x39, 0x51, 0x84, 0xf5, 0x15, 0x8f, 0x37,
0x76, 0x91, 0x02, 0x41, 0x00, 0xe4, 0x3f, 0xf0, 0xf4, 0xde, 0x79, 0x77, 0x48, 0x9b, 0x9c, 0x28, 0x45, 0x26, 0x57, 0x3c,
0x71, 0x40, 0x28, 0x6a, 0xa1, 0xfe, 0xc3, 0xe5, 0x37, 0xa1, 0x03, 0xf6, 0x2d, 0xbe, 0x80, 0x64, 0x72, 0x69, 0x2e, 0x9b,
0x4d, 0xe3, 0x2e, 0x1b, 0xfe, 0xe7, 0xf9, 0x77, 0x8c, 0x18, 0x53, 0x9f, 0xe2, 0xfe, 0x00, 0xbb, 0x49, 0x20, 0x47, 0xdf,
0x01, 0x61, 0x87, 0xd6, 0xe3, 0x44, 0xb5, 0x03, 0x2f, 0x02, 0x40, 0x54, 0xec, 0x7c, 0xbc, 0xdd, 0x0a, 0xaa, 0xde, 0xe6,
0xc9, 0xf2, 0x8d, 0x6c, 0x2a, 0x35, 0xf6, 0x3c, 0x63, 0x55, 0x29, 0x40, 0xf1, 0x32, 0x82, 0x9f, 0x53, 0xb3, 0x9e, 0x5f,
0xc1, 0x53, 0x52, 0x3e, 0xac, 0x2e, 0x28, 0x51, 0xa1, 0x16, 0xdb, 0x90, 0xe3, 0x99, 0x7e, 0x88, 0xa4, 0x04, 0x7c, 0x92,
0xae, 0xd2, 0xe7, 0xd4, 0xe1, 0x55, 0x20, 0x90, 0x3e, 0x3c, 0x6a, 0x63, 0xf0, 0x34, 0xf1, 0x02, 0x41, 0x00, 0x84, 0x5a,
0x17, 0x6c, 0xc6, 0x3c, 0x84, 0xd0, 0x93, 0x7a, 0xff, 0x56, 0xe9, 0x9e, 0x98, 0x2b, 0xcb, 0x5a, 0x24, 0x4a, 0xff, 0x21,
0xb4, 0x9e, 0x87, 0x3d, 0x76, 0xd8, 0x9b, 0xa8, 0x73, 0x96, 0x6c, 0x2b, 0x5c, 0x5e, 0xd3, 0xa6, 0xff, 0x10, 0xd6, 0x8e,
0xaf, 0xa5, 0x8a, 0xcd, 0xa2, 0xde, 0xcb, 0x0e, 0xbd, 0x8a, 0xef, 0xae, 0xfd, 0x3f, 0x1d, 0xc0, 0xd8, 0xf8, 0x3b, 0xf5,
0x02, 0x7d, 0x02, 0x41, 0x00, 0x8b, 0x26, 0xd3, 0x2c, 0x7d, 0x28, 0x38, 0x92, 0xf1, 0xbf, 0x15, 0x16, 0x39, 0x50, 0xc8,
0x6d, 0x32, 0xec, 0x28, 0xf2, 0x8b, 0xd8, 0x70, 0xc5, 0xed, 0xe1, 0x7b, 0xff, 0x2d, 0x66, 0x8c, 0x86, 0x77, 0x43, 0xeb,
0xb6, 0xf6, 0x50, 0x66, 0xb0, 0x40, 0x24, 0x6a, 0xaf, 0x98, 0x21, 0x45, 0x30, 0x01, 0x59, 0xd0, 0xc3, 0xfc, 0x7b, 0xae,
0x30, 0x18, 0xeb, 0x90, 0xfb, 0x17, 0xd3, 0xce, 0xb5
};

uint64_t Node::PendingInterest::lastPendingInterestId_ = 0;
uint64_t Node::RegisteredPrefix::lastRegisteredPrefixId_ = 0;

/**
 * Set the KeyLocator using the full SELFREG_PUBLIC_KEY_DER, sign the data packet using SELFREG_PRIVATE_KEY_DER 
 * and set the signature.
 * This is a temporary function, because we expect in the future that registerPrefix will not require a signature on the packet.
 * @param data The Data packet to sign.
 * @param wireFormat The WireFormat for encoding the Data packet.
 */
static void
selfregSign(Data& data, WireFormat& wireFormat)
{
  data.setSignature(Sha256WithRsaSignature());
  Sha256WithRsaSignature *signature = dynamic_cast<Sha256WithRsaSignature*>(data.getSignature());
  
  // Set the public key.
  uint8_t publicKeyDigest[SHA256_DIGEST_LENGTH];
  ndn_digestSha256(SELFREG_PUBLIC_KEY_DER, sizeof(SELFREG_PUBLIC_KEY_DER), publicKeyDigest);
  signature->getPublisherPublicKeyDigest().setPublisherPublicKeyDigest(Blob(publicKeyDigest, sizeof(publicKeyDigest)));
  signature->getKeyLocator().setType(ndn_KeyLocatorType_KEY);
  signature->getKeyLocator().setKeyData(Blob(SELFREG_PUBLIC_KEY_DER, sizeof(SELFREG_PUBLIC_KEY_DER)));

  // Sign the fields.
  SignedBlob encoding = data.wireEncode(wireFormat);
  uint8_t signedPortionDigest[SHA256_DIGEST_LENGTH];
  ndn_digestSha256(encoding.signedBuf(), encoding.signedSize(), signedPortionDigest);
  uint8_t signatureBits[1000];
  unsigned int signatureBitsLength;
  // Use a temporary pointer since d2i updates it.
  const uint8_t *derPointer = SELFREG_PRIVATE_KEY_DER;
  RSA *privateKey = d2i_RSAPrivateKey(NULL, &derPointer, sizeof(SELFREG_PRIVATE_KEY_DER));
  if (!privateKey)
    throw runtime_error("Error decoding private key in d2i_RSAPrivateKey");
  int success = RSA_sign(NID_sha256, signedPortionDigest, sizeof(signedPortionDigest), signatureBits, &signatureBitsLength, privateKey);
  // Free the private key before checking for success.
  RSA_free(privateKey);
  if (!success)
    throw runtime_error("Error in RSA_sign");
  
  signature->setSignature(Blob(signatureBits, (size_t)signatureBitsLength));
}

Node::Node(const ptr_lib::shared_ptr<Transport>& transport)
: transport_(transport),
  ndndIdFetcherInterest_(Name("/%C1.M.S.localhost/%C1.M.SRV/ndnd/KEY"), 4000.0)
{
}

uint64_t 
Node::expressInterest(const Interest& interest, const OnData& onData, const OnTimeout& onTimeout, WireFormat& wireFormat)
{
  // TODO: Properly check if we are already connected to the expected host.
  if (!transport_->getIsConnected())
    transport_->connect(*this);
  
  uint64_t pendingInterestId = PendingInterest::getNextPendingInterestId();
  pendingInterestTable_.push_back(ptr_lib::shared_ptr<PendingInterest>(new PendingInterest
    (pendingInterestId, ptr_lib::shared_ptr<const Interest>(new Interest(interest)), onData, onTimeout)));
  
  Blob encoding = interest.wireEncode(wireFormat);  
  transport_->send(*encoding);
  
  return pendingInterestId;
}

void
Node::removePendingInterest(uint64_t pendingInterestId)
{
  // Go backwards through the list so we can erase entries.
  // Remove all entries even though pendingInterestId should be unique.
  for (int i = (int)pendingInterestTable_.size() - 1; i >= 0; --i) {
    if (pendingInterestTable_[i]->getPendingInterestId() == pendingInterestId)
      pendingInterestTable_.erase(pendingInterestTable_.begin() + i);
  }
}

uint64_t 
Node::registerPrefix
  (const Name& prefix, const OnInterest& onInterest, const OnRegisterFailed& onRegisterFailed, const ForwardingFlags& flags, WireFormat& wireFormat)
{
  // Get the registeredPrefixId now so we can return it to the caller.
  uint64_t registeredPrefixId = RegisteredPrefix::getNextRegisteredPrefixId();

  if (ndndId_.size() == 0) {
    // First fetch the ndndId of the connected hub.
    NdndIdFetcher fetcher
      (ptr_lib::shared_ptr<NdndIdFetcher::Info>(new NdndIdFetcher::Info
        (this, registeredPrefixId, prefix, onInterest, onRegisterFailed, flags, wireFormat)));
    // It is OK for func_lib::function make a copy of the function object because the Info is in a ptr_lib::shared_ptr.
    expressInterest(ndndIdFetcherInterest_, fetcher, fetcher, wireFormat);
  }
  else
    registerPrefixHelper(registeredPrefixId, ptr_lib::make_shared<const Name>(prefix), onInterest, onRegisterFailed, flags, wireFormat);
  
  return registeredPrefixId;
}

void
Node::removeRegisteredPrefix(uint64_t registeredPrefixId)
{
  // Go backwards through the list so we can erase entries.
  // Remove all entries even though pendingInterestId should be unique.
  for (int i = (int)registeredPrefixTable_.size() - 1; i >= 0; --i) {
    if (registeredPrefixTable_[i]->getRegisteredPrefixId() == registeredPrefixId)
      registeredPrefixTable_.erase(registeredPrefixTable_.begin() + i);
  }
}

void 
Node::NdndIdFetcher::operator()(const ptr_lib::shared_ptr<const Interest>& interest, const ptr_lib::shared_ptr<Data>& ndndIdData)
{
  const Sha256WithRsaSignature *signature = dynamic_cast<const Sha256WithRsaSignature*>(ndndIdData->getSignature());
  if (signature && signature->getPublisherPublicKeyDigest().getPublisherPublicKeyDigest().size() > 0) {
    // Set the ndndId_ and continue.
    // TODO: If there are multiple connected hubs, the NDN ID is really stored per connected hub.
    info_->node_.ndndId_ = signature->getPublisherPublicKeyDigest().getPublisherPublicKeyDigest();
    info_->node_.registerPrefixHelper
      (info_->registeredPrefixId_, info_->prefix_, info_->onInterest_, info_->onRegisterFailed_, info_->flags_, info_->wireFormat_);
  }
  else
    info_->onRegisterFailed_(info_->prefix_);
}

void 
Node::NdndIdFetcher::operator()(const ptr_lib::shared_ptr<const Interest>& timedOutInterest)
{
  info_->onRegisterFailed_(info_->prefix_);
}

void 
Node::registerPrefixHelper
  (uint64_t registeredPrefixId, const ptr_lib::shared_ptr<const Name>& prefix, const OnInterest& onInterest, const OnRegisterFailed& onRegisterFailed, 
   const ForwardingFlags& flags, WireFormat& wireFormat)
{
  // Create a ForwardingEntry.

  // AlexA: ndnd ignores any freshness that is larger than 3600 sec and sets 300 sec instead
  //        to register "forever" (=2000000000 sec), freshnessPeriod must be omitted
  ForwardingEntry forwardingEntry("selfreg", *prefix, PublisherPublicKeyDigest(), -1, flags, -1);
  Blob content = forwardingEntry.wireEncode();

  // Set the ForwardingEntry as the content of a Data packet and sign.
  Data data;
  data.setContent(content);
  data.getMetaInfo().setTimestampMilliseconds(time(NULL) * 1000.0);
  // For now, self sign with an arbirary key.  In the future, we may not require a signature to register.
  selfregSign(data, wireFormat);
  Blob encodedData = data.wireEncode();
  
  // Create an interest where the name has the encoded Data packet.
  Name interestName;
  const uint8_t component0[] = "ndnx";
  const uint8_t component2[] = "selfreg";
  interestName.append(component0, sizeof(component0) - 1);
  interestName.append(ndndId_);
  interestName.append(component2, sizeof(component2) - 1);
  interestName.append(encodedData);
  
  Interest interest(interestName);
  interest.setScope(1);
  Blob encodedInterest = interest.wireEncode();
  
  // Save the onInterest callback and send the registration interest.
  registeredPrefixTable_.push_back(ptr_lib::shared_ptr<RegisteredPrefix>(new RegisteredPrefix(registeredPrefixId, prefix, onInterest)));
  
  transport_->send(*encodedInterest);
}

void 
Node::processEvents()
{
  transport_->processEvents();
  
  // Check for PIT entry timeouts.  Go backwards through the list so we can erase entries.
  MillisecondsSince1970 nowMilliseconds = ndn_getNowMilliseconds();
  for (int i = (int)pendingInterestTable_.size() - 1; i >= 0; --i) {
    if (pendingInterestTable_[i]->isTimedOut(nowMilliseconds)) {
      // Save the PendingInterest and remove it from the PIT.  Then call the callback.
      ptr_lib::shared_ptr<PendingInterest> pendingInterest = pendingInterestTable_[i];
      pendingInterestTable_.erase(pendingInterestTable_.begin() + i);
      pendingInterest->callTimeout();
      
      // Refresh now since the timeout callback might have delayed.
      nowMilliseconds = ndn_getNowMilliseconds();
    }
  }
}

void 
Node::onReceivedElement(const uint8_t *element, size_t elementLength)
{
  BinaryXmlDecoder decoder(element, elementLength);
  
  if (decoder.peekDTag(ndn_BinaryXml_DTag_Interest)) {
    ptr_lib::shared_ptr<Interest> interest(new Interest());
    interest->wireDecode(element, elementLength);
    
    RegisteredPrefix *entry = getEntryForRegisteredPrefix(interest->getName());
    if (entry)
      entry->getOnInterest()(entry->getPrefix(), interest, *transport_, entry->getRegisteredPrefixId());
  }
  else if (decoder.peekDTag(ndn_BinaryXml_DTag_ContentObject)) {
    ptr_lib::shared_ptr<Data> data(new Data());
    data->wireDecode(element, elementLength);
    
    int iPitEntry = getEntryIndexForExpressedInterest(data->getName());
    if (iPitEntry >= 0) {
      // Copy pointers to the needed objects and remove the PIT entry before the calling the callback.
      const OnData onData = pendingInterestTable_[iPitEntry]->getOnData();
      const ptr_lib::shared_ptr<const Interest> interest = pendingInterestTable_[iPitEntry]->getInterest();
      pendingInterestTable_.erase(pendingInterestTable_.begin() + iPitEntry);
      onData(interest, data);
    }
  }
}

void 
Node::shutdown()
{
  transport_->close();
}

int 
Node::getEntryIndexForExpressedInterest(const Name& name)
{
  // TODO: Doesn't this belong in the Name class?
  vector<struct ndn_NameComponent> nameComponents;
  nameComponents.reserve(name.size());
  struct ndn_Name nameStruct;
  ndn_Name_initialize(&nameStruct, &nameComponents[0], nameComponents.capacity());
  name.get(nameStruct);
  
  int iResult = -1;
    
  for (size_t i = 0; i < pendingInterestTable_.size(); ++i) {
    if (ndn_Interest_matchesName((struct ndn_Interest *)&pendingInterestTable_[i]->getInterestStruct(), &nameStruct)) {
      if (iResult < 0 || 
          pendingInterestTable_[i]->getInterestStruct().name.nComponents > 
          pendingInterestTable_[iResult]->getInterestStruct().name.nComponents)
        // Update to the longer match.
        iResult = i;
    }
  }
    
  return iResult;
}
  
Node::RegisteredPrefix*
Node::getEntryForRegisteredPrefix(const Name& name)
{
  int iResult = -1;
    
  for (size_t i = 0; i < registeredPrefixTable_.size(); ++i) {
    if (registeredPrefixTable_[i]->getPrefix()->match(name)) {
      if (iResult < 0 || 
          registeredPrefixTable_[i]->getPrefix()->size() > registeredPrefixTable_[iResult]->getPrefix()->size())
        // Update to the longer match.
        iResult = i;
    }
  }
    
  if (iResult >= 0)
    return registeredPrefixTable_[iResult].get();
  else
    return 0;
}

Node::PendingInterest::PendingInterest
  (uint64_t pendingInterestId, const ptr_lib::shared_ptr<const Interest>& interest, const OnData& onData, const OnTimeout& onTimeout)
: pendingInterestId_(pendingInterestId), interest_(interest), onData_(onData), onTimeout_(onTimeout),
  interestStruct_(new struct ndn_Interest)
{
  // Set up timeoutTime_.
  if (interest_->getInterestLifetimeMilliseconds() >= 0)
    timeoutTimeMilliseconds_ = ndn_getNowMilliseconds() + interest_->getInterestLifetimeMilliseconds();
  else
    // No timeout.
    timeoutTimeMilliseconds_ = -1;
  
  // Set up interestStruct_.
  // TODO: Doesn't this belong in the Interest class?
  nameComponents_.reserve(interest_->getName().size());
  excludeEntries_.reserve(interest_->getExclude().size());
  ndn_Interest_initialize
    (interestStruct_.get(), &nameComponents_[0], nameComponents_.capacity(), &excludeEntries_[0], excludeEntries_.capacity());
  interest_->get(*interestStruct_);  
}

void 
Node::PendingInterest::callTimeout()
{
  if (onTimeout_) {
    // Ignore all exceptions.
    try {
      onTimeout_(interest_);
    }
    catch (...) { }
  }
}

}
