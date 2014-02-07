/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "common.hpp"

#include "validator-regex.hpp"
#include "signature-sha256-with-rsa.hpp"
#include "certificate-cache-ttl.hpp"

#include "../util/logging.hpp"

INIT_LOGGER("ndn::ValidatorRegex");

using namespace std;

namespace ndn
{

const shared_ptr<CertificateCache> ValidatorRegex::DefaultCertificateCache = shared_ptr<CertificateCache>();

ValidatorRegex::ValidatorRegex(shared_ptr<Face> face,
                               shared_ptr<CertificateCache> certificateCache /* = DefaultCertificateCache */,
                               const int stepLimit /* = 3 */)
  : Validator(face)
  , m_stepLimit(stepLimit)
  , m_certificateCache(certificateCache)
{
  if(!static_cast<bool>(face))
    throw Error("Face is not set!");

  if(!static_cast<bool>(m_certificateCache))
    m_certificateCache = make_shared<CertificateCacheTtl>(m_face->ioService());
}

void
ValidatorRegex::onCertificateValidated(const shared_ptr<const Data> &signCertificate, 
                                       const shared_ptr<const Data> &data, 
                                       const OnDataValidated &onValidated, 
                                       const OnDataValidationFailed &onValidationFailed)
{
  shared_ptr<IdentityCertificate> certificate = make_shared<IdentityCertificate>(*signCertificate);
  
  if(!certificate->isTooLate() && !certificate->isTooEarly())
    {
      m_certificateCache->insertCertificate(certificate);
      
      try{
        if(verifySignature(*data, certificate->getPublicKeyInfo()))
          {
            onValidated(data);
            return;
          }
      }catch(Signature::Error &e){
        _LOG_DEBUG("ValidatorRegex Error: " << e.what());
        onValidationFailed(data);
        return;
      }
    }
  else
    {
      _LOG_DEBUG("Wrong Invalidity: " << e.what());
      onValidationFailed(data);
      return;
    }
}

void
ValidatorRegex::onCertificateValidationFailed(const shared_ptr<const Data> &signCertificate, 
                                              const shared_ptr<const Data> &data, 
                                              const OnDataValidationFailed &onValidationFailed)
{ onValidationFailed(data); }

void
ValidatorRegex::checkPolicy(const shared_ptr<const Data> &data, 
                            int stepCount, 
                            const OnDataValidated &onValidated, 
                            const OnDataValidationFailed &onValidationFailed,
                            vector<shared_ptr<ValidationRequest> > &nextSteps)
{
  if(m_stepLimit == stepCount){
    _LOG_DEBUG("reach the maximum steps of verification");
    onValidationFailed(data);
    return;
  }
  
  RuleList::iterator it = m_mustFailVerify.begin();
  for(; it != m_mustFailVerify.end(); it++)
    if((*it)->satisfy(*data))
      {
        onValidationFailed(data);
        return;
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
              if(verifySignature(*data, sig, trustedCert->getPublicKeyInfo()))
                onValidated(data);
              else
                onValidationFailed(data);
              
              return;
            }
            else{
              // _LOG_DEBUG("KeyLocator is not trust anchor");                
              OnDataValidated onKeyValidated = bind(&ValidatorRegex::onCertificateValidated, this, 
                                                    _1, data, onValidated, onValidationFailed);
              
              OnDataValidationFailed onKeyValidationFailed = bind(&ValidatorRegex::onCertificateValidationFailed, this, 
                                                                  _1, data, onValidationFailed);              

              shared_ptr<ValidationRequest> nextStep = make_shared<ValidationRequest>(Interest(boost::cref(sig.getKeyLocator().getName())), 
                                                                                      onKeyValidated,
                                                                                      onKeyValidationFailed,
                                                                                      3,
                                                                                      stepCount + 1);
              nextSteps.push_back(nextStep);
              return;
            }
          }catch(SignatureSha256WithRsa::Error &e){
            _LOG_DEBUG("ValidatorRegex Error: " << e.what());
            onValidationFailed(data);
            return;
          }catch(KeyLocator::Error &e){
            _LOG_DEBUG("ValidatorRegex Error: " << e.what());
            onValidationFailed(data);
            return;
          }
        }
    }

  onValidationFailed(data);
  return;
}

}//ndn
