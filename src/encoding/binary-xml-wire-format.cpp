/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include <ndn-cpp/interest.hpp>
#include <ndn-cpp/data.hpp>
#include <ndn-cpp/forwarding-entry.hpp>
#include <ndn-cpp/encoding/binary-xml-wire-format.hpp>
#include "../c/encoding/binary-xml-interest.h"
#include "../c/encoding/binary-xml-data.h"
#include "../c/encoding/binary-xml-forwarding-entry.h"
#include "binary-xml-encoder.hpp"
#include "binary-xml-decoder.hpp"

using namespace std;

namespace ndn {

namespace c {

class Exclude : public ndn::Exclude
{
public:
  void
  get(struct ndn_Exclude& excludeStruct) const
  {
    if (excludeStruct.maxEntries < size())
      throw runtime_error("excludeStruct.maxEntries must be >= this exclude getEntryCount()");

    int entries = 0;
    for (Exclude::const_reverse_iterator i = rbegin (); i != rend (); i++)
      {
        if (!i->first.empty())
          {
            excludeStruct.entries[entries].type = ndn_Exclude_COMPONENT;
            excludeStruct.entries[entries].component.value.value = const_cast<uint8_t*>(i->first.getValue().buf());
            excludeStruct.entries[entries].component.value.length = i->first.getValue().size();
            ++entries;
          }
        if (i->second)
          {
            excludeStruct.entries[entries].type = ndn_Exclude_ANY;
            ++entries;
          }
      }

    excludeStruct.nEntries = entries;
  }

  void
  set(const struct ndn_Exclude& excludeStruct)
  {
    clear();

    if (excludeStruct.nEntries == 0)
      return;

    int i = 0;
    if (excludeStruct.entries[i].type == ndn_Exclude_ANY) {
      appendExclude("/", true);
      i++;
    }

    while (i < excludeStruct.nEntries) {
      ndn_ExcludeEntry *entry = &excludeStruct.entries[i];

      if (entry->type != ndn_Exclude_COMPONENT)
        throw runtime_error("unrecognized ndn_ExcludeType");

      Name::Component excludedComponent (entry->component.value.value, entry->component.value.length);
      ++i;
      entry = &excludeStruct.entries[i];

      if (i < excludeStruct.nEntries) {
        if (entry->type == ndn_Exclude_ANY) {
          appendExclude(excludedComponent, true);
          ++i;
        }
        else
          appendExclude(excludedComponent, false);
      }
      else
        appendExclude(excludedComponent, false);
    }
  }
};

class Name : public ndn::Name
{
public:
  void 
  get(struct ndn_Name& nameStruct) const
  {
    if (nameStruct.maxComponents < size())
      throw runtime_error("nameStruct.maxComponents must be >= this name getNComponents()");
  
    nameStruct.nComponents = size();
    for (size_t i = 0; i < nameStruct.nComponents; ++i) {
      nameStruct.components[i].value.value = const_cast<uint8_t*>(ndn::Name::get(i).getValue().buf());
      nameStruct.components[i].value.length = ndn::Name::get(i).getValue().size();
    }
  }
  
  void 
  set(const struct ndn_Name& nameStruct) 
  {
    clear();
    for (size_t i = 0; i < nameStruct.nComponents; ++i)
      append(nameStruct.components[i].value.value, nameStruct.components[i].value.length);  
  }
};

class Interest : public ndn::Interest
{
public:
  void 
  get(struct ndn_Interest& interestStruct) const 
  {
    reinterpret_cast<const c::Name&>(getName()).get(interestStruct.name);
    interestStruct.minSuffixComponents = getMinSuffixComponents();
    interestStruct.maxSuffixComponents = getMaxSuffixComponents();
    // publisherPublicKeyDigest_.get(interestStruct.publisherPublicKeyDigest);
    reinterpret_cast<const c::Exclude&>(getExclude()).get(interestStruct.exclude);
    interestStruct.childSelector = getChildSelector();
    interestStruct.answerOriginKind = getMustBeFresh() ?
      (ndn_Interest_ANSWER_CONTENT_STORE | ndn_Interest_ANSWER_GENERATED)
      :
      (ndn_Interest_ANSWER_CONTENT_STORE | ndn_Interest_ANSWER_GENERATED | ndn_Interest_ANSWER_STALE);
    interestStruct.scope = getScope();
    interestStruct.interestLifetimeMilliseconds = getInterestLifetime();

    interestStruct.nonce.length = 4;
    interestStruct.nonce.value = const_cast<uint8_t*>(reinterpret_cast<const uint8_t *>(getNonce()));
  }  

