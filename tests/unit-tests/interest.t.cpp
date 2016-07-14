/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2016 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 */

#include "interest.hpp"
#include "interest-filter.hpp"
#include "data.hpp"
#include "security/signature-sha256-with-rsa.hpp"
#include "security/digest-sha256.hpp"
#include "security/key-chain.hpp"
#include "encoding/buffer-stream.hpp"

#include "boost-test.hpp"
#include "identity-management-fixture.hpp"

namespace ndn {
namespace tests {

BOOST_FIXTURE_TEST_SUITE(TestInterest, IdentityManagementFixture)

const uint8_t Interest1[] = {
  0x05,  0x59, // NDN Interest
      0x07,  0x14, // Name
          0x08,  0x5, // NameComponent
              0x6c,  0x6f,  0x63,  0x61,  0x6c,
          0x08,  0x3, // NameComponent
              0x6e,  0x64,  0x6e,
          0x08,  0x6, // NameComponent
              0x70,  0x72,  0x65,  0x66,  0x69,  0x78,
      0x09,  0x37, // Selectors
          0x0d,  0x1,  0x1,  // MinSuffix
          0x0e,  0x1,  0x1,  // MaxSuffix
          0x1c, 0x16, // KeyLocator
              0x07, 0x14, // Name
                  0x08, 0x04,
                      0x74, 0x65, 0x73, 0x74,
                  0x08, 0x03,
                      0x6b, 0x65, 0x79,
                  0x08, 0x07,
                      0x6c, 0x6f, 0x63, 0x61, 0x74, 0x6f, 0x72,
          0x10,  0x14, // Exclude
              0x08,  0x4, // NameComponent
                  0x61,  0x6c,  0x65,  0x78,
              0x08,  0x4, // NameComponent
                  0x78,  0x78,  0x78,  0x78,
              0x13,  0x0, // Any
              0x08,  0x4, // NameComponent
                  0x79,  0x79,  0x79,  0x79,
          0x11,  0x1, // ChildSelector
              0x1,
      0x0a,  0x4, // Nonce
          0x1, 0x0, 0x0, 0x00,
      0x0c,       // InterestLifetime
          0x2,  0x3,  0xe8
};

const uint8_t Interest2[] = {
  0x05,  0x59, // NDN Interest
      0x07,  0x14, // Name
          0x08,  0x5, // NameComponent
              0x6c,  0x6f,  0x63,  0x61,  0x6c,
          0x08,  0x3, // NameComponent
              0x6e,  0x64,  0x6e,
          0x08,  0x6, // NameComponent
              0x70,  0x72,  0x65,  0x66,  0x69,  0x78,
      0x09,  0x37, // Selectors
          0x0d,  0x1,  0x1,  // MinSuffix
          0x0e,  0x1,  0x1,  // MaxSuffix
          0x1c, 0x16, // KeyLocator
              0x07, 0x14, // Name
                  0x08, 0x04,
                      0x74, 0x65, 0x73, 0x74,
                  0x08, 0x03,
                      0x6b, 0x65, 0x79,
                  0x08, 0x07,
                      0x6c, 0x6f, 0x63, 0x61, 0x74, 0x6f, 0x72,
          0x10,  0x14, // Exclude
              0x08,  0x4, // NameComponent
                  0x61,  0x6c,  0x65,  0x78,
              0x08,  0x4, // NameComponent
                  0x78,  0x78,  0x78,  0x78,
              0x13,  0x0, // Any
              0x08,  0x4, // NameComponent
                  0x79,  0x79,  0x79,  0x79,
          0x11,  0x1, // ChildSelector
              0x1,
      0x0a,  0x4, // Nonce
          0x2, 0x0, 0x0, 0x00,
      0x0c,       // InterestLifetime
          0x2,  0x3,  0xe8
};

const uint8_t InterestWithLink[] = {
  0x05,  0xfb, // Interest
      0x07,  0x14, // Name
          0x08,  0x5, // NameComponent
              0x6c,  0x6f,  0x63,  0x61,  0x6c,
          0x08,  0x3, // NameComponent
              0x6e,  0x64,  0x6e,
          0x08,  0x6, // NameComponent
              0x70,  0x72,  0x65,  0x66,  0x69,  0x78,
      0x0a,  0x4, // Nonce
          0x1, 0x0, 0x0, 0x00,
      0x06, 0xda, // Data
          0x07, 0x14, // Name
              0x08, 0x05,
                  0x6c, 0x6f, 0x63, 0x61, 0x6c,
              0x08, 0x03,
                  0x6e, 0x64, 0x6e,
              0x08, 0x06,
                  0x70, 0x72, 0x65, 0x66, 0x69, 0x78,
          0x14, 0x07, // MetaInfo
              0x18, 0x01, // ContentType
                  0x01,
              0x19, 0x02, // FreshnessPeriod
                  0x27, 0x10,
          0x15, 0x1a, // Content
              0x1f, 0x0c, // LinkDelegation
                  0x1e, 0x01, // LinkPreference
                      0x0a,
                  0x07, 0x07, // Name
                      0x08, 0x05,
                          0x6c, 0x6f, 0x63, 0x61, 0x6c,
              0x1f, 0x0a, // LinkDelegation
                  0x1e, 0x01, // LinkPreference
                      0x14,
                  0x07, 0x05, // Name
                      0x08, 0x03,
                          0x6e, 0x64, 0x6e,
           0x16, 0x1b, // SignatureInfo
               0x1b, 0x01, // SignatureType
                   0x01,
           0x1c, 0x16, // KeyLocator
               0x07, 0x14, // Name
                   0x08, 0x04,
                       0x74, 0x65, 0x73, 0x74,
                   0x08, 0x03,
                       0x6b, 0x65, 0x79,
                   0x08, 0x07,
                       0x6c, 0x6f, 0x63, 0x61, 0x74, 0x6f, 0x72,
           0x17, 0x80, // SignatureValue
               0x2f, 0xd6, 0xf1, 0x6e, 0x80, 0x6f, 0x10, 0xbe, 0xb1, 0x6f, 0x3e, 0x31, 0xec,
               0xe3, 0xb9, 0xea, 0x83, 0x30, 0x40, 0x03, 0xfc, 0xa0, 0x13, 0xd9, 0xb3, 0xc6,
               0x25, 0x16, 0x2d, 0xa6, 0x58, 0x41, 0x69, 0x62, 0x56, 0xd8, 0xb3, 0x6a, 0x38,
               0x76, 0x56, 0xea, 0x61, 0xb2, 0x32, 0x70, 0x1c, 0xb6, 0x4d, 0x10, 0x1d, 0xdc,
               0x92, 0x8e, 0x52, 0xa5, 0x8a, 0x1d, 0xd9, 0x96, 0x5e, 0xc0, 0x62, 0x0b, 0xcf,
               0x3a, 0x9d, 0x7f, 0xca, 0xbe, 0xa1, 0x41, 0x71, 0x85, 0x7a, 0x8b, 0x5d, 0xa9,
               0x64, 0xd6, 0x66, 0xb4, 0xe9, 0x8d, 0x0c, 0x28, 0x43, 0xee, 0xa6, 0x64, 0xe8,
               0x55, 0xf6, 0x1c, 0x19, 0x0b, 0xef, 0x99, 0x25, 0x1e, 0xdc, 0x78, 0xb3, 0xa7,
               0xaa, 0x0d, 0x14, 0x58, 0x30, 0xe5, 0x37, 0x6a, 0x6d, 0xdb, 0x56, 0xac, 0xa3,
               0xfc, 0x90, 0x7a, 0xb8, 0x66, 0x9c, 0x0e, 0xf6, 0xb7, 0x64, 0xd1,
      0x20, 0x01, // SelectedDelegation
          0x00
};

const uint8_t LINK[] = {
  0x06, 0xda, // Data
      0x07, 0x14, // Name
          0x08, 0x05,
              0x6c, 0x6f, 0x63, 0x61, 0x6c,
          0x08, 0x03,
              0x6e, 0x64, 0x6e,
          0x08, 0x06,
              0x70, 0x72, 0x65, 0x66, 0x69, 0x78,
      0x14, 0x07, // MetaInfo
          0x18, 0x01, // ContentType
              0x01,
          0x19, 0x02, // FreshnessPeriod
              0x27, 0x10,
      0x15, 0x1a, // Content
          0x1f, 0x0c, // LinkDelegation
              0x1e, 0x01, // LinkPreference
                  0x0a,
              0x07, 0x07, // Name
                  0x08, 0x05,
                      0x6c, 0x6f, 0x63, 0x61, 0x6c,
          0x1f, 0x0a, // LinkDelegation
              0x1e, 0x01, // LinkPreference
                  0x14,
              0x07, 0x05, // Name
                  0x08, 0x03,
                      0x6e, 0x64, 0x6e,
       0x16, 0x1b, // SignatureInfo
           0x1b, 0x01, // SignatureType
               0x01,
       0x1c, 0x16, // KeyLocator
           0x07, 0x14, // Name
               0x08, 0x04,
                   0x74, 0x65, 0x73, 0x74,
               0x08, 0x03,
                   0x6b, 0x65, 0x79,
               0x08, 0x07,
                   0x6c, 0x6f, 0x63, 0x61, 0x74, 0x6f, 0x72,
       0x17, 0x80, // SignatureValue
           0x2f, 0xd6, 0xf1, 0x6e, 0x80, 0x6f, 0x10, 0xbe, 0xb1, 0x6f, 0x3e, 0x31, 0xec,
           0xe3, 0xb9, 0xea, 0x83, 0x30, 0x40, 0x03, 0xfc, 0xa0, 0x13, 0xd9, 0xb3, 0xc6,
           0x25, 0x16, 0x2d, 0xa6, 0x58, 0x41, 0x69, 0x62, 0x56, 0xd8, 0xb3, 0x6a, 0x38,
           0x76, 0x56, 0xea, 0x61, 0xb2, 0x32, 0x70, 0x1c, 0xb6, 0x4d, 0x10, 0x1d, 0xdc,
           0x92, 0x8e, 0x52, 0xa5, 0x8a, 0x1d, 0xd9, 0x96, 0x5e, 0xc0, 0x62, 0x0b, 0xcf,
           0x3a, 0x9d, 0x7f, 0xca, 0xbe, 0xa1, 0x41, 0x71, 0x85, 0x7a, 0x8b, 0x5d, 0xa9,
           0x64, 0xd6, 0x66, 0xb4, 0xe9, 0x8d, 0x0c, 0x28, 0x43, 0xee, 0xa6, 0x64, 0xe8,
           0x55, 0xf6, 0x1c, 0x19, 0x0b, 0xef, 0x99, 0x25, 0x1e, 0xdc, 0x78, 0xb3, 0xa7,
           0xaa, 0x0d, 0x14, 0x58, 0x30, 0xe5, 0x37, 0x6a, 0x6d, 0xdb, 0x56, 0xac, 0xa3,
           0xfc, 0x90, 0x7a, 0xb8, 0x66, 0x9c, 0x0e, 0xf6, 0xb7, 0x64, 0xd1
};

const uint8_t InterestWithLinkMissingContentType[] = {
  0x05,  0xf8, // Interest
      0x07,  0x14, // Name
          0x08,  0x5, // NameComponent
              0x6c,  0x6f,  0x63,  0x61,  0x6c,
          0x08,  0x3, // NameComponent
              0x6e,  0x64,  0x6e,
          0x08,  0x6, // NameComponent
              0x70,  0x72,  0x65,  0x66,  0x69,  0x78,
      0x0a,  0x4, // Nonce
          0x1, 0x0, 0x0, 0x00,
      0x06, 0xd7, // Data
          0x07, 0x14, // Name
              0x08, 0x05,
                  0x6c, 0x6f, 0x63, 0x61, 0x6c,
              0x08, 0x03,
                  0x6e, 0x64, 0x6e,
              0x08, 0x06,
                  0x70, 0x72, 0x65, 0x66, 0x69, 0x78,
          0x14, 0x04, // MetaInfo
              0x19, 0x02, // FreshnessPeriod
                  0x27, 0x10,
          0x15, 0x1a, // Content
              0x1f, 0x0c, // LinkDelegation
                  0x1e, 0x01, // LinkPreference
                      0x0a,
                  0x07, 0x07, // Name
                      0x08, 0x05,
                          0x6c, 0x6f, 0x63, 0x61, 0x6c,
              0x1f, 0x0a, // LinkDelegation
                  0x1e, 0x01, // LinkPreference
                      0x14,
                  0x07, 0x05, // Name
                      0x08, 0x03,
                          0x6e, 0x64, 0x6e,
           0x16, 0x1b, // SignatureInfo
               0x1b, 0x01, // SignatureType
                   0x01,
           0x1c, 0x16, // KeyLocator
               0x07, 0x14, // Name
                   0x08, 0x04,
                       0x74, 0x65, 0x73, 0x74,
                   0x08, 0x03,
                       0x6b, 0x65, 0x79,
                   0x08, 0x07,
                       0x6c, 0x6f, 0x63, 0x61, 0x74, 0x6f, 0x72,
           0x17, 0x80, // SignatureValue
               0x2f, 0xd6, 0xf1, 0x6e, 0x80, 0x6f, 0x10, 0xbe, 0xb1, 0x6f, 0x3e, 0x31, 0xec,
               0xe3, 0xb9, 0xea, 0x83, 0x30, 0x40, 0x03, 0xfc, 0xa0, 0x13, 0xd9, 0xb3, 0xc6,
               0x25, 0x16, 0x2d, 0xa6, 0x58, 0x41, 0x69, 0x62, 0x56, 0xd8, 0xb3, 0x6a, 0x38,
               0x76, 0x56, 0xea, 0x61, 0xb2, 0x32, 0x70, 0x1c, 0xb6, 0x4d, 0x10, 0x1d, 0xdc,
               0x92, 0x8e, 0x52, 0xa5, 0x8a, 0x1d, 0xd9, 0x96, 0x5e, 0xc0, 0x62, 0x0b, 0xcf,
               0x3a, 0x9d, 0x7f, 0xca, 0xbe, 0xa1, 0x41, 0x71, 0x85, 0x7a, 0x8b, 0x5d, 0xa9,
               0x64, 0xd6, 0x66, 0xb4, 0xe9, 0x8d, 0x0c, 0x28, 0x43, 0xee, 0xa6, 0x64, 0xe8,
               0x55, 0xf6, 0x1c, 0x19, 0x0b, 0xef, 0x99, 0x25, 0x1e, 0xdc, 0x78, 0xb3, 0xa7,
               0xaa, 0x0d, 0x14, 0x58, 0x30, 0xe5, 0x37, 0x6a, 0x6d, 0xdb, 0x56, 0xac, 0xa3,
               0xfc, 0x90, 0x7a, 0xb8, 0x66, 0x9c, 0x0e, 0xf6, 0xb7, 0x64, 0xd1,
      0x20, 0x01, // SelectedDelegation
          0x00
};

const uint8_t InterestWithLinkNoMetaInfo[] = {
  0x05,  0xf2, // Interest
      0x07,  0x14, // Name
          0x08,  0x5, // NameComponent
              0x6c,  0x6f,  0x63,  0x61,  0x6c,
          0x08,  0x3, // NameComponent
              0x6e,  0x64,  0x6e,
          0x08,  0x6, // NameComponent
              0x70,  0x72,  0x65,  0x66,  0x69,  0x78,
      0x0a,  0x4, // Nonce
          0x1, 0x0, 0x0, 0x00,
      0x06, 0xd1, // Data
          0x07, 0x14, // Name
              0x08, 0x05,
                  0x6c, 0x6f, 0x63, 0x61, 0x6c,
              0x08, 0x03,
                  0x6e, 0x64, 0x6e,
              0x08, 0x06,
                  0x70, 0x72, 0x65, 0x66, 0x69, 0x78,
          0x15, 0x1a, // Content
              0x1f, 0x0c, // LinkDelegation
                  0x1e, 0x01, // LinkPreference
                      0x0a,
                  0x07, 0x07, // Name
                      0x08, 0x05,
                          0x6c, 0x6f, 0x63, 0x61, 0x6c,
              0x1f, 0x0a, // LinkPreference
                  0x1e, 0x01, // LinkPreference
                      0x14,
                  0x07, 0x05, // Name
                      0x08, 0x03,
                          0x6e, 0x64, 0x6e,
           0x16, 0x1b, // SignatureInfo
               0x1b, 0x01, // SignatureType
                   0x01,
           0x1c, 0x16, // KeyLocator
               0x07, 0x14, // Name
                   0x08, 0x04,
                       0x74, 0x65, 0x73, 0x74,
                   0x08, 0x03,
                       0x6b, 0x65, 0x79,
                   0x08, 0x07,
                       0x6c, 0x6f, 0x63, 0x61, 0x74, 0x6f, 0x72,
           0x17, 0x80, // SignatureValue
               0x2f, 0xd6, 0xf1, 0x6e, 0x80, 0x6f, 0x10, 0xbe, 0xb1, 0x6f, 0x3e, 0x31, 0xec,
               0xe3, 0xb9, 0xea, 0x83, 0x30, 0x40, 0x03, 0xfc, 0xa0, 0x13, 0xd9, 0xb3, 0xc6,
               0x25, 0x16, 0x2d, 0xa6, 0x58, 0x41, 0x69, 0x62, 0x56, 0xd8, 0xb3, 0x6a, 0x38,
               0x76, 0x56, 0xea, 0x61, 0xb2, 0x32, 0x70, 0x1c, 0xb6, 0x4d, 0x10, 0x1d, 0xdc,
               0x92, 0x8e, 0x52, 0xa5, 0x8a, 0x1d, 0xd9, 0x96, 0x5e, 0xc0, 0x62, 0x0b, 0xcf,
               0x3a, 0x9d, 0x7f, 0xca, 0xbe, 0xa1, 0x41, 0x71, 0x85, 0x7a, 0x8b, 0x5d, 0xa9,
               0x64, 0xd6, 0x66, 0xb4, 0xe9, 0x8d, 0x0c, 0x28, 0x43, 0xee, 0xa6, 0x64, 0xe8,
               0x55, 0xf6, 0x1c, 0x19, 0x0b, 0xef, 0x99, 0x25, 0x1e, 0xdc, 0x78, 0xb3, 0xa7,
               0xaa, 0x0d, 0x14, 0x58, 0x30, 0xe5, 0x37, 0x6a, 0x6d, 0xdb, 0x56, 0xac, 0xa3,
               0xfc, 0x90, 0x7a, 0xb8, 0x66, 0x9c, 0x0e, 0xf6, 0xb7, 0x64, 0xd1,
      0x20, 0x01, // SelectedDelegation
          0x00
};

const uint8_t InterestWithLinkWrongContentType[] = {
  0x05,  0xfb, // Interest
      0x07,  0x14, // Name
          0x08,  0x5, // NameComponent
              0x6c,  0x6f,  0x63,  0x61,  0x6c,
          0x08,  0x3, // NameComponent
              0x6e,  0x64,  0x6e,
          0x08,  0x6, // NameComponent
              0x70,  0x72,  0x65,  0x66,  0x69,  0x78,
      0x0a,  0x4, // Nonce
          0x1, 0x0, 0x0, 0x00,
      0x06, 0xda, // Data
          0x07, 0x14, // Name
              0x08, 0x05,
                  0x6c, 0x6f, 0x63, 0x61, 0x6c,
              0x08, 0x03,
                  0x6e, 0x64, 0x6e,
              0x08, 0x06,
                  0x70, 0x72, 0x65, 0x66, 0x69, 0x78,
          0x14, 0x07, // MetaInfo
              0x18, 0x01, // ContentType
                  0x00,
              0x19, 0x02, // FreshnessPeriod
                  0x27, 0x10,
          0x15, 0x1a, // Content
              0x1f, 0x0c, // LinkDelegation
                  0x1e, 0x01, // LinkPreference
                      0x0a,
                  0x07, 0x07, // Name
                      0x08, 0x05,
                          0x6c, 0x6f, 0x63, 0x61, 0x6c,
              0x1f, 0x0a, // LinkDelegation
                  0x1e, 0x01, // LinkPreference
                      0x14,
                  0x07, 0x05, // Name
                      0x08, 0x03,
                          0x6e, 0x64, 0x6e,
           0x16, 0x1b, // SignatureInfo
               0x1b, 0x01, // SignatureType
                   0x01,
           0x1c, 0x16, // KeyLocator
               0x07, 0x14, // Name
                   0x08, 0x04,
                       0x74, 0x65, 0x73, 0x74,
                   0x08, 0x03,
                       0x6b, 0x65, 0x79,
                   0x08, 0x07,
                       0x6c, 0x6f, 0x63, 0x61, 0x74, 0x6f, 0x72,
           0x17, 0x80, // SignatureValue
               0x2f, 0xd6, 0xf1, 0x6e, 0x80, 0x6f, 0x10, 0xbe, 0xb1, 0x6f, 0x3e, 0x31, 0xec,
               0xe3, 0xb9, 0xea, 0x83, 0x30, 0x40, 0x03, 0xfc, 0xa0, 0x13, 0xd9, 0xb3, 0xc6,
               0x25, 0x16, 0x2d, 0xa6, 0x58, 0x41, 0x69, 0x62, 0x56, 0xd8, 0xb3, 0x6a, 0x38,
               0x76, 0x56, 0xea, 0x61, 0xb2, 0x32, 0x70, 0x1c, 0xb6, 0x4d, 0x10, 0x1d, 0xdc,
               0x92, 0x8e, 0x52, 0xa5, 0x8a, 0x1d, 0xd9, 0x96, 0x5e, 0xc0, 0x62, 0x0b, 0xcf,
               0x3a, 0x9d, 0x7f, 0xca, 0xbe, 0xa1, 0x41, 0x71, 0x85, 0x7a, 0x8b, 0x5d, 0xa9,
               0x64, 0xd6, 0x66, 0xb4, 0xe9, 0x8d, 0x0c, 0x28, 0x43, 0xee, 0xa6, 0x64, 0xe8,
               0x55, 0xf6, 0x1c, 0x19, 0x0b, 0xef, 0x99, 0x25, 0x1e, 0xdc, 0x78, 0xb3, 0xa7,
               0xaa, 0x0d, 0x14, 0x58, 0x30, 0xe5, 0x37, 0x6a, 0x6d, 0xdb, 0x56, 0xac, 0xa3,
               0xfc, 0x90, 0x7a, 0xb8, 0x66, 0x9c, 0x0e, 0xf6, 0xb7, 0x64, 0xd1,
      0x20, 0x01, // SelectedDelegation
          0x00
};

const uint8_t InterestWithSelectedDelegationButNoLink[] = {
  0x05,  0x1f, // Interest
      0x07,  0x14, // Name
          0x08,  0x5, // NameComponent
              0x6c,  0x6f,  0x63,  0x61,  0x6c,
          0x08,  0x3, // NameComponent
              0x6e,  0x64,  0x6e,
          0x08,  0x6, // NameComponent
              0x70,  0x72,  0x65,  0x66,  0x69,  0x78,
      0x0a,  0x4, // Nonce
          0x1, 0x0, 0x0, 0x00,
      0x20, 0x01, // SelectedDelegation
          0x00
};

const uint8_t InterestWithLinkNotNonIntegerSelectedDelegation[] = {
  0x05,  0xfb, // Interest
      0x07,  0x14, // Name
          0x08,  0x5, // NameComponent
              0x6c,  0x6f,  0x63,  0x61,  0x6c,
          0x08,  0x3, // NameComponent
              0x6e,  0x64,  0x6e,
          0x08,  0x6, // NameComponent
              0x70,  0x72,  0x65,  0x66,  0x69,  0x78,
      0x0a,  0x4, // Nonce
          0x1, 0x0, 0x0, 0x00,
      0x06, 0xda, // Data
          0x07, 0x14, // Name
              0x08, 0x05,
                  0x6c, 0x6f, 0x63, 0x61, 0x6c,
              0x08, 0x03,
                  0x6e, 0x64, 0x6e,
              0x08, 0x06,
                  0x70, 0x72, 0x65, 0x66, 0x69, 0x78,
          0x14, 0x07, // MetaInfo
              0x18, 0x01, // ContentType
                  0x01,
              0x19, 0x02, // FreshnessPeriod
                  0x27, 0x10,
          0x15, 0x1a, // Content
              0x1f, 0x0c, // LinkDelegation
                  0x1e, 0x01, // LinkPreference
                      0x0a,
                  0x07, 0x07, // Name
                      0x08, 0x05,
                          0x6c, 0x6f, 0x63, 0x61, 0x6c,
              0x1f, 0x0a, // LinkDelegation
                  0x1e, 0x01, // LinkPreference
                      0x14,
                  0x07, 0x05, // Name
                      0x08, 0x03,
                          0x6e, 0x64, 0x6e,
           0x16, 0x1b, // SignatureInfo
               0x1b, 0x01, // SignatureType
                   0x01,
           0x1c, 0x16, // KeyLocator
               0x07, 0x14, // Name
                   0x08, 0x04,
                       0x74, 0x65, 0x73, 0x74,
                   0x08, 0x03,
                       0x6b, 0x65, 0x79,
                   0x08, 0x07,
                       0x6c, 0x6f, 0x63, 0x61, 0x74, 0x6f, 0x72,
           0x17, 0x78, // SignatureValue
               0x2f, 0xd6, 0xf1, 0x6e, 0x80, 0x6f, 0x10, 0xbe, 0xb1, 0x6f, 0x3e, 0x31, 0xec,
               0xe3, 0xb9, 0xea, 0x83, 0x30, 0x40, 0x03, 0xfc, 0xa0, 0x13, 0xd9, 0xb3, 0xc6,
               0x25, 0x16, 0x2d, 0xa6, 0x58, 0x41, 0x69, 0x62, 0x56, 0xd8, 0xb3, 0x6a, 0x38,
               0x76, 0x56, 0xea, 0x61, 0xb2, 0x32, 0x70, 0x1c, 0xb6, 0x4d, 0x10, 0x1d, 0xdc,
               0x92, 0x8e, 0x52, 0xa5, 0x8a, 0x1d, 0xd9, 0x96, 0x5e, 0xc0, 0x62, 0x0b, 0xcf,
               0x3a, 0x9d, 0x7f, 0xca, 0xbe, 0xa1, 0x41, 0x71, 0x85, 0x7a, 0x8b, 0x5d, 0xa9,
               0x64, 0xd6, 0x66, 0xb4, 0xe9, 0x8d, 0x0c, 0x28, 0x43, 0xee, 0xa6, 0x64, 0xe8,
               0x55, 0xf6, 0x1c, 0x19, 0x0b, 0xef, 0x99, 0x25, 0x1e, 0xdc, 0x78, 0xb3, 0xa7,
               0xaa, 0x0d, 0x14, 0x58, 0x30, 0xe5, 0x37, 0x6a, 0x6d, 0xdb, 0x56, 0xac, 0xa3,
               0xfc, 0x90, 0x7a, 0xb8, 0x66, 0x9c, 0x0e, 0xf6, 0xb7,
      0x20, 0x03, // SelectedDelegation
          0xAA, 0xAA, 0xAA
};

const uint8_t InterestWithLinkNonDecreasingOrder[] = {
  0x05,  0xfb, // Interest
      0x07,  0x14, // Name
          0x08,  0x5, // NameComponent
              0x6c,  0x6f,  0x63,  0x61,  0x6c,
          0x08,  0x3, // NameComponent
              0x6e,  0x64,  0x6e,
          0x08,  0x6, // NameComponent
              0x70,  0x72,  0x65,  0x66,  0x69,  0x78,
      0x0a,  0x4, // Nonce
          0x1, 0x0, 0x0, 0x00,
      0x06, 0xda, // Data
          0x07, 0x14, // Name
              0x08, 0x05,
                  0x6c, 0x6f, 0x63, 0x61, 0x6c,
              0x08, 0x03,
                  0x6e, 0x64, 0x6e,
              0x08, 0x06,
                  0x70, 0x72, 0x65, 0x66, 0x69, 0x78,
          0x14, 0x07, // MetaInfo
              0x18, 0x01, // ContentType
                  0x01,
              0x19, 0x02, // FreshnessPeriod
                  0x27, 0x10,
          0x15, 0x1a, // Content
              0x1f, 0x0c, // LinkDelegation
                  0x1e, 0x01, // LinkPreference
                      0x14,
                  0x07, 0x07, // Name
                      0x08, 0x05,
                          0x6c, 0x6f, 0x63, 0x61, 0x6c,
              0x1f, 0x0a, // LinkDelegation
                  0x1e, 0x01, // LinkPreference
                      0x0a,
                  0x07, 0x05, // Name
                      0x08, 0x03,
                          0x6e, 0x64, 0x6e,
           0x16, 0x1b, // SignatureInfo
               0x1b, 0x01, // SignatureType
                   0x01,
           0x1c, 0x16, // KeyLocator
               0x07, 0x14, // Name
                   0x08, 0x04,
                       0x74, 0x65, 0x73, 0x74,
                   0x08, 0x03,
                       0x6b, 0x65, 0x79,
                   0x08, 0x07,
                       0x6c, 0x6f, 0x63, 0x61, 0x74, 0x6f, 0x72,
           0x17, 0x80, // SignatureValue
               0x2f, 0xd6, 0xf1, 0x6e, 0x80, 0x6f, 0x10, 0xbe, 0xb1, 0x6f, 0x3e, 0x31, 0xec,
               0xe3, 0xb9, 0xea, 0x83, 0x30, 0x40, 0x03, 0xfc, 0xa0, 0x13, 0xd9, 0xb3, 0xc6,
               0x25, 0x16, 0x2d, 0xa6, 0x58, 0x41, 0x69, 0x62, 0x56, 0xd8, 0xb3, 0x6a, 0x38,
               0x76, 0x56, 0xea, 0x61, 0xb2, 0x32, 0x70, 0x1c, 0xb6, 0x4d, 0x10, 0x1d, 0xdc,
               0x92, 0x8e, 0x52, 0xa5, 0x8a, 0x1d, 0xd9, 0x96, 0x5e, 0xc0, 0x62, 0x0b, 0xcf,
               0x3a, 0x9d, 0x7f, 0xca, 0xbe, 0xa1, 0x41, 0x71, 0x85, 0x7a, 0x8b, 0x5d, 0xa9,
               0x64, 0xd6, 0x66, 0xb4, 0xe9, 0x8d, 0x0c, 0x28, 0x43, 0xee, 0xa6, 0x64, 0xe8,
               0x55, 0xf6, 0x1c, 0x19, 0x0b, 0xef, 0x99, 0x25, 0x1e, 0xdc, 0x78, 0xb3, 0xa7,
               0xaa, 0x0d, 0x14, 0x58, 0x30, 0xe5, 0x37, 0x6a, 0x6d, 0xdb, 0x56, 0xac, 0xa3,
               0xfc, 0x90, 0x7a, 0xb8, 0x66, 0x9c, 0x0e, 0xf6, 0xb7, 0x64, 0xd1,
      0x20, 0x01, // SelectedDelegation
          0x01
};

BOOST_AUTO_TEST_CASE(InterestEqualityChecks)
{
  // Interest ::= INTEREST-TYPE TLV-LENGTH
  //                Name
  //                Selectors?
  //                Nonce
  //                InterestLifetime?
  //                Link?
  //                SelectedDelegation?

  Interest a;
  Interest b;

  // if nonce is not set, it will be set to a random value
  a.setNonce(1);
  b.setNonce(1);

  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // check comparison on Name
  a.setName("ndn:/A");
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setName("ndn:/B");
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setName("ndn:/A");
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // check comparison on Selectors
  a.setChildSelector(1);
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setChildSelector(1);
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // check comparison on Nonce
  a.setNonce(100);
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setNonce(100);
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // check comparison on InterestLifetime
  a.setInterestLifetime(time::seconds(10));
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setInterestLifetime(time::seconds(10));
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  a.setLink(Block(LINK, sizeof(LINK)));
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setLink(Block(LINK, sizeof(LINK)));
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // Selected Delegation
  BOOST_CHECK_EQUAL(a.hasSelectedDelegation(), false);
  BOOST_CHECK_EQUAL(b.hasSelectedDelegation(), false);

  a.setSelectedDelegation(Name("/local"));
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setSelectedDelegation(Name("/local"));
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);
}

BOOST_AUTO_TEST_CASE(SelectorsEqualityChecks)
{
  // Selectors ::= SELECTORS-TYPE TLV-LENGTH
  //                 MinSuffixComponents?
  //                 MaxSuffixComponents?
  //                 PublisherPublicKeyLocator?
  //                 Exclude?
  //                 ChildSelector?
  //                 MustBeFresh?

  Selectors a;
  Selectors b;
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // MinSuffixComponents
  a.setMinSuffixComponents(1);
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setMinSuffixComponents(2);
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setMinSuffixComponents(1);
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // MaxSuffixComponents
  a.setMaxSuffixComponents(10);
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setMaxSuffixComponents(10);
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // PublisherPublicKeyLocator
  a.setPublisherPublicKeyLocator(KeyLocator("/key/Locator/name"));
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setPublisherPublicKeyLocator(KeyLocator("/key/Locator/name"));
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // Exclude
  a.setExclude(Exclude().excludeOne(name::Component("exclude")));
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setExclude(Exclude().excludeOne(name::Component("exclude")));
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // ChildSelector
  a.setChildSelector(1);
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setChildSelector(1);
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  // MustBeFresh
  a.setMustBeFresh(true);
  BOOST_CHECK_EQUAL(a == b, false);
  BOOST_CHECK_EQUAL(a != b, true);

  b.setMustBeFresh(true);
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);
}

