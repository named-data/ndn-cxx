/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_MANAGEMENT_NFD_CONTROL_HPP
#define NDN_MANAGEMENT_NFD_CONTROL_HPP

#include "../common.hpp"
#include "controller.hpp"

#include "../name.hpp"
#include "../interest.hpp"
#include "../data.hpp"
#include "../security/key-chain.hpp"

namespace ndn {

class Name;

namespace nfd {

class FibManagementOptions;

class Controller : public ndn::Controller
{
public:
  typedef function<void(const FibManagementOptions&)> FibCommandSucceedCallback;

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

  void
  startFibCommand(const std::string& command,
                  const FibManagementOptions& options,
                  const FibCommandSucceedCallback& onSuccess,
                  const FailCallback& onFailure);
private:
  void
  recordSelfRegisteredFaceId(const FibManagementOptions& entry,
                             const SuccessCallback& onSuccess);

  // void
  // processFaceActionResponse(const shared_ptr<Data>& data,
  //                           const FaceOperationSucceedCallback& onSuccess,
  //                           const FailCallback&    onFail);

  void
  processFibCommandResponse(const shared_ptr<Data>& data,
                            const FibCommandSucceedCallback& onSuccess,
                            const FailCallback& onFail);
  
private:
  Node& m_face;
  KeyChain m_keyChain;
  uint64_t m_faceId; // internal face ID (needed for prefix de-registration)
};

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_CONTROL_HPP
