/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_MANAGEMENT_NFD_CONTROLLER_HPP
#define NDN_MANAGEMENT_NFD_CONTROLLER_HPP

#include "controller.hpp"
#include "nfd-control-parameters.hpp"
#include "../util/command-interest-generator.hpp"

namespace ndn {

namespace nfd {

class Controller : public ndn::Controller
{
public:
  typedef function<void(const FibManagementOptions&)> FibCommandSucceedCallback;
  typedef function<void(const FaceManagementOptions&)> FaceCommandSucceedCallback;
  typedef function<void(const StrategyChoiceOptions&)> StrategyChoiceCommandSucceedCallback;

  /**
   * @brief Construct ndnd::Control object
   */
  Controller(Face& face);

  virtual void
  selfRegisterPrefix(const Name& prefixToRegister,
                     const SuccessCallback& onSuccess,
                     const FailCallback&    onFail);

  virtual void
  selfDeregisterPrefix(const Name& prefixToDeRegister,
                       const SuccessCallback& onSuccess,
                       const FailCallback&    onFail);

  /**
   * \brief Adds a nexthop to an existing or new FIB entry
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

  void
  startStrategyChoiceCommand(const std::string& command,
                             const StrategyChoiceOptions& options,
                             const StrategyChoiceCommandSucceedCallback& onSuccess,
                             const FailCallback& onFailure);

private:
  void
  processFibCommandResponse(Data& data,
                            const FibCommandSucceedCallback& onSuccess,
                            const FailCallback& onFail);

  void
  processFaceCommandResponse(Data& data,
                             const FaceCommandSucceedCallback& onSuccess,
                             const FailCallback& onFail);

  void
  processStrategyChoiceCommandResponse(Data& data,
                                       const StrategyChoiceCommandSucceedCallback& onSuccess,
                                       const FailCallback& onFail);

protected:
  Face& m_face;
  CommandInterestGenerator m_commandInterestGenerator;
};

} // namespace nfd
} // namespace ndn

#endif // NDN_MANAGEMENT_NFD_CONTROLLER_HPP
