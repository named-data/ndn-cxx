/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#include "signature-sha256-with-rsa.h"
#include "ndn-cpp/wire/base.h"

namespace ndn {
namespace signature {

const std::string Sha256WithRsa::s_oid = "2.16.840.1.101.3.4.2.1";

Sha256WithRsa::~Sha256WithRsa ()
{
}

void
Sha256WithRsa::doubleDispatch (std::ostream &os, wire::Base &wire, void *userData) const
{
  wire.appendSignature (os, *this, userData);
}

} // signature
} // ndn
