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
               const CommandFailCallback&    onFailure);

public: // selfreg
  virtual void
  selfRegisterPrefix(const Name& prefixToRegister,
                     const SuccessCallback& onSuccess,
                     const FailCallback&    onFail);

  virtual void
  selfDeregisterPrefix(const Name& prefixToDeRegister,
                       const SuccessCallback& onSuccess,
                       const FailCallback&    onFail);

public:
  /** \deprecated use CommandSucceedCallback instead
   */
  typedef function<void(const FibManagementOptions&)> FibCommandSucceedCallback;
  /** \deprecated use CommandSucceedCallback instead
   */
  typedef function<void(const FaceManagementOptions&)> FaceCommandSucceedCallback;
  /** \deprecated use CommandSucceedCallback instead
   */
  typedef function<void(const StrategyChoiceOptions&)> StrategyChoiceCommandSucceedCallback;

  /**
   * \brief Adds a nexthop to an existing or new FIB entry
   * \deprecated use startCommand instead
   *
   * If FIB entry for the specified prefix does not exist, it will be automatically created.
   *
   * \param prefix    Prefix of the FIB entry
   * \param faceId    ID of the face which should be added as a next hop for prefix FIB entry.
   *                  If a nexthop of same FaceId exists on the FIB entry, its cost is updated.
   *                  If FaceId is set to zero, it is implied as the face of the entity sending
   *                  this command.
   * \param cost      Cost that should be associated with the next hop
   * \param onSuccess Callback that will be called when operation succeeds
   * \param onFail    Callback that will be called when operation fails
   */
  void
  fibAddNextHop(const Name& prefix, uint64_t faceId, int cost,
                const FibCommandSucceedCallback& onSuccess,
                const FailCallback& onFail);

  /**
   * \brief Remove a nexthop from FIB entry
   * \deprecated use startCommand instead
   *
   * If after removal of the nexthop FIB entry has zero next hops, this FIB entry will
   * be automatically deleted.
   *
   * \param prefix    Prefix of the FIB entry
   * \param faceId    ID of the face which should be removed FIB entry.
   *                  If FaceId is set to zero, it is implied as the face of the entity sending
   *                  this command.
   * \param onSuccess Callback that will be called when operation succeeds
   * \param onFail    Callback that will be called when operation fails
   */
  void
  fibRemoveNextHop(const Name& prefix, uint64_t faceId,
                   const FibCommandSucceedCallback& onSuccess,
                   const FailCallback& onFail);

protected:
  /** \deprecated use startCommand instead
   */
  void
  startFibCommand(const std::string& command,
                  const FibManagementOptions& options,
                  const FibCommandSucceedCallback& onSuccess,
                  const FailCallback& onFailure);

  /** \deprecated use startCommand instead
   */
  void
  startFaceCommand(const std::string& command,
                   const FaceManagementOptions& options,
                   const FaceCommandSucceedCallback& onSuccess,
                   const FailCallback& onFailure);

  /** \deprecated use startCommand instead
   */
  void
  startStrategyChoiceCommand(const std::string& command,
                             const StrategyChoiceOptions& options,
                             const StrategyChoiceCommandSucceedCallback& onSuccess,
                             const FailCallback& onFailure);

private:
  void
  processCommandResponse(const Data& data,
                         const shared_ptr<ControlCommand>& command,
                         const CommandSucceedCallback& onSuccess,
                         const CommandFailCallback& onFailure);

protected:
  Face& m_face;
  CommandInterestGenerator m_commandInterestGenerator;
};


template<typename Command>
void
Controller::start(const ControlParameters& parameters,
                  const CommandSucceedCallback& onSuccess,
                  const CommandFailCallback&    onFailure)
{
  shared_ptr<ControlCommand> command = make_shared<Command>();

  Interest commandInterest = command->makeCommandInterest(parameters, m_commandInterestGenerator);

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
