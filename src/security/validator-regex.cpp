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

namespace ndn {

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
      
      if(verifySignature(*data, certificate->getPublicKeyInfo()))
        return onValidated(data);
      else
        return onValidationFailed(data, 
                                  "Cannot verify signature: " + data->getName().toUri());
    }
  else
    {
      _LOG_DEBUG("Wrong validity:");
      return onValidationFailed(data, 
                                "Signing certificate " + signCertificate->getName().toUri() + " is no longer valid.");
    }
}

void
ValidatorRegex::onCertificateValidationFailed(const shared_ptr<const Data> &signCertificate, 
                                              const string& failureInfo,
                                              const shared_ptr<const Data> &data, 
                                              const OnDataValidationFailed &onValidationFailed)
{ onValidationFailed(data, failureInfo); }

void
ValidatorRegex::checkPolicy(const Data& data, 
                            int stepCount, 
                            const OnDataValidated &onValidated, 
                            const OnDataValidationFailed &onValidationFailed,
                            vector<shared_ptr<ValidationRequest> > &nextSteps)
{
  if(m_stepLimit == stepCount)
    return onValidationFailed(data.shared_from_this(), 
                              "Maximum steps of validation reached: " + data.getName().toUri());

  RuleList::iterator it = m_mustFailVerify.begin();
  for(; it != m_mustFailVerify.end(); it++)
    if((*it)->satisfy(data))
      return onValidationFailed(data.shared_from_this(),
                                "Comply with mustFail policy: " + data.getName().toUri());

  it = m_verifyPolicies.begin();
  for(; it != m_verifyPolicies.end(); it++)
    {
      if((*it)->satisfy(data))
        {
          try
            {
              SignatureSha256WithRsa sig(data.getSignature());                
            
              Name keyLocatorName = sig.getKeyLocator().getName();
              shared_ptr<const Certificate> trustedCert;
              if(m_trustAnchors.end() == m_trustAnchors.find(keyLocatorName))
                trustedCert = m_certificateCache->getCertificate(keyLocatorName);
              else
                trustedCert = m_trustAnchors[keyLocatorName];
            
              if(static_cast<bool>(trustedCert)){
                if(verifySignature(data, sig, trustedCert->getPublicKeyInfo()))
                  return onValidated(data.shared_from_this());
                else
                  return onValidationFailed(data.shared_from_this(), 
                                            "Cannot verify signature: " + data.getName().toUri());
              }
              else{
                // _LOG_DEBUG("KeyLocator is not trust anchor");                
                OnDataValidated onKeyValidated = bind(&ValidatorRegex::onCertificateValidated, this, 
                                                      _1, data.shared_from_this(), onValidated, onValidationFailed);
              
                OnDataValidationFailed onKeyValidationFailed = bind(&ValidatorRegex::onCertificateValidationFailed, this, 
                                                                    _1, _2, data.shared_from_this(), onValidationFailed);              

                shared_ptr<ValidationRequest> nextStep = make_shared<ValidationRequest>(Interest(boost::cref(sig.getKeyLocator().getName())), 
                                                                                        onKeyValidated,
                                                                                        onKeyValidationFailed,
                                                                                        3,
                                                                                        stepCount + 1);
                nextSteps.push_back(nextStep);

                return;
              }
            }
          catch(SignatureSha256WithRsa::Error &e)
            {
              return onValidationFailed(data.shared_from_this(), 
                                        "Not SignatureSha256WithRsa signature: " + data.getName().toUri());
            }
          catch(KeyLocator::Error &e)
            {
              return onValidationFailed(data.shared_from_this(),
                                        "Key Locator is not a name: " + data.getName().toUri());
            }
        }
    }

  return onValidationFailed(data.shared_from_this(), 
                            "No policy found for data: " + data.getName().toUri());
}

} // namespace ndn
