/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
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

#include "management/nfd-command-options.hpp"
#include "security/signing-helpers.hpp"

#ifdef NDN_MANAGEMENT_NFD_COMMAND_OPTIONS_KEEP_DEPRECATED_SIGNING_PARAMS
#include "security/identity-certificate.hpp"
#endif // NDN_MANAGEMENT_NFD_COMMAND_OPTIONS_KEEP_DEPRECATED_SIGNING_PARAMS

#include "boost-test.hpp"

namespace ndn {
namespace nfd {
namespace tests {

BOOST_AUTO_TEST_SUITE(ManagementNfdCommandOptions)

BOOST_AUTO_TEST_CASE(Timeout)
{
  CommandOptions co;
  BOOST_CHECK_EQUAL(co.getTimeout(), CommandOptions::DEFAULT_TIMEOUT);

  co.setTimeout(time::milliseconds(7414));
  BOOST_CHECK_EQUAL(co.getTimeout(), time::milliseconds(7414));

  BOOST_CHECK_THROW(co.setTimeout(time::milliseconds::zero()), std::out_of_range);
  BOOST_CHECK_THROW(co.setTimeout(time::milliseconds(-1)), std::out_of_range);
  BOOST_CHECK_EQUAL(co.getTimeout(), time::milliseconds(7414)); // unchanged after throw

  co.setTimeout(time::milliseconds(1));
  BOOST_CHECK_EQUAL(co.getTimeout(), time::milliseconds(1));
}

BOOST_AUTO_TEST_CASE(Prefix)
{
  CommandOptions co;
  BOOST_CHECK_EQUAL(co.getPrefix(), CommandOptions::DEFAULT_PREFIX);

  co.setPrefix(Name()); // empty Name is okay
  BOOST_CHECK_EQUAL(co.getPrefix(), Name());

  co.setPrefix("ndn:/localhop/net/example/nfd");
  BOOST_CHECK_EQUAL(co.getPrefix(), Name("ndn:/localhop/net/example/nfd"));
}

BOOST_AUTO_TEST_CASE(SigningInfo)
{
  CommandOptions co;
  BOOST_CHECK_EQUAL(co.getSigningInfo().getSignerType(), security::SigningInfo::SIGNER_TYPE_NULL);

  co.setSigningInfo(signingByIdentity("ndn:/tmp/identity"));
  BOOST_CHECK_EQUAL(co.getSigningInfo().getSignerType(), security::SigningInfo::SIGNER_TYPE_ID);
  BOOST_CHECK_EQUAL(co.getSigningInfo().getSignerName(), "ndn:/tmp/identity");
}

#ifdef NDN_MANAGEMENT_NFD_COMMAND_OPTIONS_KEEP_DEPRECATED_SIGNING_PARAMS
BOOST_AUTO_TEST_CASE(SigningParams)
{
  CommandOptions co;
  BOOST_CHECK_EQUAL(co.getSigningParamsKind(), CommandOptions::SIGNING_PARAMS_DEFAULT);

  co.setSigningIdentity("ndn:/tmp/identity");
  BOOST_CHECK_EQUAL(co.getSigningParamsKind(), CommandOptions::SIGNING_PARAMS_IDENTITY);
  BOOST_CHECK_EQUAL(co.getSigningIdentity(), Name("ndn:/tmp/identity"));

  co.setSigningDefault(); // reset
  BOOST_CHECK_EQUAL(co.getSigningParamsKind(), CommandOptions::SIGNING_PARAMS_DEFAULT);

  co.setSigningIdentity(Name()); // empty Name is valid identity Name
  BOOST_CHECK_EQUAL(co.getSigningParamsKind(), CommandOptions::SIGNING_PARAMS_IDENTITY);
  BOOST_CHECK_EQUAL(co.getSigningIdentity(), Name());

  co.setSigningDefault(); // reset
  BOOST_CHECK_EQUAL(co.getSigningParamsKind(), CommandOptions::SIGNING_PARAMS_DEFAULT);

  co.setSigningCertificate("ndn:/tmp/KEY/identity/dsk-1/ID-CERT/%FD%01");
  BOOST_CHECK_EQUAL(co.getSigningParamsKind(), CommandOptions::SIGNING_PARAMS_CERTIFICATE);
  BOOST_CHECK_EQUAL(co.getSigningCertificate(),
                    Name("ndn:/tmp/KEY/identity/dsk-1/ID-CERT/%FD%01"));

  co.setSigningDefault(); // reset
  BOOST_CHECK_EQUAL(co.getSigningParamsKind(), CommandOptions::SIGNING_PARAMS_DEFAULT);

  co.setSigningCertificate("ndn:/KEY/dsk-1/ID-CERT/%FD%01"); // minimal certificateName
  BOOST_CHECK_EQUAL(co.getSigningParamsKind(), CommandOptions::SIGNING_PARAMS_CERTIFICATE);
  BOOST_CHECK_EQUAL(co.getSigningCertificate(), Name("ndn:/KEY/dsk-1/ID-CERT/%FD%01"));

  co.setSigningDefault(); // reset
  BOOST_CHECK_EQUAL(co.getSigningParamsKind(), CommandOptions::SIGNING_PARAMS_DEFAULT);

  BOOST_CHECK_THROW(co.setSigningCertificate(IdentityCertificate()), // invalid certificate
                    std::invalid_argument);
  BOOST_CHECK_EQUAL(co.getSigningParamsKind(),
                    CommandOptions::SIGNING_PARAMS_DEFAULT); // unchanged after throw
}
#endif // NDN_MANAGEMENT_NFD_COMMAND_OPTIONS_KEEP_DEPRECATED_SIGNING_PARAMS

BOOST_AUTO_TEST_SUITE_END()

} // namespace tests
} // namespace nfd
} // namespace ndn