BOOST_AUTO_TEST_CASE(LinkObject)
{
  Link link1("test", {{100, "/test3"}, {20, "/test2"}, {10, "/test1"}});
  m_keyChain.sign(link1);
  Block wire = link1.wireEncode();

  Interest a;
  BOOST_REQUIRE_NO_THROW(a.setLink(wire));

  BOOST_REQUIRE_NO_THROW(a.getLink());

  Link link2 = a.getLink();
  Name name = link2.getName();
  BOOST_CHECK_EQUAL(Name("test"), name);
  BOOST_CHECK_EQUAL(a.hasLink(), true);
  Link::DelegationSet delegations;
  delegations = link2.getDelegations();

  auto i = delegations.begin();
  BOOST_CHECK_EQUAL(std::get<0>(*i), 10);
  BOOST_CHECK_EQUAL(std::get<1>(*i), Name("test1"));
  ++i;
  BOOST_CHECK_EQUAL(std::get<0>(*i), 20);
  BOOST_CHECK_EQUAL(std::get<1>(*i), Name("test2"));
  ++i;
  BOOST_CHECK_EQUAL(std::get<0>(*i), 100);
  BOOST_CHECK_EQUAL(std::get<1>(*i), Name("test3"));

  a.setLink(Block(LINK, sizeof(LINK)));
  BOOST_CHECK_EQUAL(a.getLink().getDelegations().size(), 2);

  a.unsetLink();
  BOOST_CHECK_EQUAL(a.hasLink(), false);
}

