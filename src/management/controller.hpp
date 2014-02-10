/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_MANAGEMENT_CONTROLLER_HPP
#define NDN_MANAGEMENT_CONTROLLER_HPP

#include "../common.hpp"
#include "../name.hpp"
#include "../interest.hpp"
#include "../data.hpp"

namespace ndn {

class Node;

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
  selfDeregisterPrefix(const Name& prefixToRegister,
                       const SuccessCallback& onSuccess,
                       const FailCallback&    onFail) = 0;
};

} // namespace ndn

#endif // NDN_MANAGEMENT_CONTROLLER_HPP