  void 
  set(const struct ndn_Interest& interestStruct) 
  {
    reinterpret_cast<c::Name&>(getName()).set(interestStruct.name);
    setMinSuffixComponents(interestStruct.minSuffixComponents);
    setMaxSuffixComponents(interestStruct.maxSuffixComponents);
  
    // publisherPublicKeyDigest_.set(interestStruct.publisherPublicKeyDigest);
  
    reinterpret_cast<c::Exclude&>(getExclude()).set(interestStruct.exclude);
    setChildSelector(interestStruct.childSelector);
    // answerOriginKind_ = interestStruct.answerOriginKind;
    setScope(interestStruct.scope);
    setInterestLifetime(interestStruct.interestLifetimeMilliseconds);

    setNonce(*reinterpret_cast<uint32_t*>(interestStruct.nonce.value));
  }
};

class Data : public ndn::Data
{
public:
  void 
  get(struct ndn_Data& dataStruct) const 
  {
    // signature_->get(dataStruct.signature);
    // name_.get(dataStruct.name);
    // metaInfo_.get(dataStruct.metaInfo);
    // content_.get(dataStruct.content);
  }

  void 
  set(const struct ndn_Data& dataStruct)
  {
    // signature_->set(dataStruct.signature);
    // name_.set(dataStruct.name);
    // metaInfo_.set(dataStruct.metaInfo);
    // content_ = Blob(dataStruct.content);
  }
};

class ForwardingEntry : public ndn::ForwardingEntry
{
public:
  void 
  get(struct ndn_ForwardingEntry& forwardingEntryStruct) const 
  {
    reinterpret_cast<const c::Name&>(getPrefix()).get(forwardingEntryStruct.prefix);
    // publisherPublicKeyDigest_.get(forwardingEntryStruct.publisherPublicKeyDigest);
    forwardingEntryStruct.faceId = getFaceId();
    // forwardingEntryStruct.forwardingFlags = getForwardingFlags();
    forwardingEntryStruct.freshnessSeconds = getFreshnessPeriod() / 1000;

    forwardingEntryStruct.action.length = getAction().size();
    if (getAction().size() > 0)
      forwardingEntryStruct.action.value = (uint8_t *)&getAction();
    else
      forwardingEntryStruct.action.value = 0;
  }

