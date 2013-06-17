/* -*- Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013 University of California, Los Angeles
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Zhenkai Zhu <zhenkai@cs.ucla.edu>
 *         Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_VERIFIER_H
#define NDN_VERIFIER_H

#include "ndn.cxx/common.h"
#include "ndn.cxx/name.h"
#include "ndn.cxx/cert.h"
#include "ndn.cxx/pco.h"
#include <map>
#include <boost/thread/locks.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/thread.hpp>

namespace ndn {

class Wrapper;

class Verifier
{
public:
  Verifier(Wrapper *ccnx);
  ~Verifier();

  bool verify(PcoPtr pco, double maxWait);

private:

private:
  Wrapper *m_ccnx;
  Hash m_rootKeyDigest;
  typedef std::map<Hash, CertPtr> CertCache;
  CertCache m_certCache;
  typedef boost::recursive_mutex RecLock;
  typedef boost::unique_lock<RecLock> UniqueRecLock;
  RecLock m_cacheLock;
};

} // ndn

#endif // NDN_VERIFIER_H
