/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_CERTIFICATE_H
#define NDN_CERTIFICATE_H

#include "ndn-cpp/data.h"

namespace ndn {

/**
 * @brief Class representing NDN identity
 *
 * - name
 *   - full NDN name of the NDN certificate
 *     - /ndn/ucla.edu/alex/cert/<pubkey.sha256>/<issuer>
 * - content
 *   - X.509 certificate in DER format (X.509 certificate can include any necessary identity information, as well as is fully extendable)
 *     - Subject: 
 *       - full real name, associated with the certificate
 *       - full affiliation, associated with the certificate
 *     - Subject Public Key Info
 *     - Validity
 * - signature
 *   - issuerCertName (KeyLocator/CertName)
 *     - /ndn/ucla.edu/cert/<pubkey.sha256>/<issuer>
 *
 */
class Certificate : public Data
{
public:
};

} // ndn

#endif // NDN_CERTIFICATE_H