BOOST_AUTO_TEST_CASE(SelectedDelegationChecks)
{
  Link link("test", {{10, "/test1"}, {20, "/test2"}, {100, "/test3"}});
  m_keyChain.sign(link);
  Block wire = link.wireEncode();

  Interest a;
  a.setLink(wire);
  BOOST_CHECK_EQUAL(a.hasSelectedDelegation(), false);

  BOOST_REQUIRE_NO_THROW(a.setSelectedDelegation(Name("test2")));
  BOOST_CHECK_EQUAL(a.getSelectedDelegation(), Name("test2"));

  BOOST_REQUIRE_NO_THROW(a.setSelectedDelegation(uint32_t(2)));
  BOOST_CHECK_EQUAL(a.getSelectedDelegation(), Name("test3"));

  a.unsetSelectedDelegation();
  BOOST_CHECK_EQUAL(a.hasSelectedDelegation(), false);
}

BOOST_AUTO_TEST_CASE(EncodeDecodeWithLink)
{
  Link link1("test", {{10, "/test1"}, {20, "/test2"}, {100, "/test3"}});
  m_keyChain.sign(link1);
  Block wire = link1.wireEncode();

  Interest a;
  a.setName("/Test/Encode/Decode/With/Link");
  a.setChildSelector(1);
  a.setNonce(100);
  a.setInterestLifetime(time::seconds(10));
  a.setLink(wire);

  Block interestBlock = a.wireEncode();
  Interest b(interestBlock);

  BOOST_CHECK_EQUAL(a == b, true);

  Link link2 = b.getLink();
  Link::DelegationSet delegations;
  delegations = link2.getDelegations();

  auto i = delegations.begin();
  BOOST_CHECK_EQUAL(std::get<0>(*i), 10);
  BOOST_CHECK_EQUAL(std::get<1>(*i), Name("test1"));
  ++i;
  BOOST_CHECK_EQUAL(std::get<0>(*i), 20);
  BOOST_CHECK_EQUAL(std::get<1>(*i), Name("test2"));
  ++i;
  BOOST_CHECK_EQUAL(std::get<0>(*i), 100);
  BOOST_CHECK_EQUAL(std::get<1>(*i), Name("test3"));

}

