/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_MANAGEMENT_NFD_CONTROLLER_HPP
#define NDN_MANAGEMENT_NFD_CONTROLLER_HPP

#include "controller.hpp"
#include "nfd-control-command.hpp"
#include "../face.hpp"

namespace ndn {
namespace nfd {

/** \brief NFD Management protocol - ControlCommand client
 */
class Controller : public ndn::Controller
{
public:
  /** \brief a callback on command success
   */
  typedef function<void(const ControlParameters&)> CommandSucceedCallback;

  /** \brief a callback on command failure
   */
  typedef function<void(uint32_t/*code*/,const std::string&/*reason*/)> CommandFailCallback;

  explicit
  Controller(Face& face);

  /** \brief start command execution
   */
  template<typename Command>
  void
  start(const ControlParameters& parameters,
        const CommandSucceedCallback& onSuccess,
        const CommandFailCallback& onFailure,
        time::milliseconds timeout = getDefaultCommandTimeout());

public: // selfreg using FIB Management commands
  virtual void
  selfRegisterPrefix(const Name& prefixToRegister,
                     const SuccessCallback& onSuccess,
                     const FailCallback&    onFail);

  virtual void
  selfDeregisterPrefix(const Name& prefixToDeRegister,
                       const SuccessCallback& onSuccess,
                       const FailCallback&    onFail);

private:
  void
  processCommandResponse(const Data& data,
                         const shared_ptr<ControlCommand>& command,
                         const CommandSucceedCallback& onSuccess,
                         const CommandFailCallback& onFailure);

public:
  static time::milliseconds
  getDefaultCommandTimeout()
  {
    return time::milliseconds(10000);
  }

protected:
  Face& m_face;
  CommandInterestGenerator m_commandInterestGenerator;
};


template<typename Command>
void
Controller::start(const ControlParameters& parameters,
                  const CommandSucceedCallback& onSuccess,
                  const CommandFailCallback&    onFailure,
                  time::milliseconds timeout)
{
  BOOST_ASSERT(timeout > time::milliseconds::zero());

  shared_ptr<ControlCommand> command = make_shared<Command>();

  Interest commandInterest = command->makeCommandInterest(parameters, m_commandInterestGenerator);
  commandInterest.setInterestLifetime(timeout);

  // http://msdn.microsoft.com/en-us/library/windows/desktop/ms740668.aspx
  const uint32_t timeoutCode = 10060;
  m_face.expressInterest(commandInterest,
                         bind(&Controller::processCommandResponse, this, _2,
                              command, onSuccess, onFailure),
                         bind(onFailure, timeoutCode, "Command Interest timed out"));
}

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_CONTROLLER_HPP
