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

#include <ndn-cpp/security/verifier.hpp>

#include <ndn-cpp/security/sec-policy.hpp>

#include <cryptopp/rsa.h>

#include "../util/logging.hpp"

using namespace std;
using namespace ndn::func_lib;
#if NDN_CPP_HAVE_CXX11
// In the std library, the placeholders are in a different namespace than boost.
using namespace ndn::func_lib::placeholders;
#endif

INIT_LOGGER("ndn.Verifier");

namespace ndn {
const ptr_lib::shared_ptr<SecPolicy>     Verifier::DefaultPolicy     = ptr_lib::shared_ptr<SecPolicy>();

Verifier::Verifier(const ptr_lib::shared_ptr<SecPolicy>     &policy     /* = DefaultPolicy */)                   
  : policy_(policy)
{
  if (policy_ == DefaultPolicy)
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
Verifier::verifyData
  (const ptr_lib::shared_ptr<Data>& data, const OnVerified& onVerified, const OnVerifyFailed& onVerifyFailed, int stepCount)
{
  if (policy().requireVerify(*data)) {
    ptr_lib::shared_ptr<ValidationRequest> nextStep = policy_->checkVerificationPolicy
      (data, stepCount, onVerified, onVerifyFailed);
    if (static_cast<bool>(nextStep))
      {
        if (!face_)
          throw Error("Face should be set prior to verifyData method to call");
        
        face_->expressInterest
          (*nextStep->interest_, 
           bind(&Verifier::onCertificateData, this, _1, _2, nextStep), 
           bind(&Verifier::onCertificateInterestTimeout, this, _1, nextStep->retry_, onVerifyFailed, data, nextStep));
      }
  }
  else if (policy().skipVerifyAndTrust(*data))
    onVerified(data);
  else
    onVerifyFailed(data);
}

void
Verifier::onCertificateData(const ptr_lib::shared_ptr<const Interest> &interest, const ptr_lib::shared_ptr<Data> &data, ptr_lib::shared_ptr<ValidationRequest> nextStep)
{
  // Try to verify the certificate (data) according to the parameters in nextStep.
  verifyData(data, nextStep->onVerified_, nextStep->onVerifyFailed_, nextStep->stepCount_);
}

void
Verifier::onCertificateInterestTimeout
  (const ptr_lib::shared_ptr<const Interest> &interest, int retry, const OnVerifyFailed& onVerifyFailed, const ptr_lib::shared_ptr<Data> &data, 
   ptr_lib::shared_ptr<ValidationRequest> nextStep)
{
  if (retry > 0)
    // Issue the same expressInterest as in verifyData except decrement retry.
    face_->expressInterest
      (*interest, 
       bind(&Verifier::onCertificateData, this, _1, _2, nextStep), 
       bind(&Verifier::onCertificateInterestTimeout, this, _1, retry - 1, onVerifyFailed, data, nextStep));
  else
    onVerifyFailed(data);
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