BOOST_AUTO_TEST_CASE(DecodeInterestWithLink)
{
  Block interestBlock(InterestWithLink, sizeof(InterestWithLink));

  ndn::Interest i;
  BOOST_REQUIRE_NO_THROW(i.wireDecode(interestBlock));
  Link link = i.getLink();
  BOOST_CHECK_EQUAL(link.getName(), Name("/local/ndn/prefix"));
  Link::DelegationSet delegations = link.getDelegations();

  auto it = delegations.begin();
  BOOST_CHECK_EQUAL(std::get<0>(*it), 10);
  BOOST_CHECK_EQUAL(std::get<1>(*it), Name("local"));
  ++it;
  BOOST_CHECK_EQUAL(std::get<0>(*it), 20);
  BOOST_CHECK_EQUAL(std::get<1>(*it), Name("ndn"));

  BOOST_REQUIRE_NO_THROW(i.getSelectedDelegation());
  BOOST_CHECK_EQUAL(i.getSelectedDelegation(), Name("local"));
}

BOOST_AUTO_TEST_CASE(DecodeInterestWithLinkNonDecreasingOrder)
{
  Block interestBlock(InterestWithLinkNonDecreasingOrder,
                      sizeof(InterestWithLinkNonDecreasingOrder));

  ndn::Interest i;
  BOOST_REQUIRE_NO_THROW(i.wireDecode(interestBlock));
  BOOST_REQUIRE_NO_THROW(i.getSelectedDelegation());
  BOOST_CHECK_EQUAL(i.getSelectedDelegation(), Name("ndn"));
}

