/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2017 Regents of the University of California.
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
 */

#ifndef NDN_SECURITY_V2_CERTIFICATE_FETCHER_HPP
#define NDN_SECURITY_V2_CERTIFICATE_FETCHER_HPP

#include "certificate-request.hpp"
#include "certificate-storage.hpp"
#include "validation-state.hpp"

namespace ndn {

class Face;

namespace security {
namespace v2 {

/**
 * @brief Interface used by the validator to fetch missing certificates
 */
class CertificateFetcher : noncopyable
{
public:
  using ValidationContinuation = std::function<void(const Certificate& cert,
                                                    const shared_ptr<ValidationState>& state)>;

  CertificateFetcher();

  virtual
  ~CertificateFetcher();

  /**
   * @brief Assign certificate storage to check known certificate and to cache unverified ones
   * @note The supplied @p certStorage should be valid for the lifetime of CertificateFetcher
   */
  virtual void
  setCertificateStorage(CertificateStorage& certStorage);

  /**
   * @brief Asynchronously fetch certificate
   * @pre m_certStorage != nullptr
   *
   * If the requested certificate exists in the storage, then this method will immediately call
   * continueValidation with the certification.  If certificate is not available, the
   * implementation-specific doFetch will be called to asynchronously fetch certificate.  The
   * successfully retrieved certificate will be automatically added to the unverified cache of
   * the certificate storage.
   *
   * When the requested certificate is retrieved, continueValidation is called.  Otherwise, the
   * fetcher implementation call state->failed() with the appropriate error code and diagnostic
   * message.
   */
  void
  fetch(const shared_ptr<CertificateRequest>& certRequest, const shared_ptr<ValidationState>& state,
        const ValidationContinuation& continueValidation);

private:
  /**
   * @brief Asynchronous certificate fetching implementation
   */
  virtual void
  doFetch(const shared_ptr<CertificateRequest>& certRequest, const shared_ptr<ValidationState>& state,
          const ValidationContinuation& continueValidation) = 0;

protected:
  CertificateStorage* m_certStorage;
};

} // namespace v2
} // namespace security
} // namespace ndn

#endif // NDN_SECURITY_V2_CERTIFICATE_FETCHER_HPP
