/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_DER_EXCEPTION_HPP
#define NDN_DER_EXCEPTION_HPP

#include <exception>
#include <string>

namespace ndn {

namespace der {

class DerException : public std::exception {
public:
  DerException(const std::string& errorMessage) throw();
  
  ~DerException() throw();
  
  std::string Msg() { return errorMessage_; }
  
  virtual const char* what() const throw() { return errorMessage_.c_str(); }

private:
  const std::string errorMessage_;
};

class NegativeLengthException : public DerException
{
public:
  NegativeLengthException(const std::string& errorMessage)
  : DerException(errorMessage)
  {}
};

class DerEncodingException : public DerException
{
public:
  DerEncodingException(const std::string& errorMessage)
  : DerException(errorMessage)
  {}
};

class DerDecodingException : public DerException
{
public:
  DerDecodingException(const std::string& errorMessage)
  : DerException(errorMessage)
  {}
};

} // der

}

#endif