BOOST_AUTO_TEST_CASE(LinkObjectMissingContentType)
{
  Block interestBlock(InterestWithLinkMissingContentType,
                      sizeof(InterestWithLinkMissingContentType));

  ndn::Interest i;
  BOOST_REQUIRE_NO_THROW(i.wireDecode(interestBlock));
  BOOST_REQUIRE_THROW(i.getLink(), Link::Error);
}

BOOST_AUTO_TEST_CASE(LinkObjectNoMetaInfo)
{
  Block interestBlock(InterestWithLinkNoMetaInfo,
                      sizeof(InterestWithLinkNoMetaInfo));

  ndn::Interest i;
  BOOST_REQUIRE_NO_THROW(i.wireDecode(interestBlock));
  BOOST_CHECK_THROW(i.getLink(), tlv::Error);
  BOOST_CHECK_THROW(i.getLink(), tlv::Error);
}

BOOST_AUTO_TEST_CASE(LinkObjectWrongContentType)
{
  Block interestBlock(InterestWithLinkWrongContentType,
                      sizeof(InterestWithLinkWrongContentType));

  ndn::Interest i;
  BOOST_REQUIRE_NO_THROW(i.wireDecode(interestBlock));
  BOOST_CHECK_THROW(i.getLink(), Link::Error);
}

