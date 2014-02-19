/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_MANAGEMENT_NFD_CONTROL_HPP
#define NDN_MANAGEMENT_NFD_CONTROL_HPP

#include "controller.hpp"
#include "../security/key-chain.hpp"

namespace ndn {

namespace nfd {

class FibManagementOptions;
class FaceManagementOptions;

class Controller : public ndn::Controller
{
public:
  typedef function<void(const FibManagementOptions&)> FibCommandSucceedCallback;
  typedef function<void(const FaceManagementOptions&)> FaceCommandSucceedCallback;

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
  startFibCommand(const std::string& command,
                  const FibManagementOptions& options,
                  const FibCommandSucceedCallback& onSuccess,
                  const FailCallback& onFailure);

  void
  startFaceCommand(const std::string& command,
                   const FaceManagementOptions& options,
                   const FaceCommandSucceedCallback& onSuccess,
                   const FailCallback& onFailure);

private:
  void
  selfRegisterPrefixAddNextop(const FibManagementOptions& entry,
                              const SuccessCallback& onSuccess,
                              const FailCallback&    onFail);

  void
  recordSelfRegisteredFaceId(const FibManagementOptions& entry,
                             const SuccessCallback& onSuccess);

  void
  processFibCommandResponse(Data& data,
                            const FibCommandSucceedCallback& onSuccess,
                            const FailCallback& onFail);

  void
  processFaceCommandResponse(Data& data,
                             const FaceCommandSucceedCallback& onSuccess,
                             const FailCallback& onFail);

protected:
  Face& m_face;
  KeyChain m_keyChain;
  uint64_t m_faceId; // internal face ID (needed for prefix de-registration)
};

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_CONTROL_HPP