  void 
  set(const struct ndn_ForwardingEntry& forwardingEntryStruct) 
  {
    if (forwardingEntryStruct.action.value && forwardingEntryStruct.action.length > 0)
      setAction(string(forwardingEntryStruct.action.value, forwardingEntryStruct.action.value + forwardingEntryStruct.action.length));
    else
      setAction("");

    Name prefix;
    reinterpret_cast<c::Name&>(prefix).set(forwardingEntryStruct.prefix);
    setPrefix(prefix);
    // publisherPublicKeyDigest_.set(forwardingEntryStruct.publisherPublicKeyDigest);  
    setFaceId(forwardingEntryStruct.faceId);
    // setForwardingFlags(forwardingEntryStruct.forwardingFlags);
    setFreshnessPeriod(forwardingEntryStruct.freshnessSeconds * 1000);
  }
};

}


// This is declared in the WireFormat class.
WireFormat*
WireFormat::newInitialDefaultWireFormat() 
{
  return new BinaryXmlWireFormat();
}
  
Blob 
BinaryXmlWireFormat::encodeInterest(const Interest& interest) 
{
  struct ndn_NameComponent nameComponents[100];
  struct ndn_ExcludeEntry excludeEntries[100];
  struct ndn_Interest interestStruct;
  ndn_Interest_initialize
    (&interestStruct, nameComponents, sizeof(nameComponents) / sizeof(nameComponents[0]), 
     excludeEntries, sizeof(excludeEntries) / sizeof(excludeEntries[0]));
  reinterpret_cast<const c::Interest&>(interest).get(interestStruct);

  BinaryXmlEncoder encoder;
  ndn_Error error;
  if ((error = ndn_encodeBinaryXmlInterest(&interestStruct, &encoder)))
    throw runtime_error(ndn_getErrorString(error));
     
  return encoder.getOutput();
}

void 
BinaryXmlWireFormat::decodeInterest(Interest& interest, const uint8_t *input, size_t inputLength)
{
  struct ndn_NameComponent nameComponents[100];
  struct ndn_ExcludeEntry excludeEntries[100];
  struct ndn_Interest interestStruct;
  ndn_Interest_initialize
    (&interestStruct, nameComponents, sizeof(nameComponents) / sizeof(nameComponents[0]), 
     excludeEntries, sizeof(excludeEntries) / sizeof(excludeEntries[0]));
    
  BinaryXmlDecoder decoder(input, inputLength);  
  ndn_Error error;
  if ((error = ndn_decodeBinaryXmlInterest(&interestStruct, &decoder)))
    throw runtime_error(ndn_getErrorString(error));

  reinterpret_cast<c::Interest&>(interest).set(interestStruct);
}

Blob 
BinaryXmlWireFormat::encodeData(const Data& data, size_t *signedPortionBeginOffset, size_t *signedPortionEndOffset) 
{
  struct ndn_NameComponent nameComponents[100];
  struct ndn_NameComponent keyNameComponents[100];
  struct ndn_Data dataStruct;
  ndn_Data_initialize
    (&dataStruct, nameComponents, sizeof(nameComponents) / sizeof(nameComponents[0]), 
     keyNameComponents, sizeof(keyNameComponents) / sizeof(keyNameComponents[0]));
  reinterpret_cast<const c::Data&>(data).get(dataStruct);

  BinaryXmlEncoder encoder(1500);
  ndn_Error error;
  if ((error = ndn_encodeBinaryXmlData(&dataStruct, signedPortionBeginOffset, signedPortionEndOffset, &encoder)))
    throw runtime_error(ndn_getErrorString(error));
     
  return encoder.getOutput();
}

void 
BinaryXmlWireFormat::decodeData
  (Data& data, const uint8_t *input, size_t inputLength, size_t *signedPortionBeginOffset, size_t *signedPortionEndOffset)
{
  struct ndn_NameComponent nameComponents[100];
  struct ndn_NameComponent keyNameComponents[100];
  struct ndn_Data dataStruct;
  ndn_Data_initialize
    (&dataStruct, nameComponents, sizeof(nameComponents) / sizeof(nameComponents[0]), 
     keyNameComponents, sizeof(keyNameComponents) / sizeof(keyNameComponents[0]));
    
  BinaryXmlDecoder decoder(input, inputLength);  
  ndn_Error error;
  if ((error = ndn_decodeBinaryXmlData(&dataStruct, signedPortionBeginOffset, signedPortionEndOffset, &decoder)))
    throw runtime_error(ndn_getErrorString(error));

  reinterpret_cast<c::Data&>(data).set(dataStruct);
}

Blob 
BinaryXmlWireFormat::encodeForwardingEntry(const ForwardingEntry& forwardingEntry) 
{
  struct ndn_NameComponent prefixNameComponents[100];
  struct ndn_ForwardingEntry forwardingEntryStruct;
  ndn_ForwardingEntry_initialize
    (&forwardingEntryStruct, prefixNameComponents, sizeof(prefixNameComponents) / sizeof(prefixNameComponents[0]));
  reinterpret_cast<const c::ForwardingEntry&>(forwardingEntry).get(forwardingEntryStruct);

  BinaryXmlEncoder encoder;
  ndn_Error error;
  if ((error = ndn_encodeBinaryXmlForwardingEntry(&forwardingEntryStruct, &encoder)))
    throw runtime_error(ndn_getErrorString(error));
     
  return encoder.getOutput();
}

void 
BinaryXmlWireFormat::decodeForwardingEntry(ForwardingEntry& forwardingEntry, const uint8_t *input, size_t inputLength)
{
  struct ndn_NameComponent prefixNameComponents[100];
  struct ndn_ForwardingEntry forwardingEntryStruct;
  ndn_ForwardingEntry_initialize
    (&forwardingEntryStruct, prefixNameComponents, sizeof(prefixNameComponents) / sizeof(prefixNameComponents[0]));
    
  BinaryXmlDecoder decoder(input, inputLength);  
  ndn_Error error;
  if ((error = ndn_decodeBinaryXmlForwardingEntry(&forwardingEntryStruct, &decoder)))
    throw runtime_error(ndn_getErrorString(error));

  reinterpret_cast<c::ForwardingEntry&>(forwardingEntry).set(forwardingEntryStruct);
}

}