BOOST_AUTO_TEST_CASE(InterestContainingSelectedDelegationButNoLink)
{
  Block interestBlock(InterestWithSelectedDelegationButNoLink,
                      sizeof(InterestWithSelectedDelegationButNoLink));

  ndn::Interest i;
  BOOST_CHECK_THROW(i.wireDecode(interestBlock), Interest::Error);
}

BOOST_AUTO_TEST_CASE(SelectedDelegationIsNotNonNegativeInteger)
{
  Block interestBlock(InterestWithLinkNotNonIntegerSelectedDelegation,
                      sizeof(InterestWithLinkNotNonIntegerSelectedDelegation));

  ndn::Interest i;
  BOOST_CHECK_THROW(i.wireDecode(interestBlock), tlv::Error);
}

BOOST_AUTO_TEST_CASE(SelectedDelegationEqualToDelegationCount)
{
  Link link1("test", {{10, "/test1"}, {20, "/test2"}, {100, "/test3"}});
  m_keyChain.sign(link1);
  Block wire = link1.wireEncode();

  Interest a;
  a.setName("/Test/Encode/Decode/With/Link");
  a.setChildSelector(1);
  a.setNonce(100);
  a.setInterestLifetime(time::seconds(10));
  a.setLink(wire);
  BOOST_CHECK_THROW(a.setSelectedDelegation(3), Interest::Error);
}

