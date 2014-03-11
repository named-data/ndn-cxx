/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2014 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_MANAGEMENT_NFD_CONTROLLER_HPP
#define NDN_MANAGEMENT_NFD_CONTROLLER_HPP

#include "controller.hpp"
#include "../util/command-interest-generator.hpp"

namespace ndn {
namespace nrd {

class PrefixRegOptions;

class Controller : public ndn::Controller
{
public:
  typedef function<void(const PrefixRegOptions&)> CommandSucceedCallback;

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

  void
  registerPrefix(const PrefixRegOptions& options,
                 const CommandSucceedCallback& onSuccess,
                 const FailCallback& onFail);

  void
  unregisterPrefix(const PrefixRegOptions& options,
                 const CommandSucceedCallback& onSuccess,
                 const FailCallback&    onFail);

  void
  advertisePrefix(const PrefixRegOptions& options,
                  const CommandSucceedCallback& onSuccess,
                  const FailCallback& onFail);

  void
  withdrawPrefix(const PrefixRegOptions& options,
                 const CommandSucceedCallback& onSuccess,
                 const FailCallback& onFail);

protected:
  void
  startCommand(const std::string& command,
               const PrefixRegOptions& options,
               const CommandSucceedCallback& onSuccess,
               const FailCallback& onFailure);

private:
  void
  processCommandResponse(Data& data,
                         const CommandSucceedCallback& onSuccess,
                         const FailCallback& onFail);

protected:
  Face& m_face;
  CommandInterestGenerator m_commandInterestGenerator;
};

} // namespace nrd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_CONTROLLER_HPP
