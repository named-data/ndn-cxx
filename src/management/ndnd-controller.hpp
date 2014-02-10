/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_MANAGEMENT_NDND_CONTROLLER_HPP
#define NDN_MANAGEMENT_NDND_CONTROLLER_HPP

#include "../common.hpp"
#include "controller.hpp"

#include "../name.hpp"
#include "../interest.hpp"
#include "../data.hpp"

namespace ndn {

class Node;

namespace ndnd {

class FaceInstance;
class ForwardingEntry;

/*
 * @brief Class implementing Face and Prefix management operations for ndnd-tlv
 *
 * ndnd::Control should be used when connecting to ndnd-tlv daemon
 */
class Controller : public ndn::Controller
{
public:
  typedef function<void(const ForwardingEntry&)> PrefixOperationSucceedCallback;
  typedef function<void(const FaceInstance&)>    FaceOperationSucceedCallback;

  /**
   * @brief Construct ndnd::Control object
   */
  Controller(Node& face);

  virtual void
  selfRegisterPrefix(const Name& prefixToRegister,
                     const SuccessCallback& onSuccess,
                     const FailCallback&    onFail);

  virtual void
  selfDeregisterPrefix(const Name& prefixToRegister,
                       const SuccessCallback& onSuccess,
                       const FailCallback&    onFail);

protected:
  void
  startFaceAction(const FaceInstance& entry,
                  const FaceOperationSucceedCallback& onSuccess,
                  const FailCallback& onFailure);

  void
  startPrefixAction(const ForwardingEntry& entry,
                    const PrefixOperationSucceedCallback& onSuccess,
                    const FailCallback& onFailure);

private:
  void 
  onNdnidFetched(const shared_ptr<const Interest>& interest,
                 const shared_ptr<Data>& data);


  void
  recordSelfRegisteredFaceId(const ForwardingEntry& entry,
                             const SuccessCallback& onSuccess);

  void
  processFaceActionResponse(const shared_ptr<Data>& data,
                            const FaceOperationSucceedCallback& onSuccess,
                            const FailCallback&    onFail);

  void
  processPrefixActionResponse(const shared_ptr<Data>& data,
                              const PrefixOperationSucceedCallback& onSuccess,
                              const FailCallback&    onFail);

private:
  Node& m_face;
  Block m_ndndId;
  int64_t m_faceId; // internal face ID (needed for prefix de-registration)

  struct FilterRequest
  {
    FilterRequest(const Name& prefixToRegister,
                  const SuccessCallback& onSuccess,
                  const FailCallback&    onFailure)
      : m_prefixToRegister(prefixToRegister)
      , m_onSuccess(onSuccess)
      , m_onFailure(onFailure)
    {
    }
    
    Name             m_prefixToRegister;
    SuccessCallback  m_onSuccess;
    FailCallback     m_onFailure;
  };

  typedef std::list<FilterRequest> FilterRequestList;
  FilterRequestList m_filterRequests;
};

} // namespace ndnd
} // namespace ndn

#endif // NDN_MANAGEMENT_NDND_CONTROLLER_HPP
