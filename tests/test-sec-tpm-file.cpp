/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi0@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#if __clang__
#pragma clang diagnostic ignored "-Wtautological-compare"
// #pragma clang diagnostic push
// #pragma clang diagnostic ignored "-Wreorder"
// #pragma clang diagnostic ignored "-Wunused-variable"
// #pragma clang diagnostic ignored "-Wunused-function"
// #elif __GNUC__
// #pragma GCC diagnostic ignored "-Wreorder"
// #pragma GCC diagnostic ignored "-Wunused-variable"
// #pragma GCC diagnostic ignored "-Wunused-function"
#endif

#include <boost/test/unit_test.hpp>

#include <ndn-cpp-dev/security/key-chain.hpp>
#include <cryptopp/rsa.h>

using namespace std;
using namespace ndn;


BOOST_AUTO_TEST_SUITE(TestSecTpmFile)

BOOST_AUTO_TEST_CASE (SignVerify)
{
  SecTpmFile tpm;

  Name keyName("/tmp/ksk-123456");
  try {
    tpm.generateKeyPairInTpm(keyName, KEY_TYPE_RSA, 2048);
  }
  catch(const SecTpm::Error&) {
  }
  
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

  //We should remove the temporary test key, this should be fixed in a later commit which will add delete operation in SecTpm.
}

BOOST_AUTO_TEST_SUITE_END()
