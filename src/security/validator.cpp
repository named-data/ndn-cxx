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
 * @author Jeff Thompson <jefft0@remap.ucla.edu>
 */

#include "common.hpp"

#include "validator.hpp"
#include "../util/crypto.hpp"

#include "cryptopp.hpp"

using namespace std;

namespace ndn {

Validator::Validator()
  : m_hasFace(false)
  , m_face(*static_cast<Face*>(0))
{
}

Validator::Validator(Face& face)
  : m_hasFace(true)
  , m_face(face)
{
}

void
Validator::validate(const Interest& interest,
                    const OnInterestValidated& onValidated,
                    const OnInterestValidationFailed& onValidationFailed,
                    int nSteps)
{
  vector<shared_ptr<ValidationRequest> > nextSteps;
  checkPolicy(interest, nSteps, onValidated, onValidationFailed, nextSteps);

  if (!nextSteps.empty())
    {
      if (!m_hasFace)
        {
          onValidationFailed(interest.shared_from_this(),
                             "Require more information to validate the interest!");
          return;
        }

      vector<shared_ptr<ValidationRequest> >::const_iterator it = nextSteps.begin();
      OnFailure onFailure = bind(onValidationFailed, interest.shared_from_this(), _1);
      for (; it != nextSteps.end(); it++)
        m_face.expressInterest((*it)->m_interest,
                               bind(&Validator::onData, this, _1, _2, *it),
                               bind(&Validator::onTimeout,
                                    this, _1, (*it)->m_nRetrials,
                                    onFailure,
                                    *it));
    }
  else
    {
      // If there is no nextStep,
      // that means InterestPolicy has already been able to verify the Interest.
      // No more further processes.
    }
}

void
Validator::validate(const Data& data,
                    const OnDataValidated& onValidated,
                    const OnDataValidationFailed& onValidationFailed,
                    int nSteps)
{
  vector<shared_ptr<ValidationRequest> > nextSteps;
  checkPolicy(data, nSteps, onValidated, onValidationFailed, nextSteps);

  if (!nextSteps.empty())
    {
      if (!m_hasFace)
        {
          onValidationFailed(data.shared_from_this(),
                             "Require more information to validate the data!");
        }

      vector<shared_ptr<ValidationRequest> >::const_iterator it = nextSteps.begin();
      OnFailure onFailure = bind(onValidationFailed, data.shared_from_this(), _1);
      for (; it != nextSteps.end(); it++)
        m_face.expressInterest((*it)->m_interest,
                               bind(&Validator::onData, this, _1, _2, *it),
                               bind(&Validator::onTimeout,
                                    this, _1, (*it)->m_nRetrials,
                                    onFailure,
                                    *it));
    }
  else
    {
      // If there is no nextStep,
      // that means Data Policy has already been able to verify the Interest.
      // No more further processes.
    }
}

void
Validator::onData(const Interest& interest,
                  const Data& data,
                  const shared_ptr<ValidationRequest>& nextStep)
{
  validate(data, nextStep->m_onValidated, nextStep->m_onDataValidated, nextStep->m_nSteps);
}

void
Validator::onTimeout(const Interest& interest,
                     int nRetrials,
                     const OnFailure& onFailure,
                     const shared_ptr<ValidationRequest>& nextStep)
{
  if (nRetrials > 0)
    // Issue the same expressInterest except decrement nRetrials.
    m_face.expressInterest(interest,
                            bind(&Validator::onData, this, _1, _2, nextStep),
                            bind(&Validator::onTimeout, this, _1,
                                 nRetrials - 1, onFailure, nextStep));
  else
    onFailure("Cannot fetch cert: " + interest.getName().toUri());
}

bool
Validator::verifySignature(const Data& data, const PublicKey& key)
{
  try
    {
      switch (data.getSignature().getType())
        {
        case Signature::Sha256WithRsa:
          {
            SignatureSha256WithRsa sigSha256Rsa(data.getSignature());
            return verifySignature(data, sigSha256Rsa, key);
          }
        default:
          {
            return false;
          }
        }
    }
  catch (Signature::Error& e)
    {
      return false;
    }
  return false;
}

bool
Validator::verifySignature(const Interest& interest, const PublicKey& key)
{
  const Name& interestName = interest.getName();

  if (interestName.size() < 2)
    return false;

  try
    {
      const Block& nameBlock = interestName.wireEncode();

      Signature sig(interestName[-2].blockFromValue(),
                    interestName[-1].blockFromValue());

      switch (sig.getType())
        {
        case Signature::Sha256WithRsa:
          {
            SignatureSha256WithRsa sigSha256Rsa(sig);

            return verifySignature(nameBlock.value(),
                                   nameBlock.value_size() - interestName[-1].size(),
                                   sigSha256Rsa, key);
          }
        default:
          {
            return false;
          }
        }
    }
  catch (Signature::Error& e)
    {
      return false;
    }
  catch (Block::Error& e)
    {
      return false;
    }
  return false;
}

bool
Validator::verifySignature(const Buffer& data, const Signature& sig, const PublicKey& key)
{
  try
    {
      switch (sig.getType())
        {
        case Signature::Sha256WithRsa:
          {
            SignatureSha256WithRsa sigSha256Rsa(sig);
            return verifySignature(data, sigSha256Rsa, key);
          }
        default:
          {
            return false;
          }
        }
    }
  catch (Signature::Error& e)
    {
      return false;
    }
  return false;
}

bool
Validator::verifySignature(const uint8_t* buf,
                           const size_t size,
                           const SignatureSha256WithRsa& sig,
                           const PublicKey& key)
{
  try
    {
      using namespace CryptoPP;

      RSA::PublicKey publicKey;
      ByteQueue queue;

      queue.Put(reinterpret_cast<const byte*>(key.get().buf()), key.get().size());
      publicKey.Load(queue);

      RSASS<PKCS1v15, SHA256>::Verifier verifier(publicKey);
      return verifier.VerifyMessage(buf, size,
                                    sig.getValue().value(),
                                    sig.getValue().value_size());
    }
  catch (CryptoPP::Exception& e)
    {
      return false;
    }
}

bool
Validator::verifySignature(const uint8_t* buf, const size_t size, const SignatureSha256& sig)
{
  try
    {
      ConstBufferPtr buffer = crypto::sha256(buf, size);
      const Block& sigValue = sig.getValue();

      if (static_cast<bool>(buffer) &&
          buffer->size() == sigValue.value_size() &&
          buffer->size() == crypto::SHA256_DIGEST_SIZE)
        {

          const uint8_t* p1 = buffer->buf();
          const uint8_t* p2 = sigValue.value();

          return 0 == memcmp(p1, p2, crypto::SHA256_DIGEST_SIZE);
        }
      else
        return false;
    }
  catch (CryptoPP::Exception& e)
    {
      return false;
    }
}

} // namespace ndn
