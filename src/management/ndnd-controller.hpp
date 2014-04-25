/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (c) 2013-2014,  Regents of the University of California.
 * All rights reserved.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * This file licensed under New BSD License.  See COPYING for detailed information about
 * ndn-cxx library copyright, permissions, and redistribution restrictions.
 */

#ifndef NDN_MANAGEMENT_NDND_CONTROLLER_HPP
#define NDN_MANAGEMENT_NDND_CONTROLLER_HPP

#include "controller.hpp"

namespace ndn {
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
  Controller(Face& face);

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
  onNdnidFetched(const Interest& interest, Data& data);

  void
  recordSelfRegisteredFaceId(const ForwardingEntry& entry,
                             const SuccessCallback& onSuccess);

  void
  processFaceActionResponse(Data& data,
                            const FaceOperationSucceedCallback& onSuccess,
                            const FailCallback&    onFail);

  void
  processPrefixActionResponse(Data& data,
                              const PrefixOperationSucceedCallback& onSuccess,
                              const FailCallback&    onFail);

private:
  Face& m_face;
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
