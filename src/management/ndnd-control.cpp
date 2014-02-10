/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include "common.hpp"
#include "../node.hpp"
#include "../security/signature-sha256-with-rsa.hpp"
#include "../util/random.hpp"

#include "ndnd-forwarding-entry.hpp"
#include "ndnd-face-instance.hpp"
#include "ndnd-status-response.hpp"

namespace ndn {
namespace ndnd {

Control::Control(Node& face)
  : m_face(face)
  , m_faceId(-1)
{
}

void
Control::selfRegisterPrefix(const Name& prefixToRegister,
                            const SuccessCallback& onSuccess,
                            const FailCallback&    onFail)
{
  if (!m_ndndId.hasValue())
    {
      if (m_filterRequests.empty())
        {
          m_face.expressInterest(Name("/%C1.M.S.localhost/%C1.M.SRV/ndnd/KEY"),
                                 bind(&Control::onNdnidFetched, this, _1, _2),
                                 bind(onFail));
        }
      m_filterRequests.push_back(FilterRequest(prefixToRegister, onSuccess, onFail));
    }
  else
    startPrefixAction(ForwardingEntry("selfreg", prefixToRegister),
                      bind(&Control::recordSelfRegisteredFaceId, this, _1, onSuccess),
                      onFail);
}


void
Control::selfDeregisterPrefix(const Name& prefixToRegister,
                              const SuccessCallback& onSuccess,
                              const FailCallback&    onFail)
{
  if (!m_ndndId.hasValue() || m_faceId == -1)
    {
      if (static_cast<bool>(onFail))
        onFail();
      return;
    }

  startPrefixAction(ForwardingEntry("unreg", prefixToRegister, m_faceId),
                    bind(onSuccess), onFail);
}


void 
Control::onNdnidFetched(const shared_ptr<const Interest>& interest,
                        const shared_ptr<Data>& data)
{
  if (data->getName().size() > interest->getName().size())
    {
      m_ndndId = data->getName()[interest->getName().size()];

      for (FilterRequestList::iterator i = m_filterRequests.begin();
           i != m_filterRequests.end();
           ++i)
        {
          startPrefixAction(ForwardingEntry("selfreg", i->m_prefixToRegister),
                            bind(&Control::recordSelfRegisteredFaceId, this, _1, i->m_onSuccess),
                            i->m_onFailure);
        }
    }
  else
    {
      for (FilterRequestList::iterator i = m_filterRequests.begin();
           i != m_filterRequests.end();
           ++i)
        {
          if (static_cast<bool>(i->m_onFailure))
              i->m_onFailure();
        }
    }
  m_filterRequests.clear();
}

void
Control::recordSelfRegisteredFaceId(const ForwardingEntry& entry,
                                    const SuccessCallback& onSuccess)
{
  m_faceId = entry.getFaceId();
  if (static_cast<bool>(onSuccess))
    onSuccess();
}

void
Control::startFaceAction(const FaceInstance& entry,
                         const FaceOperationSucceedCallback& onSuccess,
                         const FailCallback& onFailure)
{
  // Set the ForwardingEntry as the content of a Data packet and sign.
  Data data;
  data.setName(Name().appendVersion(ndn::random::generateWord32()));
  data.setContent(entry.wireEncode());
  
  // Create an empty signature, since nobody going to verify it for now
  // @todo In the future, we may require real signatures to do the registration
  SignatureSha256WithRsa signature;
  signature.setValue(Block(Tlv::SignatureValue));
  data.setSignature(signature);

  // Create an interest where the name has the encoded Data packet.
  Name interestName;
  interestName.append("ndnx");
  interestName.append(m_ndndId);
  interestName.append(entry.getAction());
  interestName.append(data.wireEncode());

  Interest interest(interestName);
  interest.setScope(1);
  interest.setInterestLifetime(1000);
  interest.setMustBeFresh(true);

  m_face.expressInterest(interest,
                         bind(&Control::processFaceActionResponse, this, _2, onSuccess, onFailure),
                         bind(onFailure));
}

void
Control::startPrefixAction(const ForwardingEntry& entry,
                           const PrefixOperationSucceedCallback& onSuccess,
                           const FailCallback& onFailure)
{
  // Set the ForwardingEntry as the content of a Data packet and sign.
  Data data;
  data.setName(Name().appendVersion(random::generateWord32()));
  data.setContent(entry.wireEncode());
  
  // Create an empty signature, since nobody going to verify it for now
  // @todo In the future, we may require real signatures to do the registration
  SignatureSha256WithRsa signature;
  signature.setValue(Block(Tlv::SignatureValue));
  data.setSignature(signature);

  // Create an interest where the name has the encoded Data packet.
  Name interestName;
  interestName.append("ndnx");
  interestName.append(m_ndndId);
  interestName.append(entry.getAction());
  interestName.append(data.wireEncode());

  Interest interest(interestName);
  interest.setScope(1);
  interest.setInterestLifetime(1000);
  interest.setMustBeFresh(true);

  m_face.expressInterest(interest,
                         bind(&Control::processPrefixActionResponse, this, _2, onSuccess, onFailure),
                         bind(onFailure));
}

void
Control::processFaceActionResponse(const shared_ptr<Data>& data,
                                   const FaceOperationSucceedCallback& onSuccess,
                                   const FailCallback& onFail)
{
  Block content = data->getContent();
  content.parse();

  if (content.getAll().empty())
    {
      if (static_cast<bool>(onFail))
        onFail();
      return;
    }

  Block::element_iterator val = content.getAll().begin();
  
  switch(val->type())
    {
    case Tlv::FaceManagement::FaceInstance:
      {
        FaceInstance entry;
        entry.wireDecode(*val);

        if (static_cast<bool>(onSuccess))
          onSuccess(entry);
        return;
      }
    case Tlv::FaceManagement::StatusResponse:
      {
        StatusResponse resp;
        resp.wireDecode(*val);

        if (static_cast<bool>(onFail))
          onFail();
        return;
      }
    default:
      {
        if (static_cast<bool>(onFail))
          onFail();
        return;
      }
    }
}

void
Control::processPrefixActionResponse(const shared_ptr<Data>& data,
                                     const PrefixOperationSucceedCallback& onSuccess,
                                     const FailCallback& onFail)
{
  Block content = data->getContent();
  content.parse();

  if (content.getAll().empty())
    {
      if (static_cast<bool>(onFail))
        onFail();
      return;
    }

  Block::element_iterator val = content.getAll().begin();
  
  switch(val->type())
    {
    case Tlv::FaceManagement::ForwardingEntry:
      {
        ForwardingEntry entry;
        entry.wireDecode(*val);

        if (static_cast<bool>(onSuccess))
          onSuccess(entry);
        return;
      }
    case Tlv::FaceManagement::StatusResponse:
      {
        StatusResponse resp;
        resp.wireDecode(*val);

        // std::cerr << "StatusReponse: " << resp << std::endl;
      
        if (static_cast<bool>(onFail))
          onFail();
        return;
      }
    default:
      {
        if (static_cast<bool>(onFail))
          onFail();
        return;
      }
    }
}

} // namespace ndnd
} // namespace ndn
