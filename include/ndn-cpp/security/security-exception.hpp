/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SECURITY_EXCEPTION_HPP
#define	NDN_SECURITY_EXCEPTION_HPP

#include <exception>
#include <string>

namespace ndn {

class SecurityException : public std::exception {
public:
  SecurityException(const std::string& errorMessage) throw();
    
  virtual ~SecurityException() throw();
    
  std::string Msg() { return errorMessage_; }

  virtual const char* what() const throw() { return errorMessage_.c_str(); }
    
private:
  const std::string errorMessage_;
};

class UnrecognizedKeyFormatException : public SecurityException {
public:
  UnrecognizedKeyFormatException(const std::string& errorMessage)
  : SecurityException(errorMessage)
  {
  }
};

class UnrecognizedDigestAlgorithmException : public SecurityException {
public:
  UnrecognizedDigestAlgorithmException(const std::string& errorMessage)
  : SecurityException(errorMessage)
  {
  }
};

}

#endif
