/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <ndn-cpp/common.hpp>

#include "public-key.hpp"

#if NDN_CPP_USE_SYSTEM_BOOST
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
namespace ndnboost = boost;
#else
// We can use ndnboost::iostreams because this is internal and will not conflict with the application if it uses boost::iostreams.
#include <ndnboost/iostreams/stream.hpp>
#include <ndnboost/iostreams/device/array.hpp>
#endif

#include <cryptopp/rsa.h>
#include <cryptopp/base64.h>
#include <cryptopp/files.h>

using namespace std;
using namespace CryptoPP;

namespace ndn {

static OID RSA_OID("1.2.840.113549.1.1.1");

PublicKey::PublicKey()
{
}

/**
 * Create a new PublicKey with the given values.
 * @param algorithm The algorithm of the public key.
 * @param keyDer The blob of the PublicKeyInfo in terms of DER.
 */
PublicKey::PublicKey(const uint8_t *keyDerBuf, size_t keyDerSize)
{
  StringSource src(keyDerBuf, keyDerSize, true);
  decode(src);
}

void
PublicKey::encode(CryptoPP::BufferedTransformation &out) const
{
  // SubjectPublicKeyInfo ::= SEQUENCE {
  //     algorithm           AlgorithmIdentifier
  //     keybits             BIT STRING   }

  out.Put(key_.buf(), key_.size());
}

void
PublicKey::decode(CryptoPP::BufferedTransformation &in)
{
  // SubjectPublicKeyInfo ::= SEQUENCE {
  //     algorithm           AlgorithmIdentifier
  //     keybits             BIT STRING   }

  try {
    std::string out;
    StringSink sink(out);

    ////////////////////////
    // part 1: copy as is //
    ////////////////////////
    BERSequenceDecoder decoder(in);
    {
      assert (decoder.IsDefiniteLength());

      DERSequenceEncoder encoder(sink);
      decoder.TransferTo(encoder, decoder.RemainingLength());
      encoder.MessageEnd();
    }
    decoder.MessageEnd();

    ////////////////////////
    // part 2: check if the key is RSA (since it is the only supported for now)
    ////////////////////////
    StringSource checkedSource(out, true);
    BERSequenceDecoder subjectPublicKeyInfo(checkedSource);
    {
      BERSequenceDecoder algorithmInfo(subjectPublicKeyInfo);
      {
        OID algorithm;
        algorithm.decode(algorithmInfo);

        if (algorithm != RSA_OID)
          throw Error("Only RSA public keys are supported for now (" + algorithm.toString() + " requested");
      }
    }

    key_.assign(out.begin(), out.end());
  }
  catch (CryptoPP::BERDecodeErr &err) {
    throw Error("PublicKey decoding error");
  }
}

// Blob
// PublicKey::getDigest(DigestAlgorithm digestAlgorithm) const
// {
//   if (digestAlgorithm == DIGEST_ALGORITHM_SHA256) {
//     uint8_t digest[SHA256_DIGEST_LENGTH];
//     ndn_digestSha256(keyDer_.buf(), keyDer_.size(), digest);
    
//     return Blob(digest, sizeof(digest));
//   }
//   else
//     throw UnrecognizedDigestAlgorithmException("Wrong format!");
// }

std::ostream &
operator <<(std::ostream &os, const PublicKey &key)
{
  CryptoPP::StringSource(key.get().buf(), key.get().size(), true,
                         new CryptoPP::Base64Encoder(new CryptoPP::FileSink(os), true, 64));

  return os;
}

}
