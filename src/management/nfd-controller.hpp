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

  /** \brief a callback on signing command interest
   */
  typedef function<void(Interest&)> Sign;

  explicit
  Controller(Face& face);

  /** \brief start command execution
   */
  template<typename Command>
  void
  start(const ControlParameters& parameters,
        const CommandSucceedCallback& onSuccess,
        const CommandFailCallback& onFailure,
        const time::milliseconds& timeout = getDefaultCommandTimeout())
  {
    start<Command>(parameters, onSuccess, onFailure,
                   bind(&CommandInterestGenerator::generate,
                        &m_commandInterestGenerator, _1,
                        boost::cref(Name())),
                   timeout);
  }

  template<typename Command>
  void
  start(const ControlParameters& parameters,
        const CommandSucceedCallback& onSuccess,
        const CommandFailCallback& onFailure,
        const IdentityCertificate& certificate,
        const time::milliseconds& timeout = getDefaultCommandTimeout())
  {
    start<Command>(parameters, onSuccess, onFailure,
                   bind(&CommandInterestGenerator::generate,
                        &m_commandInterestGenerator, _1,
                        boost::cref(certificate.getName())),
                   timeout);
  }

  template<typename Command>
  void
  start(const ControlParameters& parameters,
        const CommandSucceedCallback& onSuccess,
        const CommandFailCallback& onFailure,
        const Name& identity,
        const time::milliseconds& timeout = getDefaultCommandTimeout())
  {
    start<Command>(parameters, onSuccess, onFailure,
                   bind(&CommandInterestGenerator::generateWithIdentity,
                        &m_commandInterestGenerator, _1,
                        boost::cref(identity)),
                   timeout);
  }

public: // selfreg using FIB Management commands
  virtual void
  selfRegisterPrefix(const Name& prefixToRegister,
                     const SuccessCallback& onSuccess,
                     const FailCallback&    onFail)
  {
    this->selfRegisterPrefix(prefixToRegister, onSuccess, onFail,
                             bind(&CommandInterestGenerator::generate,
                                  &m_commandInterestGenerator, _1,
                                  boost::cref(Name())));
  }

  virtual void
  selfRegisterPrefix(const Name& prefixToRegister,
                     const SuccessCallback& onSuccess,
                     const FailCallback&    onFail,
                     const IdentityCertificate& certificate)
  {
    this->selfRegisterPrefix(prefixToRegister, onSuccess, onFail,
                             bind(&CommandInterestGenerator::generate,
                                  &m_commandInterestGenerator, _1,
                                  boost::cref(certificate.getName())));
  }

  virtual void
  selfRegisterPrefix(const Name& prefixToRegister,
                     const SuccessCallback& onSuccess,
                     const FailCallback&    onFail,
                     const Name& identity)
  {
    this->selfRegisterPrefix(prefixToRegister, onSuccess, onFail,
                             bind(&CommandInterestGenerator::generateWithIdentity,
                                  &m_commandInterestGenerator, _1,
                                  boost::cref(identity)));
  }

  virtual void
  selfDeregisterPrefix(const Name& prefixToDeRegister,
                       const SuccessCallback& onSuccess,
                       const FailCallback&    onFail)
  {
    this->selfDeregisterPrefix(prefixToDeRegister, onSuccess, onFail,
                               bind(&CommandInterestGenerator::generate,
                                    &m_commandInterestGenerator, _1,
                                    boost::cref(Name())));
  }

  virtual void
  selfDeregisterPrefix(const Name& prefixToDeRegister,
                       const SuccessCallback& onSuccess,
                       const FailCallback&    onFail,
                       const IdentityCertificate& certificate)
  {
    this->selfDeregisterPrefix(prefixToDeRegister, onSuccess, onFail,
                               bind(&CommandInterestGenerator::generate,
                                    &m_commandInterestGenerator, _1,
                                    boost::cref(certificate.getName())));
  }

  virtual void
  selfDeregisterPrefix(const Name& prefixToDeRegister,
                       const SuccessCallback& onSuccess,
                       const FailCallback&    onFail,
                       const Name& identity)
  {
    this->selfDeregisterPrefix(prefixToDeRegister, onSuccess, onFail,
                               bind(&CommandInterestGenerator::generateWithIdentity,
                                    &m_commandInterestGenerator, _1,
                                    boost::cref(identity)));
  }

protected:
  template<typename Command>
  void
  start(const ControlParameters& parameters,
        const CommandSucceedCallback& onSuccess,
        const CommandFailCallback& onFailure,
        const Sign& sign,
        const time::milliseconds& timeout = getDefaultCommandTimeout());

  virtual void
  selfRegisterPrefix(const Name& prefixToRegister,
                     const SuccessCallback& onSuccess,
                     const FailCallback&    onFail,
                     const Sign& sign);

  virtual void
  selfDeregisterPrefix(const Name& prefixToDeRegister,
                       const SuccessCallback& onSuccess,
                       const FailCallback&    onFail,
                       const Sign& sign);

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
                  const Sign& sign,
                  const time::milliseconds& timeout)
{
  BOOST_ASSERT(timeout > time::milliseconds::zero());

  shared_ptr<ControlCommand> command = make_shared<Command>();

  Interest commandInterest = command->makeCommandInterest(parameters, sign);

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
