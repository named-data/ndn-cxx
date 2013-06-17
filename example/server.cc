/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#include <ndn.cxx.h>
#include <iostream>

using namespace std;

ndn::Name InterestBaseName;

// create a global handler
ndn::Wrapper handler;

void OnInterest (ndn::InterestPtr interest)
{
  cerr << interest->getName () << endl;
  
  static int COUNTER = 0;

  ostringstream os;
  os << "C++ LINE #" << (COUNTER++) << endl;
  
  handler.publishData (interest->getName (), os.str (), 5);
}

int
main (int argc, char **argv)
{
  InterestBaseName = ndn::Name ("ccnx:/my-local-prefix/simple-fetch/file");

  handler.setInterestFilter (InterestBaseName, OnInterest);
  
  while (true)
    {
      sleep (1);
    }
  return 0;
}
