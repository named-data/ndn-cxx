/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2014 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_MANAGEMENT_NRD_CONTROLLER_HPP
#define NDN_MANAGEMENT_NRD_CONTROLLER_HPP

#include "nfd-controller.hpp"

namespace ndn {
namespace nrd {

/// \deprecated
class PrefixRegOptions;

class Controller : public nfd::Controller
{
public:
  /// \deprecated
  typedef function<void(const PrefixRegOptions&)> CommandSucceedCallback;

  explicit
  Controller(Face& face);

public: // selfreg using RIB Management commands
  virtual void
  selfRegisterPrefix(const Name& prefixToRegister,
                     const SuccessCallback& onSuccess,
                     const FailCallback&    onFail);

  virtual void
  selfDeregisterPrefix(const Name& prefixToRegister,
                       const SuccessCallback& onSuccess,
                       const FailCallback&    onFail);

public:
  /// \deprecated .start<RibRegisterCommand>
  void
  registerPrefix(const PrefixRegOptions& options,
                 const CommandSucceedCallback& onSuccess,
                 const FailCallback& onFail);

  /// \deprecated .start<RibUnregisterCommand>
  void
  unregisterPrefix(const PrefixRegOptions& options,
                 const CommandSucceedCallback& onSuccess,
                 const FailCallback&    onFail);

  /// \deprecated
  void
  advertisePrefix(const PrefixRegOptions& options,
                  const CommandSucceedCallback& onSuccess,
                  const FailCallback& onFail);

  /// \deprecated
  void
  withdrawPrefix(const PrefixRegOptions& options,
                 const CommandSucceedCallback& onSuccess,
                 const FailCallback& onFail);

protected:
  /// \deprecated
  void
  startCommand(const std::string& command,
               const PrefixRegOptions& options,
               const CommandSucceedCallback& onSuccess,
               const FailCallback& onFailure);

private:
  /// \deprecated
  void
  processCommandResponse(Data& data,
                         const CommandSucceedCallback& onSuccess,
                         const FailCallback& onFail);
};

} // namespace nrd
} // namespace ndn

#endif // NDN_MANAGEMENT_NRD_CONTROLLER_HPP
