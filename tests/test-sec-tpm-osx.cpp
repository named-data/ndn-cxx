/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi0@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#if __clang__
#pragma clang diagnostic ignored "-Wtautological-compare"
#endif

#include <boost/test/unit_test.hpp>

#include "security/key-chain.hpp"
#include <cryptopp/rsa.h>

using namespace std;
using namespace ndn;


BOOST_AUTO_TEST_SUITE(TestSecTpmOsx)

BOOST_AUTO_TEST_CASE (Delete)
{
  SecTpmOsx tpm;
  
  Name keyName("/tmp/ksk-123456");
  tpm.generateKeyPairInTpm(keyName, KEY_TYPE_RSA, 2048);
  
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), true);
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), true);
  
  tpm.deleteKeyPairInTpm(keyName);
  
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PUBLIC), false);
  BOOST_REQUIRE_EQUAL(tpm.doesKeyExistInTpm(keyName, KEY_CLASS_PRIVATE), false);
}

BOOST_AUTO_TEST_CASE (SignVerify)
{
  SecTpmOsx tpm;

  Name keyName("/tmp/ksk-123456");
  tpm.generateKeyPairInTpm(keyName, KEY_TYPE_RSA, 2048);
  
  Data data("/tmp/test/1");
  const uint8_t content[] = {0x01, 0x02, 0x03, 0x04};

  Block sigBlock = tpm.signInTpm(content, sizeof(content), keyName, DIGEST_ALGORITHM_SHA256);
  ptr_lib::shared_ptr<PublicKey> pubkeyPtr = tpm.getPublicKeyFromTpm(keyName);

  {
    using namespace CryptoPP;
    
    RSA::PublicKey publicKey;
    ByteQueue queue;
    queue.Put(reinterpret_cast<const byte*>(pubkeyPtr->get().buf()), pubkeyPtr->get().size());
    publicKey.Load(queue);

    RSASS<PKCS1v15, SHA256>::Verifier verifier (publicKey);
    bool result = verifier.VerifyMessage(content, sizeof(content),
				  sigBlock.value(), sigBlock.value_size());
  
    BOOST_REQUIRE_EQUAL(result, true);
  }

  tpm.deleteKeyPairInTpm(keyName);
}

BOOST_AUTO_TEST_SUITE_END()