BOOST_AUTO_TEST_CASE(SelectedDelegationGreaterThanDelegationCount)
{
  Link link1("test", {{10, "/test1"}, {20, "/test2"}, {100, "/test3"}});
  m_keyChain.sign(link1);
  Block wire = link1.wireEncode();

  Interest a;
  a.setName("/Test/Encode/Decode/With/Link");
  a.setChildSelector(1);
  a.setNonce(100);
  a.setInterestLifetime(time::seconds(10));
  a.setLink(wire);
  BOOST_CHECK_THROW(a.setSelectedDelegation(4), Interest::Error);
}

BOOST_AUTO_TEST_CASE(Decode)
{
  Block interestBlock(Interest1, sizeof(Interest1));

  ndn::Interest i;
  BOOST_REQUIRE_NO_THROW(i.wireDecode(interestBlock));

  BOOST_CHECK_EQUAL(i.getName().toUri(), "/local/ndn/prefix");
  BOOST_CHECK_EQUAL(i.getInterestLifetime(), time::milliseconds(1000));
  BOOST_CHECK_EQUAL(i.getMinSuffixComponents(), 1);
  BOOST_CHECK_EQUAL(i.getMaxSuffixComponents(), 1);
  BOOST_CHECK_EQUAL(i.getPublisherPublicKeyLocator().getType(),
                    static_cast<uint32_t>(KeyLocator::KeyLocator_Name));
  BOOST_CHECK_EQUAL(i.getPublisherPublicKeyLocator().getName(), "ndn:/test/key/locator");
  BOOST_CHECK_EQUAL(i.getChildSelector(), 1);
  BOOST_CHECK_EQUAL(i.getMustBeFresh(), false);
  BOOST_CHECK_EQUAL(i.getExclude().toUri(), "alex,xxxx,*,yyyy");
  BOOST_CHECK_EQUAL(i.getNonce(), 1U);
  BOOST_CHECK_EQUAL(i.hasLink(), false);
  BOOST_CHECK_EQUAL(i.hasSelectedDelegation(), false);
}

BOOST_AUTO_TEST_CASE(DecodeFromStream)
{
  boost::iostreams::stream<boost::iostreams::array_source> is(
    reinterpret_cast<const char *>(Interest1), sizeof(Interest1));

  Block interestBlock = Block::fromStream(is);

  ndn::Interest i;
  BOOST_REQUIRE_NO_THROW(i.wireDecode(interestBlock));

  BOOST_CHECK_EQUAL(i.getName().toUri(), "/local/ndn/prefix");
  BOOST_CHECK_EQUAL(i.getInterestLifetime(), time::milliseconds(1000));
  BOOST_CHECK_EQUAL(i.getMinSuffixComponents(), 1);
  BOOST_CHECK_EQUAL(i.getMaxSuffixComponents(), 1);
  BOOST_CHECK_EQUAL(i.getChildSelector(), 1);
  BOOST_CHECK_EQUAL(i.getMustBeFresh(), false);
  BOOST_CHECK_EQUAL(i.getExclude().toUri(), "alex,xxxx,*,yyyy");
  BOOST_CHECK_EQUAL(i.getNonce(), 1U);
  BOOST_CHECK_EQUAL(i.hasLink(), false);
  BOOST_CHECK_EQUAL(i.hasSelectedDelegation(), false);
}

BOOST_AUTO_TEST_CASE(Encode)
{
  ndn::Interest i(ndn::Name("/local/ndn/prefix"));
  i.setInterestLifetime(time::milliseconds(1000));
  i.setMinSuffixComponents(1);
  i.setMaxSuffixComponents(1);
  i.setPublisherPublicKeyLocator(KeyLocator("ndn:/test/key/locator"));
  i.setChildSelector(1);
  i.setMustBeFresh(false);
  Exclude exclude;
  exclude
    .excludeOne(name::Component("alex"))
    .excludeRange(name::Component("xxxx"), name::Component("yyyy"));
  i.setExclude(exclude);
  i.setNonce(1);

  BOOST_CHECK_EQUAL(i.hasWire(), false);
  const Block &wire = i.wireEncode();
  BOOST_CHECK_EQUAL(i.hasWire(), true);

  BOOST_CHECK_EQUAL_COLLECTIONS(Interest1, Interest1 + sizeof(Interest1),
                                wire.begin(), wire.end());

  const uint8_t* originalWire = wire.wire();
  i.setNonce(2);
  BOOST_CHECK_EQUAL(i.hasWire(), true);
  BOOST_CHECK_EQUAL(originalWire, i.wireEncode().wire());
  BOOST_CHECK_EQUAL(i.hasWire(), true);

  BOOST_CHECK_EQUAL_COLLECTIONS(Interest2, Interest2 + sizeof(Interest2),
                                wire.begin(), wire.end());

  std::ostringstream strStream;
  BOOST_CHECK_NO_THROW(strStream << i);

  BOOST_CHECK_EQUAL(strStream.str(),
                    "/local/ndn/prefix?"
                    "ndn.MinSuffixComponents=1&ndn.MaxSuffixComponents=1&"
                    "ndn.ChildSelector=1&"
                    "ndn.InterestLifetime=1000&"
                    "ndn.Nonce=2&ndn.Exclude=alex,xxxx,*,yyyy");

  i.refreshNonce();
  BOOST_CHECK_EQUAL(i.hasWire(), true);
  BOOST_CHECK_EQUAL(originalWire, i.wireEncode().wire());
  BOOST_CHECK_NE(i.getNonce(), 2);
}

