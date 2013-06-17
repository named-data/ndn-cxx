/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *                     Zhenkai Zhu
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Zhenkai Zhu <zhenkai@cs.ucla.edu>
 *         Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

// #include "platforms/osx/keychain-osx.h"
#include "ndn-cpp/error.h"

#include <boost/test/unit_test.hpp>

#include <fstream>

using namespace ndn;
using namespace std;
using namespace boost;

BOOST_AUTO_TEST_SUITE(SecurityTests)

BOOST_AUTO_TEST_CASE (Basic)
{
  // Ptr<Keychain> keychain;
  // BOOST_CHECK_NO_THROW (keychain = Ptr<keychain::OSX>::Create ());

  // Name keyName ("/my/private/key1");
  // keychain->generateKeyPair (keyName);
  // // keychain->deleteKeyPair (keyName);

  // Ptr<Blob> key = keychain->getPublicKey (keyName);
  // ofstream f ("out.pub");
  // f.write (key->buf (), key->size ());
}

BOOST_AUTO_TEST_SUITE_END()
