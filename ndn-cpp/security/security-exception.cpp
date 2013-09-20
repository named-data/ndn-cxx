/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "security-exception.hpp"
using namespace std;

namespace ndn {

SecurityException::SecurityException(const string& errorMessage) throw()
: errorMessage_(errorMessage)
{
}

SecurityException::~SecurityException() throw()
{
}

}
