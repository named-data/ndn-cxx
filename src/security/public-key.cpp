/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
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
 *
 * @author Yingdi Yu <http://irl.cs.ucla.edu/~yingdi/>
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 */

#include "public-key.hpp"

#include "../encoding/oid.hpp"
#include "../util/crypto.hpp"
#include "cryptopp.hpp"

namespace ndn {

PublicKey::PublicKey()
  : m_type(KEY_TYPE_NULL)
{
}

PublicKey::PublicKey(const uint8_t* keyDerBuf, size_t keyDerSize)
  : m_type(KEY_TYPE_NULL)
{
  CryptoPP::StringSource src(keyDerBuf, keyDerSize, true);
  decode(src);
}

const Block&
PublicKey::computeDigest() const
{
  if (m_key.empty())
    BOOST_THROW_EXCEPTION(Error("Public key is empty"));

  if (m_digest.hasWire())
    return m_digest;
  else {
    m_digest = Block(tlv::KeyDigest, crypto::sha256(m_key.buf(), m_key.size()));
    m_digest.encode();
    return m_digest;
  }
}


void
PublicKey::encode(CryptoPP::BufferedTransformation& out) const
{
  // SubjectPublicKeyInfo ::= SEQUENCE {
  //     algorithm           AlgorithmIdentifier
  //     keybits             BIT STRING   }

  out.Put(m_key.buf(), m_key.size());
}

void
PublicKey::decode(CryptoPP::BufferedTransformation& in)
{
  // SubjectPublicKeyInfo ::= SEQUENCE {
  //     algorithm           AlgorithmIdentifier
  //     keybits             BIT STRING   }

  using namespace CryptoPP;
  try
    {
      std::string out;
      StringSink sink(out);

      ////////////////////////
      // part 1: copy as is //
      ////////////////////////
      BERSequenceDecoder decoder(in);
      {
        assert(decoder.IsDefiniteLength());

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

          if (algorithm == oid::RSA)
            m_type = KEY_TYPE_RSA;
          else if (algorithm == oid::ECDSA)
            m_type = KEY_TYPE_ECDSA;
          else
            BOOST_THROW_EXCEPTION(Error("Only RSA/ECDSA public keys are supported for now (" +
                                        algorithm.toString() + " requested)"));
        }
      }

      m_key.assign(out.begin(), out.end());
    }
  catch (CryptoPP::BERDecodeErr& err)
    {
      m_type = KEY_TYPE_NULL;
      BOOST_THROW_EXCEPTION(Error("PublicKey decoding error"));
    }

  m_digest.reset();
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

std::ostream&
operator<<(std::ostream& os, const PublicKey& key)
{
  CryptoPP::StringSource(key.get().buf(), key.get().size(), true,
                         new CryptoPP::Base64Encoder(new CryptoPP::FileSink(os), true, 64));

  return os;
}

} // namespace ndn
