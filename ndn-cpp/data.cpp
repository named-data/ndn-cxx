/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include "common.hpp"
#include "data.hpp"
#include "sha256-with-rsa-signature.hpp"

using namespace std;

namespace ndn {

Signature::~Signature()
{
}
  
void MetaInfo::get(struct ndn_MetaInfo& metaInfoStruct) const 
{
  metaInfoStruct.timestampMilliseconds = timestampMilliseconds_;
  metaInfoStruct.type = type_;
  metaInfoStruct.freshnessSeconds = freshnessSeconds_;
  finalBlockID_.get(metaInfoStruct.finalBlockID);
}

void MetaInfo::set(const struct ndn_MetaInfo& metaInfoStruct)
{
  timestampMilliseconds_ = metaInfoStruct.timestampMilliseconds;
  type_ = metaInfoStruct.type;
  freshnessSeconds_ = metaInfoStruct.freshnessSeconds;
  finalBlockID_.setValue(Blob(metaInfoStruct.finalBlockID.value, metaInfoStruct.finalBlockID.valueLength));
}

Data::Data()
: signature_(new Sha256WithRsaSignature())
{
}
  
Data::Data(const Name& name)
: name_(name), signature_(new Sha256WithRsaSignature())
{
}

void Data::get(struct ndn_Data& dataStruct) const 
{
  signature_->get(dataStruct.signature);
  name_.get(dataStruct.name);
  metaInfo_.get(dataStruct.metaInfo);
  
  dataStruct.contentLength = content_.size();
  if (content_.size() > 0)
    dataStruct.content = (unsigned char*)content_.buf();
  else
    dataStruct.content = 0;
}

void Data::set(const struct ndn_Data& dataStruct)
{
  signature_->set(dataStruct.signature);
  name_.set(dataStruct.name);
  metaInfo_.set(dataStruct.metaInfo);
  content_ = Blob(dataStruct.content, dataStruct.contentLength);
}

}
