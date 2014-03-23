/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#include "common.hpp"
#include "ndnd-controller.hpp"

#include "../face.hpp"
#include "../security/signature-sha256-with-rsa.hpp"
#include "../util/random.hpp"

#include "ndnd-forwarding-entry.hpp"
#include "ndnd-face-instance.hpp"
#include "ndnd-status-response.hpp"

namespace ndn {
namespace ndnd {

Controller::Controller(Face& face)
  : m_face(face)
  , m_faceId(-1)
{
}

void
Controller::selfRegisterPrefix(const Name& prefixToRegister,
                               const SuccessCallback& onSuccess,
                               const FailCallback&    onFail)
{
  if (!m_ndndId.hasValue())
    {
      if (m_filterRequests.empty())
        {
          m_face.expressInterest(Name("/%C1.M.S.localhost/%C1.M.SRV/ndnd/KEY"),
                                 bind(&Controller::onNdnidFetched, this, _1, _2),
                                 bind(onFail, "NDNDID fetching timed out"));
        }
      m_filterRequests.push_back(FilterRequest(prefixToRegister, onSuccess, onFail));
    }
  else
    startPrefixAction(ForwardingEntry("selfreg", prefixToRegister),
                      bind(&Controller::recordSelfRegisteredFaceId, this, _1, onSuccess),
                      onFail);
}


void
Controller::selfDeregisterPrefix(const Name& prefixToRegister,
                                 const SuccessCallback& onSuccess,
                                 const FailCallback&    onFail)
{
  if (!m_ndndId.hasValue() || m_faceId == -1)
    {
      if (static_cast<bool>(onFail))
        onFail("NDNID is not available (must have been present after a successful registration operation)");
      return;
    }

  startPrefixAction(ForwardingEntry("unreg", prefixToRegister, m_faceId),
                    bind(onSuccess), onFail);
}


void
Controller::onNdnidFetched(const Interest& interest, Data& data)
{
  if (data.getName().size() > interest.getName().size())
    {
      m_ndndId = data.getName()[interest.getName().size()];

      if (m_ndndId.value_size() < 6)
        {
          for (FilterRequestList::iterator i = m_filterRequests.begin();
               i != m_filterRequests.end();
               ++i)
            {
              if (static_cast<bool>(i->m_onFailure))
                i->m_onFailure("Fetched unrecognized NDNID");
            }

          return;
        }

      for (FilterRequestList::iterator i = m_filterRequests.begin();
           i != m_filterRequests.end();
           ++i)
        {
          startPrefixAction(ForwardingEntry("selfreg", i->m_prefixToRegister),
                            bind(&Controller::recordSelfRegisteredFaceId, this, _1, i->m_onSuccess),
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
              i->m_onFailure("NDNID cannot be fetched");
        }
    }
  m_filterRequests.clear();
}

void
Controller::recordSelfRegisteredFaceId(const ForwardingEntry& entry,
                                       const SuccessCallback& onSuccess)
{
  m_faceId = entry.getFaceId();
  if (static_cast<bool>(onSuccess))
    onSuccess();
}

void
Controller::startFaceAction(const FaceInstance& entry,
                            const FaceOperationSucceedCallback& onSuccess,
                            const FailCallback& onFail)
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
  interestName.append(m_ndndId.value_begin()+6, m_ndndId.value_end());
  interestName.append(entry.getAction());
  interestName.append(data.wireEncode());

  Interest interest(interestName);
  interest.setScope(1);
  interest.setInterestLifetime(time::seconds(1));
  interest.setMustBeFresh(true);

  m_face.expressInterest(interest,
                         bind(&Controller::processFaceActionResponse, this, _2, onSuccess, onFail),
                         bind(onFail, "Command Interest failed"));
}

void
Controller::startPrefixAction(const ForwardingEntry& entry,
                              const PrefixOperationSucceedCallback& onSuccess,
                              const FailCallback& onFail)
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
  interestName.append(m_ndndId.value_begin() + 6, m_ndndId.value_end());
  interestName.append(entry.getAction());
  interestName.append(data.wireEncode());

  Interest interest(interestName);
  interest.setScope(1);
  interest.setInterestLifetime(time::seconds(1));
  interest.setMustBeFresh(true);

  m_face.expressInterest(interest,
                         bind(&Controller::processPrefixActionResponse, this, _2, onSuccess, onFail),
                         bind(onFail, "Command Interest timed out"));
}

void
Controller::processFaceActionResponse(Data& data,
                                      const FaceOperationSucceedCallback& onSuccess,
                                      const FailCallback& onFail)
{
  Block content = data.getContent();
  content.parse();

  if (content.elements().empty())
    {
      if (static_cast<bool>(onFail))
        onFail("Empty response");
      return;
    }

  Block::element_const_iterator val = content.elements_begin();

  switch(val->type())
    {
    case tlv::ndnd::FaceInstance:
      {
        FaceInstance entry;
        entry.wireDecode(*val);

        if (static_cast<bool>(onSuccess))
          onSuccess(entry);
        return;
      }
    case tlv::ndnd::StatusResponse:
      {
        StatusResponse resp;
        resp.wireDecode(*val);

        if (static_cast<bool>(onFail))
          onFail(resp.getInfo());
        return;
      }
    default:
      {
        if (static_cast<bool>(onFail))
          onFail("Invalid response");
        return;
      }
    }
}

void
Controller::processPrefixActionResponse(Data& data,
                                        const PrefixOperationSucceedCallback& onSuccess,
                                        const FailCallback& onFail)
{
  Block content = data.getContent();
  content.parse();

  if (content.elements().empty())
    {
      if (static_cast<bool>(onFail))
        onFail("Empty response");
      return;
    }

  Block::element_const_iterator val = content.elements_begin();

  switch(val->type())
    {
    case tlv::ndnd::ForwardingEntry:
      {
        ForwardingEntry entry;
        entry.wireDecode(*val);

        if (static_cast<bool>(onSuccess))
          onSuccess(entry);
        return;
      }
    case tlv::ndnd::StatusResponse:
      {
        StatusResponse resp;
        resp.wireDecode(*val);

        // std::cerr << "StatusReponse: " << resp << std::endl;

        if (static_cast<bool>(onFail))
          onFail(resp.getInfo());
        return;
      }
    default:
      {
        if (static_cast<bool>(onFail))
          onFail("Invalid response");
        return;
      }
    }
}

} // namespace ndnd
} // namespace ndn
