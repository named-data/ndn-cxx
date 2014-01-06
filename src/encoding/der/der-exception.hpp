/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_DER_EXCEPTION_HPP
#define NDN_DER_EXCEPTION_HPP

#include <stdexcept>
#include <string>

namespace ndn {

namespace der {

struct DerException : public std::runtime_error { DerException(const std::string &msg) : std::runtime_error(msg) {} };

class NegativeLengthException : public DerException { NegativeLengthException(const std::string &msg) : DerException(msg) {} };

class DerEncodingException : public DerException { DerEncodingException(const std::string &msg) : DerException(msg) {} };

class DerDecodingException : public DerException { DerDecodingException(const std::string &msg) : DerException(msg) {} };

} // der

}

#endif
