/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SECURITY_SIGNATURE_SHA256_WITH_RSA_HPP
#define NDN_SECURITY_SIGNATURE_SHA256_WITH_RSA_HPP

#include "../data.hpp"
#include "../encoding/tlv.hpp"

namespace ndn {

/**
 * Representing of SHA256-with-RSA signature in a data packet.
 */
class SignatureSha256WithRsa : public Signature {
public:
  SignatureSha256WithRsa()
  {
    m_info = Block(Tlv::SignatureInfo);

    m_type = Signature::Sha256WithRsa;
    m_info.push_back(nonNegativeIntegerBlock(Tlv::SignatureType, Tlv::SignatureSha256WithRsa));
    m_info.push_back(m_keyLocator.wireEncode());
  }

  SignatureSha256WithRsa(const Signature& signature)
    : Signature(signature)
  {
    if (getType() != Signature::Sha256WithRsa)
      throw Signature::Error("Incorrect signature type");

    m_info.parse();
    Block::element_const_iterator i = m_info.find(Tlv::KeyLocator);
    if (i != m_info.elements_end())
      {
        m_keyLocator.wireDecode(*i);
      }
  }

  const KeyLocator&
  getKeyLocator() const
  {
    return m_keyLocator;
  }

  void
  setKeyLocator(const KeyLocator& keyLocator)
  {
    m_keyLocator = keyLocator;

    m_info.remove(ndn::Tlv::KeyLocator);
    m_info.push_back(m_keyLocator.wireEncode());
  }

private:
  KeyLocator m_keyLocator;
};

} // namespace ndn

#endif //NDN_SECURITY_SIGNATURE_SHA256_WITH_RSA_HPP
