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
  /** \brief a callback on signing command interest
   */
  typedef function<void(Interest&)> Sign;

  /// \deprecated
  typedef function<void(const PrefixRegOptions&)> CommandSucceedCallback;

  explicit
  Controller(Face& face);

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

  // selfreg using RIB Management commands
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
  /// \deprecated
  void
  processCommandResponse(Data& data,
                         const CommandSucceedCallback& onSuccess,
                         const FailCallback& onFail);
};

} // namespace nrd
} // namespace ndn

#endif // NDN_MANAGEMENT_NRD_CONTROLLER_HPP
