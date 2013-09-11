/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include "common.hpp"
#include "data.hpp"

using namespace std;

namespace ndn {

void Signature::get(struct ndn_Signature& signatureStruct) const 
{
  signatureStruct.digestAlgorithmLength = digestAlgorithm_.size();
  if (digestAlgorithm_.size() > 0)
    signatureStruct.digestAlgorithm = (unsigned char *)&digestAlgorithm_[0];
  else
    signatureStruct.digestAlgorithm = 0;

  signatureStruct.witnessLength = witness_.size();
  if (witness_.size() > 0)
    signatureStruct.witness = (unsigned char *)&witness_[0];
  else
    signatureStruct.witness = 0;

  signatureStruct.signatureLength = signature_.size();
  if (signature_.size() > 0)
    signatureStruct.signature = (unsigned char *)&signature_[0];
  else
    signatureStruct.signature = 0;
}

void Signature::set(const struct ndn_Signature& signatureStruct)
{
  setVector(digestAlgorithm_, signatureStruct.digestAlgorithm, signatureStruct.digestAlgorithmLength);
  setVector(witness_, signatureStruct.witness, signatureStruct.witnessLength);
  setVector(signature_, signatureStruct.signature, signatureStruct.signatureLength);
}

void MetaInfo::get(struct ndn_MetaInfo& metaInfoStruct) const 
{
  publisherPublicKeyDigest_.get(metaInfoStruct.publisherPublicKeyDigest);
  metaInfoStruct.timestampMilliseconds = timestampMilliseconds_;
  metaInfoStruct.type = type_;
  metaInfoStruct.freshnessSeconds = freshnessSeconds_;
  
  metaInfoStruct.finalBlockIDLength = finalBlockID_.size();
  if (finalBlockID_.size() > 0)
    metaInfoStruct.finalBlockID = (unsigned char *)&finalBlockID_[0];
  else
    metaInfoStruct.finalBlockID = 0;

  keyLocator_.get(metaInfoStruct.keyLocator);
}

void MetaInfo::set(const struct ndn_MetaInfo& metaInfoStruct)
{
  publisherPublicKeyDigest_.set(metaInfoStruct.publisherPublicKeyDigest);
  timestampMilliseconds_ = metaInfoStruct.timestampMilliseconds;
  type_ = metaInfoStruct.type;
  freshnessSeconds_ = metaInfoStruct.freshnessSeconds;
  setVector(finalBlockID_, metaInfoStruct.finalBlockID, metaInfoStruct.finalBlockIDLength);
  keyLocator_.set(metaInfoStruct.keyLocator);
}

void Data::get(struct ndn_Data& dataStruct) const 
{
  signature_.get(dataStruct.signature);
  name_.get(dataStruct.name);
  metaInfo_.get(dataStruct.metaInfo);
  
  dataStruct.contentLength = content_.size();
  if (content_.size() > 0)
    dataStruct.content = (unsigned char *)&content_[0];
  else
    dataStruct.content = 0;
}

void Data::set(const struct ndn_Data& dataStruct)
{
  signature_.set(dataStruct.signature);
  name_.set(dataStruct.name);
  metaInfo_.set(dataStruct.metaInfo);
  setVector(content_, dataStruct.content, dataStruct.contentLength);
}

}
