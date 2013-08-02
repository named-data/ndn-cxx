/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include "common.hpp"
#include "data.hpp"

using namespace std;

namespace ndn {

void Signature::get(struct ndn_Signature &signatureStruct) const 
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

void Signature::set(const struct ndn_Signature &signatureStruct)
{
  setVector(digestAlgorithm_, signatureStruct.digestAlgorithm, signatureStruct.digestAlgorithmLength);
  setVector(witness_, signatureStruct.witness, signatureStruct.witnessLength);
  setVector(signature_, signatureStruct.signature, signatureStruct.signatureLength);
}

void SignedInfo::get(struct ndn_SignedInfo &signedInfoStruct) const 
{
  publisherPublicKeyDigest_.get(signedInfoStruct.publisherPublicKeyDigest);
  signedInfoStruct.timestampMilliseconds = timestampMilliseconds_;
  signedInfoStruct.type = type_;
  signedInfoStruct.freshnessSeconds = freshnessSeconds_;
  
  signedInfoStruct.finalBlockIDLength = finalBlockID_.size();
  if (finalBlockID_.size() > 0)
    signedInfoStruct.finalBlockID = (unsigned char *)&finalBlockID_[0];
  else
    signedInfoStruct.finalBlockID = 0;

  keyLocator_.get(signedInfoStruct.keyLocator);
}

void SignedInfo::set(const struct ndn_SignedInfo &signedInfoStruct)
{
  publisherPublicKeyDigest_.set(signedInfoStruct.publisherPublicKeyDigest);
  timestampMilliseconds_ = signedInfoStruct.timestampMilliseconds;
  type_ = signedInfoStruct.type;
  freshnessSeconds_ = signedInfoStruct.freshnessSeconds;
  setVector(finalBlockID_, signedInfoStruct.finalBlockID, signedInfoStruct.finalBlockIDLength);
  keyLocator_.set(signedInfoStruct.keyLocator);
}

void Data::get(struct ndn_Data &dataStruct) const 
{
  signature_.get(dataStruct.signature);
  name_.get(dataStruct.name);
  signedInfo_.get(dataStruct.signedInfo);
  
  dataStruct.contentLength = content_.size();
  if (content_.size() > 0)
    dataStruct.content = (unsigned char *)&content_[0];
  else
    dataStruct.content = 0;
}

void Data::set(const struct ndn_Data &dataStruct)
{
  signature_.set(dataStruct.signature);
  name_.set(dataStruct.name);
  signedInfo_.set(dataStruct.signedInfo);
  setVector(content_, dataStruct.content, dataStruct.contentLength);
}

}
