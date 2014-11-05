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
 */

#include "common.hpp"

#include "validator-regex.hpp"
#include "signature-sha256-with-rsa.hpp"
#include "certificate-cache-ttl.hpp"

namespace ndn {

const shared_ptr<CertificateCache> ValidatorRegex::DEFAULT_CERTIFICATE_CACHE;

ValidatorRegex::ValidatorRegex(Face* face,
                               shared_ptr<CertificateCache> certificateCache,
                               const int stepLimit)
  : Validator(face)
  , m_stepLimit(stepLimit)
  , m_certificateCache(certificateCache)
{
  if (!static_cast<bool>(m_certificateCache) && face != nullptr)
    m_certificateCache = make_shared<CertificateCacheTtl>(ref(face->getIoService()));
}

ValidatorRegex::ValidatorRegex(Face& face,
                               shared_ptr<CertificateCache> certificateCache,
                               const int stepLimit)
  : Validator(face)
  , m_stepLimit(stepLimit)
  , m_certificateCache(certificateCache)
{
  if (!static_cast<bool>(m_certificateCache))
    m_certificateCache = make_shared<CertificateCacheTtl>(ref(face.getIoService()));
}

void
ValidatorRegex::addDataVerificationRule(shared_ptr<SecRuleRelative> rule)
{
  rule->isPositive() ? m_verifyPolicies.push_back(rule) : m_mustFailVerify.push_back(rule);
}

void
ValidatorRegex::addTrustAnchor(shared_ptr<IdentityCertificate> certificate)
{
  m_trustAnchors[certificate->getName().getPrefix(-1)] = certificate;
}

void
ValidatorRegex::onCertificateValidated(const shared_ptr<const Data>& signCertificate,
                                       const shared_ptr<const Data>& data,
                                       const OnDataValidated& onValidated,
                                       const OnDataValidationFailed& onValidationFailed)
{
  shared_ptr<IdentityCertificate> certificate =
    make_shared<IdentityCertificate>(*signCertificate);

  if (!certificate->isTooLate() && !certificate->isTooEarly())
    {
      if (static_cast<bool>(m_certificateCache))
        m_certificateCache->insertCertificate(certificate);

      if (verifySignature(*data, certificate->getPublicKeyInfo()))
        return onValidated(data);
      else
        return onValidationFailed(data,
                                  "Cannot verify signature: " +
                                  data->getName().toUri());
    }
  else
    {
      return onValidationFailed(data,
                                "Signing certificate " +
                                signCertificate->getName().toUri() +
                                " is no longer valid.");
    }
}

void
ValidatorRegex::onCertificateValidationFailed(const shared_ptr<const Data>& signCertificate,
                                              const std::string& failureInfo,
                                              const shared_ptr<const Data>& data,
                                              const OnDataValidationFailed& onValidationFailed)
{
  onValidationFailed(data, failureInfo);
}

void
ValidatorRegex::checkPolicy(const Data& data,
                            int nSteps,
                            const OnDataValidated& onValidated,
                            const OnDataValidationFailed& onValidationFailed,
                            std::vector<shared_ptr<ValidationRequest> >& nextSteps)
{
  if (m_stepLimit == nSteps)
    return onValidationFailed(data.shared_from_this(),
                              "Maximum steps of validation reached: " +
                              data.getName().toUri());

  for (RuleList::iterator it = m_mustFailVerify.begin();
       it != m_mustFailVerify.end();
       it++)
    if ((*it)->satisfy(data))
      return onValidationFailed(data.shared_from_this(),
                                "Comply with mustFail policy: " +
                                data.getName().toUri());

  for (RuleList::iterator it = m_verifyPolicies.begin();
       it != m_verifyPolicies.end();
       it++)
    {
      if ((*it)->satisfy(data))
        {
          try
            {
              if (!data.getSignature().hasKeyLocator())
                return onValidationFailed(data.shared_from_this(),
                                          "Key Locator is missing in Data packet: " +
                                          data.getName().toUri());

              const KeyLocator& keyLocator = data.getSignature().getKeyLocator();
              if (keyLocator.getType() != KeyLocator::KeyLocator_Name)
                return onValidationFailed(data.shared_from_this(),
                                          "Key Locator is not a name: " +
                                          data.getName().toUri());


              const Name& keyLocatorName = keyLocator.getName();
              shared_ptr<const Certificate> trustedCert;
              if (m_trustAnchors.end() == m_trustAnchors.find(keyLocatorName) &&
                  static_cast<bool>(m_certificateCache))
                trustedCert = m_certificateCache->getCertificate(keyLocatorName);
              else
                trustedCert = m_trustAnchors[keyLocatorName];

              if (static_cast<bool>(trustedCert))
                {
                  if (verifySignature(data, data.getSignature(), trustedCert->getPublicKeyInfo()))
                    return onValidated(data.shared_from_this());
                  else
                    return onValidationFailed(data.shared_from_this(),
                                              "Cannot verify signature: " +
                                              data.getName().toUri());
                }
              else
                {
                  // KeyLocator is not a trust anchor

                  OnDataValidated onKeyValidated =
                    bind(&ValidatorRegex::onCertificateValidated, this, _1,
                         data.shared_from_this(), onValidated, onValidationFailed);

                  OnDataValidationFailed onKeyValidationFailed =
                    bind(&ValidatorRegex::onCertificateValidationFailed, this, _1, _2,
                         data.shared_from_this(), onValidationFailed);

                  Interest interest(keyLocatorName);
                  shared_ptr<ValidationRequest> nextStep =
                    make_shared<ValidationRequest>(interest,
                                                   onKeyValidated,
                                                   onKeyValidationFailed,
                                                   3,
                                                   nSteps + 1);

                  nextSteps.push_back(nextStep);

                  return;
                }
            }
          catch (KeyLocator::Error& e)
            {
              return onValidationFailed(data.shared_from_this(),
                                        "Key Locator is not a name: " +
                                        data.getName().toUri());
            }
          catch (tlv::Error& e)
            {
              return onValidationFailed(data.shared_from_this(),
                                        "Cannot decode signature");
            }
        }
    }

  return onValidationFailed(data.shared_from_this(),
                            "No policy found for data: " + data.getName().toUri());
}

} // namespace ndn
