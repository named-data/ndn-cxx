/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_VERIFIER_HPP
#define NDN_VERIFIER_HPP

#include "../data.hpp"
#include "../face.hpp"
#include "sec-policy.hpp"
#include "validation-request.hpp"
#include "public-key.hpp"
#include "signature-sha256-with-rsa.hpp"

namespace ndn {
  
/**
 * Verifier is one of the main classes of the security librar .
 *
 * The Verifier class provides the interfaces for packet verification.
 */
class Verifier {
public:
  struct Error : public std::runtime_error { Error(const std::string &what) : std::runtime_error(what) {} };

  Verifier(const ptr_lib::shared_ptr<SecPolicy> &policy = DefaultPolicy);

  /**
   * @brief Set the Face which will be used to fetch required certificates.
   * @param face A pointer to the Face object.
   *
   * Setting face is necessary for verifier operation that involve fetching data.
   */
  void
  setFace(const ptr_lib::shared_ptr<Face> &face) { face_ = face; }
  
  /**
   * @brief Get the policy.
   * @return The Policy.
   */
  inline SecPolicy&
  policy()
  {
    if (!policy_)
      throw Error("policy is not assigned to the KeyChain");

    return *policy_;
  }


  /**
   * Check the signature on the Data object and call either onVerify or onVerifyFailed. 
   * We use callback functions because verify may fetch information to check the signature.
   * @param data The Data object with the signature to check. It is an error if data does not have a wireEncoding. 
   * To set the wireEncoding, you can call data.wireDecode.
   * @param onVerified If the signature is verified, this calls onVerified(data).
   * @param onVerifyFailed If the signature check fails, this calls onVerifyFailed(data).
   */
  void
  verifyData
    (const ptr_lib::shared_ptr<Data>& data, const OnVerified& onVerified, const OnVerifyFailed& onVerifyFailed, int stepCount = 0);

  /*****************************************
   *      verifySignature method set       *
   *****************************************/
    static bool
  verifySignature(const Data &data, const Signature &sig, const PublicKey &publicKey);

  static bool
  verifySignature(const Buffer &data, const Signature &sig, const PublicKey &publicKey);

  static bool
  verifySignature(const Data& data, const SignatureSha256WithRsa& sig, const PublicKey& publicKey);

  static bool
  verifySignature(const Buffer &data, const SignatureSha256WithRsa &sig, const PublicKey &publicKey);

public:
  static const ptr_lib::shared_ptr<SecPolicy>     DefaultPolicy;
    
private:
  void
  onCertificateData
    (const ptr_lib::shared_ptr<const Interest> &interest, const ptr_lib::shared_ptr<Data> &data, ptr_lib::shared_ptr<ValidationRequest> nextStep);
  
  void
  onCertificateInterestTimeout
    (const ptr_lib::shared_ptr<const Interest> &interest, int retry, const OnVerifyFailed& onVerifyFailed, 
     const ptr_lib::shared_ptr<Data> &data, ptr_lib::shared_ptr<ValidationRequest> nextStep);

private:
  ptr_lib::shared_ptr<SecPolicy>     policy_;
  ptr_lib::shared_ptr<Face>        face_;
};

}

#endif
