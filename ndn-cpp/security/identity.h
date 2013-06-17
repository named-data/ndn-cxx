/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_IDENTITY_H
#define NDN_IDENTITY_H

namespace ndn {

/**
 * @brief Class representing NDN identity
 *
 * - name 
 *   - (indexing and lookup)
 *   - /ndn/ucla.edu/alex
 * - contents
 *   - privateKeyName
 *     - unique name of the private key
 *     - private key bits are not exposed anywhere
 *     - /ndn/ucla.edu/alex/privKey/<pubkey.sha256>
 *   - indexed list of certificates
 *     - NDN DATA packets
 *     - easy access data structure of NDN certificate (“parsed DATA packet”)
 *   - link to default certificate
 *   - revocation list
 *     - one or more NDN DATA packets
 *     - name: <identity-name>/revocation-list/<version>(/<seqno>)?
 *       - /ndn/ucla.edu/alex/revocation-list/%FD...01
 *       - initially empty, updated whenever an issued certificate is getting revoked
 *     - revocation always exists
 *
 */
class Identity
{
public:
};

} // ndn

#endif // NDN_IDENTITY_H
