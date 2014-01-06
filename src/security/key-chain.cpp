/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <ndn-cpp/security/key-chain.hpp>

#include <ndn-cpp/security/policy/policy-manager.hpp>

#include <ndn-cpp/security/identity/basic-identity-storage.hpp>


using namespace std;
using namespace ndn::func_lib;
#if NDN_CPP_HAVE_CXX11
// In the std library, the placeholders are in a different namespace than boost.
using namespace ndn::func_lib::placeholders;
#endif

namespace ndn {

const ptr_lib::shared_ptr<IdentityStorage>   KeyChain::DefaultIdentityStorage   = ptr_lib::shared_ptr<IdentityStorage>();
const ptr_lib::shared_ptr<PrivateKeyStorage> KeyChain::DefaultPrivateKeyStorage = ptr_lib::shared_ptr<PrivateKeyStorage>();
const ptr_lib::shared_ptr<PolicyManager>     KeyChain::DefaultPolicyManager     = ptr_lib::shared_ptr<PolicyManager>();
const ptr_lib::shared_ptr<EncryptionManager> KeyChain::DefaultEncryptionManager = ptr_lib::shared_ptr<EncryptionManager>();

KeyChain::KeyChain(const ptr_lib::shared_ptr<IdentityStorage>   &publicInfoStorage /* = DefaultIdentityStorage */,
                   const ptr_lib::shared_ptr<PrivateKeyStorage> &privateKeyStorage /* = DefaultPrivateKeyStorage */,
                   const ptr_lib::shared_ptr<PolicyManager>     &policyManager     /* = DefaultPolicyManager */,
                   const ptr_lib::shared_ptr<EncryptionManager> &encryptionManager /* = DefaultEncryptionManager */)
  : publicInfoStorage_(publicInfoStorage)
  , privateKeyStorage_(privateKeyStorage)
  , policyManager_(policyManager)
  , encryptionManager_(encryptionManager)
  // , maxSteps_(100)
{
  if (publicInfoStorage_ == DefaultIdentityStorage)
    {
      publicInfoStorage_ = ptr_lib::make_shared<BasicIdentityStorage>();
    }

  if (privateKeyStorage_ == DefaultPrivateKeyStorage)
    {
#ifdef USE_OSX_PRIVATEKEY_STORAGE
      privateStorage_ = ptr_lib::make_shared<OSXPrivatekeyStorage>();
      // #else
      //       m_privateStorage = Ptr<SimpleKeyStore>::Create();
#endif  
    }

  identityManager_ = ptr_lib::make_shared<IdentityManager>(publicInfoStorage_, privateKeyStorage_);

  if (policyManager_ == DefaultPolicyManager)
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
      //   policyManager_ = new NoVerifyPolicyManager();
      // #endif
    }
  
  if (encryptionManager_ == DefaultEncryptionManager)
    {
    }

// #ifdef USE_BASIC_ENCRYPTION_MANAGER
//     encryptionManager_ = new BasicEncryptionManager(m_identityManager->getPrivateStorage(), "/tmp/encryption.db");
// #endif
}


void 
KeyChain::signByIdentity(Data& data, const Name& identityName)
{
  Name signingCertificateName;
  
  if (identityName.getComponentCount() == 0) {
    Name inferredIdentity = policyManager_->inferSigningIdentity(data.getName());
    if (inferredIdentity.getComponentCount() == 0)
      signingCertificateName = identityManager_->getDefaultCertificateName();
    else
      signingCertificateName = identityManager_->getDefaultCertificateNameForIdentity(inferredIdentity);    
  }
  else
    signingCertificateName = identityManager_->getDefaultCertificateNameForIdentity(identityName);

  if (signingCertificateName.getComponentCount() == 0)
    throw Error("No qualified certificate name found!");

  if (!policyManager_->checkSigningPolicy(data.getName(), signingCertificateName))
    throw Error("Signing Cert name does not comply with signing policy");

  identities().signByCertificate(data, signingCertificateName);
}

Signature
KeyChain::signByIdentity(const uint8_t* buffer, size_t bufferLength, const Name& identityName)
{
  Name signingCertificateName = identityManager_->getDefaultCertificateNameForIdentity(identityName);
    
  if (signingCertificateName.size() == 0)
    throw Error("No qualified certificate name found!");

  return identities().signByCertificate(buffer, bufferLength, signingCertificateName);
}

void
KeyChain::verifyData
  (const ptr_lib::shared_ptr<Data>& data, const OnVerified& onVerified, const OnVerifyFailed& onVerifyFailed, int stepCount)
{
  if (policies().requireVerify(*data)) {
    ptr_lib::shared_ptr<ValidationRequest> nextStep = policyManager_->checkVerificationPolicy
      (data, stepCount, onVerified, onVerifyFailed);
    if (nextStep)
      {
        if (!face_)
          throw Error("Face should be set prior to verifyData method to call");
        
        face_->expressInterest
          (*nextStep->interest_, 
           bind(&KeyChain::onCertificateData, this, _1, _2, nextStep), 
           bind(&KeyChain::onCertificateInterestTimeout, this, _1, nextStep->retry_, onVerifyFailed, data, nextStep));
      }
  }
  else if (policies().skipVerifyAndTrust(*data))
    onVerified(data);
  else
    onVerifyFailed(data);
}

void
KeyChain::onCertificateData(const ptr_lib::shared_ptr<const Interest> &interest, const ptr_lib::shared_ptr<Data> &data, ptr_lib::shared_ptr<ValidationRequest> nextStep)
{
  // Try to verify the certificate (data) according to the parameters in nextStep.
  verifyData(data, nextStep->onVerified_, nextStep->onVerifyFailed_, nextStep->stepCount_);
}

void
KeyChain::onCertificateInterestTimeout
  (const ptr_lib::shared_ptr<const Interest> &interest, int retry, const OnVerifyFailed& onVerifyFailed, const ptr_lib::shared_ptr<Data> &data, 
   ptr_lib::shared_ptr<ValidationRequest> nextStep)
{
  if (retry > 0)
    // Issue the same expressInterest as in verifyData except decrement retry.
    face_->expressInterest
      (*interest, 
       bind(&KeyChain::onCertificateData, this, _1, _2, nextStep), 
       bind(&KeyChain::onCertificateInterestTimeout, this, _1, retry - 1, onVerifyFailed, data, nextStep));
  else
    onVerifyFailed(data);
}

}
