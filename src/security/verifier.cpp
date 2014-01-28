/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreorder"
#pragma clang diagnostic ignored "-Wtautological-compare"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-function"
#elif __GNUC__
#pragma GCC diagnostic ignored "-Wreorder"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

#include "security/verifier.hpp"

#include "security/sec-policy.hpp"

#include <cryptopp/rsa.h>

#include "../util/logging.hpp"

using namespace std;
#if NDN_CPP_HAVE_CXX11
// In the std library, the placeholders are in a different namespace than boost.
using namespace ndn::func_lib::placeholders;
#endif

INIT_LOGGER("ndn.Verifier");

namespace ndn {
const ptr_lib::shared_ptr<SecPolicy>     Verifier::DefaultPolicy     = ptr_lib::shared_ptr<SecPolicy>();

Verifier::Verifier(const ptr_lib::shared_ptr<SecPolicy>     &policy     /* = DefaultPolicy */)                   
  : m_policy(policy)
{
  if (m_policy == DefaultPolicy)
    {
      // #ifdef USE_SIMPLE_POLICY_MANAGER
      //   Ptr<SimplePolicyManager> policyManager = Ptr<SimplePolicyManager>(new SimplePolicyManager());
      //   Ptr<IdentityPolicyRule> rule1 = Ptr<IdentityPolicyRule>(new IdentityPolicyRule("^([^<KEY>]*)<KEY>(<>*)<ksk-.*><ID-CERT>",
      //                                                                                  "^([^<KEY>]*)<KEY><dsk-.*><ID-CERT>",
      //                                                                                  ">", "\\1\\2", "\\1", true));
      //   Ptr<IdentityPolicyRule> rule2 = Ptr<IdentityPolicyRule>(new IdentityPolicyRule("^([^<KEY>]*)<KEY><dsk-.*><ID-CERT>",
      //                                                                                  "^([^<KEY>]*)<KEY>(<>*)<ksk-.*><ID-CERT>",
      //                                                                                  "==", "\\1", "\\1\\2", true));
      //   Ptr<IdentityPolicyRule> rule3 = Ptr<IdentityPolicyRule>(new IdentityPolicyRule("^(<>*)$", 
      //                                                                                  "^([^<KEY>]*)<KEY><dsk-.*><ID-CERT>", 
      //                                                                                  ">", "\\1", "\\1", true));
      //   policyManager->addVerificationPolicyRule(rule1);
      //   policyManager->addVerificationPolicyRule(rule2);
      //   policyManager->addVerificationPolicyRule(rule3);
    
      //   policyManager->addSigningPolicyRule(rule3);

      //   m_policyManager = policyManager;
      //
      // #else
      //   policy_ = new NoVerifyPolicyManager();
      // #endif
    }  
}

void
Verifier::verify(const ptr_lib::shared_ptr<const Interest> &interest, 
                 const OnVerified &onVerified, 
                 const OnVerifyFailed &onVerifyFailed,
                 int stepCount)
{
  //It does not make sense to verify Interest without specified policy, verification must fail!
  if(!static_cast<bool>(m_policy))
    onVerifyFailed();
  else
    {
      //check verification policy 
      ptr_lib::shared_ptr<ValidationRequest> nextStep = m_policy->checkVerificationPolicy(interest, stepCount, onVerified, onVerifyFailed);
      if (static_cast<bool>(nextStep))
        {
          if(!m_face)
            throw Error("Face should be set prior to verify method to call");

          m_face->expressInterest
            (*nextStep->m_interest,
             func_lib::bind(&Verifier::onCertificateData, this, _1, _2, nextStep), 
             func_lib::bind(&Verifier::onCertificateInterestTimeout, this, _1, nextStep->m_retry, onVerifyFailed, nextStep));
        }
      else
        {
          //If there is no nextStep, that means InterestPolicy has already been able to verify the Interest.
          //No more further processes.
        }
    }
}

void
Verifier::verify(const ptr_lib::shared_ptr<const Data> &data, 
                 const OnVerified &onVerified, 
                 const OnVerifyFailed &onVerifyFailed, 
                 int stepCount)
{
  //It does not make sense to verify Interest without specified policy, verification must fail!
  if(!static_cast<bool>(m_policy))
    onVerifyFailed();
  else
    {
      //check verification policy 
      ptr_lib::shared_ptr<ValidationRequest> nextStep = m_policy->checkVerificationPolicy(data, stepCount, onVerified, onVerifyFailed);
      if (static_cast<bool>(nextStep))
        {
          if(!m_face)
            throw Error("Face should be set prior to verify method to call");

          m_face->expressInterest
            (*nextStep->m_interest,
             func_lib::bind(&Verifier::onCertificateData, this, _1, _2, nextStep), 
             func_lib::bind(&Verifier::onCertificateInterestTimeout, this, _1, nextStep->m_retry, onVerifyFailed, nextStep));
        }
      else
        {
          //If there is no nextStep, that means InterestPolicy has already been able to verify the Interest.
          //No more further processes.
        }
    }
}

void
Verifier::onCertificateData(const ptr_lib::shared_ptr<const Interest> &interest, 
                            const ptr_lib::shared_ptr<Data> &data, 
                            ptr_lib::shared_ptr<ValidationRequest> nextStep)
{
  // Try to verify the certificate (data) according to the parameters in nextStep.
  verify(data, 
         func_lib::bind(nextStep->m_onVerified, data),
         func_lib::bind(nextStep->m_onVerifyFailed, data),
         nextStep->m_stepCount);
}

void
Verifier::onCertificateInterestTimeout
  (const ptr_lib::shared_ptr<const Interest> &interest, int retry, const OnVerifyFailed& onVerifyFailed, ptr_lib::shared_ptr<ValidationRequest> nextStep)
{
  if (retry > 0)
    // Issue the same expressInterest as in verifyData except decrement retry.
    m_face->expressInterest
      (*interest, 
       func_lib::bind(&Verifier::onCertificateData, this, _1, _2, nextStep), 
       func_lib::bind(&Verifier::onCertificateInterestTimeout, this, _1, retry - 1, onVerifyFailed, nextStep));
  else
    onVerifyFailed();
}

bool
Verifier::verifySignature(const Data& data, const Signature& sig, const PublicKey& key)
{
  try{
    switch(sig.getType()){
    case Signature::Sha256WithRsa:
      {
        SignatureSha256WithRsa sigSha256Rsa(sig);
        return verifySignature(data, sigSha256Rsa, key);
      }
    default:
      {
        _LOG_DEBUG("verifySignature: Unknown signature type: " << sig.getType());
        return false;
      }
    }
  }catch(Signature::Error &e){
    _LOG_DEBUG("verifySignature: " << e.what());
    return false;
  }
  return false;
}

bool
Verifier::verifySignature(const Buffer &data, const Signature &sig, const PublicKey &key)
{
  try{
    switch(sig.getType()){
    case Signature::Sha256WithRsa:
      {
        SignatureSha256WithRsa sigSha256Rsa(sig);
        return verifySignature(data, sigSha256Rsa, key);
      }
    default:
      {
        _LOG_DEBUG("verifySignature: Unknown signature type: " << sig.getType());
        return false;
      }
    }
  }catch(Signature::Error &e){
    _LOG_DEBUG("verifySignature: " << e.what());
    return false;
  }
  return false;
}

bool
Verifier::verifySignature(const Data& data, const SignatureSha256WithRsa& sig, const PublicKey& key)
{
  using namespace CryptoPP;

  bool result = false;
  
  RSA::PublicKey publicKey;
  ByteQueue queue;

  queue.Put(reinterpret_cast<const byte*>(key.get().buf()), key.get().size());
  publicKey.Load(queue);

  RSASS<PKCS1v15, SHA256>::Verifier verifier (publicKey);
  result = verifier.VerifyMessage(data.wireEncode().value(), data.wireEncode().value_size() - data.getSignature().getValue().size(),
				  sig.getValue().value(), sig.getValue().value_size());

  _LOG_DEBUG("Signature verified? " << data.getName().toUri() << " " << boolalpha << result);
  
  return result;
}

bool
Verifier::verifySignature(const Buffer& data, const SignatureSha256WithRsa& sig, const PublicKey& key)
{
  using namespace CryptoPP;

  bool result = false;
  
  RSA::PublicKey publicKey;
  ByteQueue queue;

  queue.Put(reinterpret_cast<const byte*>(key.get().buf()), key.get().size());
  publicKey.Load(queue);

  RSASS<PKCS1v15, SHA256>::Verifier verifier (publicKey);
  result = verifier.VerifyMessage(data.buf(), data.size(),
				  sig.getValue().value(), sig.getValue().value_size());

  _LOG_DEBUG("Signature verified? " << data.getName().toUri() << " " << boolalpha << result);
  
  return result;
}

}
