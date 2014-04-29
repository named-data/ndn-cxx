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

#ifndef NDN_MANAGEMENT_CONTROLLER_HPP
#define NDN_MANAGEMENT_CONTROLLER_HPP

#include "../common.hpp"
#include "../name.hpp"
#include "../interest.hpp"
#include "../data.hpp"

namespace ndn {

class Name;
class Face;
class IdentityCertificate;

class Controller
{
public:
  typedef function<void()>                   SuccessCallback;
  typedef function<void(const std::string&)> FailCallback;

  virtual
  ~Controller()
  {
  }

  virtual void
  selfRegisterPrefix(const Name& prefixToRegister,
                     const SuccessCallback& onSuccess,
                     const FailCallback&    onFail) = 0;

  virtual void
  selfRegisterPrefix(const Name& prefixToRegister,
                     const SuccessCallback& onSuccess,
                     const FailCallback&    onFail,
                     const IdentityCertificate& certificate) = 0;

  virtual void
  selfRegisterPrefix(const Name& prefixToRegister,
                     const SuccessCallback& onSuccess,
                     const FailCallback&    onFail,
                     const Name& identity) = 0;

  virtual void
  selfDeregisterPrefix(const Name& prefixToRegister,
                       const SuccessCallback& onSuccess,
                       const FailCallback&    onFail) = 0;

  virtual void
  selfDeregisterPrefix(const Name& prefixToRegister,
                       const SuccessCallback& onSuccess,
                       const FailCallback&    onFail,
                       const IdentityCertificate& certificate) = 0;

  virtual void
  selfDeregisterPrefix(const Name& prefixToRegister,
                       const SuccessCallback& onSuccess,
                       const FailCallback&    onFail,
                       const Name& identity) = 0;
};

} // namespace ndn

#endif // NDN_MANAGEMENT_CONTROLLER_HPP
