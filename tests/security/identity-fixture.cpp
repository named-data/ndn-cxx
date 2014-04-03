/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi0@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "security/key-chain.hpp"

#include "boost-test.hpp"

namespace ndn {

// OSX KeyChain, when used on a headless server,
// forbids usage of a private key if that key isn't created by the calling process.
// Therefore, unit testing must create its own key pair.

class IdentityFixture
{
public:
  IdentityFixture()
  {
    // save the old default identity
    try {
      m_oldDefaultIdentity = m_keyChain.getDefaultIdentity();
      m_hasOldDefaultIdentity = true;
    }
    catch (SecPublicInfo::Error& e) {
      m_hasOldDefaultIdentity = false;
    }

    m_newIdentity.set("/ndn-cpp-dev-test-identity");
    m_newIdentity.appendVersion();

    // create the new identity and self-signed certificate
    m_keyChain.createIdentity(m_newIdentity);

    // set the new identity as default identity,
    // and the corresponding certificate becomes the default certificate
    m_keyChain.setDefaultIdentity(m_newIdentity);
  }

  ~IdentityFixture()
  {
    // recover the old default setting
    if (m_hasOldDefaultIdentity) {
      m_keyChain.setDefaultIdentity(m_oldDefaultIdentity);
    }

    // remove the temporarily created identity and certificates
    // XXX This has no effect if oldDefaultIdentity doesn't exist.
    //     newIdentity would be kept as default.
    m_keyChain.deleteIdentity(m_newIdentity);
  }

private:
  KeyChain m_keyChain;
  bool m_hasOldDefaultIdentity;
  Name m_oldDefaultIdentity;
  Name m_newIdentity;
};

BOOST_GLOBAL_FIXTURE(IdentityFixture);

} // namespace ndn
