/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "sec-policy-regex.hpp"

#include "verifier.hpp"
#include "signature-sha256-with-rsa.hpp"

#include "../util/logging.hpp"

INIT_LOGGER("SecPolicyRegex");

using namespace std;

namespace ndn
{

SecPolicyRegex::SecPolicyRegex(shared_ptr<CertificateCache> certificateCache,
                                 const int stepLimit)
  : m_stepLimit(stepLimit)
  , m_certificateCache(certificateCache)
{}

void
SecPolicyRegex::onCertificateVerified(shared_ptr<Data>signCertificate, 
                                       shared_ptr<Data>data, 
                                       const OnVerified& onVerified, 
                                       const OnVerifyFailed& onVerifyFailed)
{
  shared_ptr<IdentityCertificate> certificate = make_shared<IdentityCertificate>(*signCertificate);
  
  if(!certificate->isTooLate() && !certificate->isTooEarly())
    {
      m_certificateCache->insertCertificate(certificate);
      
      try{
        if(Verifier::verifySignature(*data, data->getSignature(), certificate->getPublicKeyInfo()))
          {
            onVerified();
            return;
          }
      }catch(Signature::Error &e){
        _LOG_DEBUG("SecPolicyRegex Error: " << e.what());
        onVerifyFailed();
        return;
      }
    }
  else
    {
      onVerifyFailed();
      return;
    }
}

void
SecPolicyRegex::onCertificateVerifyFailed(shared_ptr<Data>signCertificate, 
                                           shared_ptr<Data>data, 
                                           const OnVerifyFailed& onVerifyFailed)
{ onVerifyFailed(); }

shared_ptr<ValidationRequest>
SecPolicyRegex::checkVerificationPolicy(const shared_ptr<Data>& data, 
                                         int stepCount, 
                                         const OnVerified& onVerified, 
                                         const OnVerifyFailed& onVerifyFailed)
{
  if(m_stepLimit == stepCount){
    _LOG_DEBUG("reach the maximum steps of verification");
    onVerifyFailed();
    return shared_ptr<ValidationRequest>();
  }
  
  RuleList::iterator it = m_mustFailVerify.begin();
  for(; it != m_mustFailVerify.end(); it++)
    {
      if((*it)->satisfy(*data))
        {
          onVerifyFailed();
          return shared_ptr<ValidationRequest>();
        }
    }
  
  it = m_verifyPolicies.begin();
  for(; it != m_verifyPolicies.end(); it++)
    {
      if((*it)->satisfy(*data))
        {
          try{
            SignatureSha256WithRsa sig(data->getSignature());                
            
            Name keyLocatorName = sig.getKeyLocator().getName();
            shared_ptr<const Certificate> trustedCert;
            if(m_trustAnchors.end() == m_trustAnchors.find(keyLocatorName))
              trustedCert = m_certificateCache->getCertificate(keyLocatorName);
            else
              trustedCert = m_trustAnchors[keyLocatorName];
            
            if(static_cast<bool>(trustedCert)){
              if(Verifier::verifySignature(*data, sig, trustedCert->getPublicKeyInfo()))
                onVerified();
              else
                onVerifyFailed();
              
              return shared_ptr<ValidationRequest>();
            }
            else{
              // _LOG_DEBUG("KeyLocator is not trust anchor");                
              ValidationRequest::OnCertVerified onCertVerified = bind(&SecPolicyRegex::onCertificateVerified, 
                                                                      this, 
                                                                      _1, 
                                                                      data, 
                                                                      onVerified, 
                                                                      onVerifyFailed);
              
              ValidationRequest::OnCertVerifyFailed onCertVerifyFailed = bind(&SecPolicyRegex::onCertificateVerifyFailed, 
                                                                              this, 
                                                                              _1, 
                                                                              data, 
                                                                              onVerifyFailed);
              
              
              shared_ptr<Interest> interest = make_shared<Interest>(boost::cref(sig.getKeyLocator().getName()));

              shared_ptr<ValidationRequest> nextStep = make_shared<ValidationRequest>(interest, 
                                                                                                        onCertVerified,
                                                                                                        onCertVerifyFailed,
                                                                                                        3,
                                                                                                        stepCount + 1);
              return nextStep;
            }
          }catch(SignatureSha256WithRsa::Error &e){
            _LOG_DEBUG("SecPolicyRegex Error: " << e.what());
            onVerifyFailed();
            return shared_ptr<ValidationRequest>(); 
          }catch(KeyLocator::Error &e){
            _LOG_DEBUG("SecPolicyRegex Error: " << e.what());
            onVerifyFailed();
            return shared_ptr<ValidationRequest>(); 
          }
        }
    }
  
  onVerifyFailed();
  return shared_ptr<ValidationRequest>();
}

}//ndn