BOOST_AUTO_TEST_CASE(DecodeEncode) // this test case to ensure that wireDecode resets all the fields
{
  Interest i1;
  i1.setName("/test");
  i1.setMinSuffixComponents(100);
  i1.setNonce(10);
  i1.setInterestLifetime(time::seconds(10));
  i1.setLink(Block(LINK, sizeof(LINK)));
  i1.setSelectedDelegation(0);

  Interest i2(i1.wireEncode());

  BOOST_CHECK_EQUAL(i2.getName().toUri(), "/test");
  BOOST_CHECK_EQUAL(i2.getInterestLifetime(), time::seconds(10));
  BOOST_CHECK_EQUAL(i2.getMinSuffixComponents(), 100);
  BOOST_CHECK_EQUAL(i2.getNonce(), 10);
  BOOST_CHECK_EQUAL(i2.hasLink(), true);
  BOOST_CHECK_EQUAL(i2.hasSelectedDelegation(), true);

  i2.wireDecode(Interest().wireEncode());

  BOOST_CHECK_EQUAL(i2.getName().toUri(), "/");
  BOOST_CHECK_EQUAL(i2.getInterestLifetime(), DEFAULT_INTEREST_LIFETIME);
  BOOST_CHECK_EQUAL(i2.getMinSuffixComponents(), -1);
  BOOST_WARN_NE(i2.getNonce(), 10);
  BOOST_CHECK_EQUAL(i2.hasLink(), false);
  BOOST_CHECK_EQUAL(i2.hasSelectedDelegation(), false);
}

BOOST_AUTO_TEST_CASE(MatchesData)
{
  Interest interest;
  interest.setName("ndn:/A")
          .setMinSuffixComponents(2)
          .setMaxSuffixComponents(2)
          .setPublisherPublicKeyLocator(KeyLocator("ndn:/B"))
          .setExclude(Exclude().excludeAfter(name::Component("J")));

  Data data("ndn:/A/D");
  SignatureSha256WithRsa signature(KeyLocator("ndn:/B"));
  data.setSignature(signature);
  data.wireEncode();
  BOOST_CHECK_EQUAL(interest.matchesData(data), true);

  Data data1 = data;
  data1.setName("ndn:/A"); // violates MinSuffixComponents
  data1.wireEncode();
  BOOST_CHECK_EQUAL(interest.matchesData(data1), false);

  Interest interest1 = interest;
  interest1.setMinSuffixComponents(1);
  BOOST_CHECK_EQUAL(interest1.matchesData(data1), true);

  Data data2 = data;
  data2.setName("ndn:/A/E/F"); // violates MaxSuffixComponents
  data2.wireEncode();
  BOOST_CHECK_EQUAL(interest.matchesData(data2), false);

  Interest interest2 = interest;
  interest2.setMaxSuffixComponents(3);
  BOOST_CHECK_EQUAL(interest2.matchesData(data2), true);

  Data data3 = data;
  SignatureSha256WithRsa signature3(KeyLocator("ndn:/G")); // violates PublisherPublicKeyLocator
  data3.setSignature(signature3);
  data3.wireEncode();
  BOOST_CHECK_EQUAL(interest.matchesData(data3), false);

  Interest interest3 = interest;
  interest3.setPublisherPublicKeyLocator(KeyLocator("ndn:/G"));
  BOOST_CHECK_EQUAL(interest3.matchesData(data3), true);

  Data data4 = data;
  DigestSha256 signature4; // violates PublisherPublicKeyLocator
  data4.setSignature(signature4);
  data4.wireEncode();
  BOOST_CHECK_EQUAL(interest.matchesData(data4), false);

  Interest interest4 = interest;
  interest4.setPublisherPublicKeyLocator(KeyLocator());
  BOOST_CHECK_EQUAL(interest4.matchesData(data4), true);

  Data data5 = data;
  data5.setName("ndn:/A/J"); // violates Exclude
  data5.wireEncode();
  BOOST_CHECK_EQUAL(interest.matchesData(data5), false);

  Interest interest5 = interest;
  interest5.setExclude(Exclude().excludeAfter(name::Component("K")));
  BOOST_CHECK_EQUAL(interest5.matchesData(data5), true);

  Data data6 = data;
  data6.setName("ndn:/H/I"); // violates Name
  data6.wireEncode();
  BOOST_CHECK_EQUAL(interest.matchesData(data6), false);

  Data data7 = data;
  data7.setName("ndn:/A/B");
  data7.wireEncode();

  Interest interest7("/A/B/sha256digest=D548DECEFC4B880720DC9257A8D815E9DF4465E63742EE55C29133055DAA67C2");
  BOOST_CHECK_EQUAL(interest7.matchesData(data7), true);

  Interest interest7b("/A/B/sha256digest=0000000000000000000000000000000000000000000000000000000000000000");
  BOOST_CHECK_EQUAL(interest7b.matchesData(data7), false); // violates implicit digest
}

BOOST_AUTO_TEST_CASE(InterestFilterMatching)
{
  BOOST_CHECK_EQUAL(InterestFilter("/a").doesMatch("/a/b"), true);
  BOOST_CHECK_EQUAL(InterestFilter("/a/b").doesMatch("/a/b"), true);
  BOOST_CHECK_EQUAL(InterestFilter("/a/b/c").doesMatch("/a/b"), false);

  BOOST_CHECK_EQUAL(InterestFilter("/a", "<b>").doesMatch("/a/b"), true);
  BOOST_CHECK_EQUAL(InterestFilter("/a/b", "<b>").doesMatch("/a/b"), false);

  BOOST_CHECK_EQUAL(InterestFilter("/a/b", "<b>").doesMatch("/a/b/c/b"), false);
  BOOST_CHECK_EQUAL(InterestFilter("/a/b", "<>*<b>").doesMatch("/a/b/c/b"), true);

  BOOST_CHECK_EQUAL(InterestFilter("/a", "<b>").doesMatch("/a/b/c/d"), false);
  BOOST_CHECK_EQUAL(InterestFilter("/a", "<b><>*").doesMatch("/a/b/c/d"), true);
  BOOST_CHECK_EQUAL(InterestFilter("/a", "<b><>*").doesMatch("/a/b"), true);
  BOOST_CHECK_EQUAL(InterestFilter("/a", "<b><>+").doesMatch("/a/b"), false);
  BOOST_CHECK_EQUAL(InterestFilter("/a", "<b><>+").doesMatch("/a/b/c"), true);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace ndn
