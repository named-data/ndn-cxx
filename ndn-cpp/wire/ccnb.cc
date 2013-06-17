/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#include "ccnb.h"
#include "ndn-cpp/error.h"

#include <boost/lexical_cast.hpp>

namespace ndn {
namespace wire {

#define CCN_TT_BITS 3
#define CCN_TT_MASK ((1 << CCN_TT_BITS) - 1)
#define CCN_MAX_TINY ((1 << (7-CCN_TT_BITS)) - 1)
#define CCN_TT_HBIT ((unsigned char)(1 << 7))

void
Ccnb::appendBlockHeader (std::ostream &os, size_t val, Ccnb::ccn_tt tt)
{
  unsigned char buf[1+8*((sizeof(val)+6)/7)];
  unsigned char *p = &(buf[sizeof(buf)-1]);
  size_t n = 1;
  p[0] = (CCN_TT_HBIT & ~Ccnb::CCN_CLOSE_TAG) |
  ((val & CCN_MAX_TINY) << CCN_TT_BITS) |
  (CCN_TT_MASK & tt);
  val >>= (7-CCN_TT_BITS);
  while (val != 0) {
    (--p)[0] = (((unsigned char)val) & ~CCN_TT_HBIT) | Ccnb::CCN_CLOSE_TAG;
    n++;
    val >>= 7;
  }
  os.write (reinterpret_cast<const char*> (p), n);
  // return n;
}

void
Ccnb::appendNumber (std::ostream &os, uint32_t number)
{
  std::string numberStr = boost::lexical_cast<std::string> (number);

  appendBlockHeader (os, numberStr.size (), Ccnb::CCN_UDATA);
  numberStr.size ();
  os.write (numberStr.c_str (), numberStr.size ());
}

void
Ccnb::appendName (std::ostream &os, const Name &name)
{
  Ccnb::appendBlockHeader (os, Ccnb::CCN_DTAG_Name, Ccnb::CCN_DTAG); // <Name>
  for (Name::const_iterator component = name.begin (); component != name.end (); component ++)
    {
      appendTaggedBlob (os, Ccnb::CCN_DTAG_Component, component->buf (), component->size ());
    }
  Ccnb::appendCloser (os);                                        // </Name>
}

void
Ccnb::appendTimestampBlob (std::ostream &os, const TimeInterval &time)
{
  // CCNx method function implements some markers, which are not really defined anywhere else...

  // Determine miminal number of bytes required to store the timestamp
  int required_bytes = 2; // 12 bits for fractions of a second, 4 bits left for seconds. Sometimes it is enough
  intmax_t ts = time.total_seconds () >> 4;
  for (;  required_bytes < 7 && ts != 0; ts >>= 8) // not more than 6 bytes?
     required_bytes++;

  appendBlockHeader(os, required_bytes, Ccnb::CCN_BLOB);

  // write part with seconds
  ts = time.total_seconds () >> 4;
  for (int i = 0; i < required_bytes - 2; i++)
    os.put ( ts >> (8 * (required_bytes - 3 - i)) );

  /* arithmetic contortions are to avoid overflowing 31 bits */
  ts = ((time.total_seconds () & 15) << 12) +
    (((time.total_nanoseconds () % 1000000000) / 5 * 8 + 195312) / 390625);
  for (int i = required_bytes - 2; i < required_bytes; i++)
    os.put ( ts >> (8 * (required_bytes - 1 - i)) );

  // return len + required_bytes;
}

void
Ccnb::appendExclude (std::ostream &os, const Exclude &exclude)
{
  appendBlockHeader (os, Ccnb::CCN_DTAG_Exclude, Ccnb::CCN_DTAG); // <Exclude>

  for (Exclude::const_reverse_iterator item = exclude.rbegin (); item != exclude.rend (); item ++)
    {
      if (!item->first.empty ())
        appendTaggedBlob (os, Ccnb::CCN_DTAG_Component, item->first.buf (), item->first.size ());
      if (item->second)
        {
          appendBlockHeader (os, Ccnb::CCN_DTAG_Any, Ccnb::CCN_DTAG); // <Any>
          appendCloser (os); // </Any>
        }
    }
  appendCloser (os); // </Exclude>
}

void
Ccnb::appendInterest (std::ostream &os, const Interest &interest)
{
  Ccnb::appendBlockHeader (os, Ccnb::CCN_DTAG_Interest, Ccnb::CCN_DTAG); // <Interest>

  // this is used for now as an interest template. Name should be empty
  // Ccnb::appendName (os, interest.getName ());
  Ccnb::appendName (os, Name ());                              // <Component>...</Component>...

  if (interest.getMinSuffixComponents () != Interest::ncomps)
    {
      appendTaggedNumber (os, Ccnb::CCN_DTAG_MinSuffixComponents, interest.getMinSuffixComponents ());
    }
  if (interest.getMaxSuffixComponents () != Interest::ncomps)
    {
      appendTaggedNumber (os, Ccnb::CCN_DTAG_MaxSuffixComponents, interest.getMaxSuffixComponents ());
    }
  if (interest.getExclude ().size () > 0)
    {
      appendExclude (os, interest.getExclude ());
    }
  if (interest.getChildSelector () != Interest::CHILD_DEFAULT)
    {
      appendTaggedNumber (os, Ccnb::CCN_DTAG_ChildSelector, interest.getChildSelector ());
    }
  if (interest.getAnswerOriginKind () != Interest::AOK_DEFAULT)
    {
      appendTaggedNumber (os, Ccnb::CCN_DTAG_AnswerOriginKind, interest.getAnswerOriginKind ());
    }
  if (interest.getScope () != Interest::NO_SCOPE)
    {
      appendTaggedNumber (os, Ccnb::CCN_DTAG_Scope, interest.getScope ());
    }
  if (!interest.getInterestLifetime ().is_negative ())
    {
      Ccnb::appendBlockHeader (os, Ccnb::CCN_DTAG_InterestLifetime, Ccnb::CCN_DTAG);
      Ccnb::appendTimestampBlob (os, interest.getInterestLifetime ());
      Ccnb::appendCloser (os);
    }
  // if (GetNonce()>0)
  //   {
  //     uint32_t nonce = interest.GetNonce();
  //     appendTaggedBlob (start, Ccnb::CCN_DTAG_Nonce, nonce);
  //   }

  // if (GetNack ()>0)
  //   {
  //     appendBlockHeader (start, Ccnb::CCN_DTAG_Nack, Ccnb::CCN_DTAG);
  //     appendNumber (start, interest.GetNack ());
  //     appendCloser (start);
  //   }
  Ccnb::appendCloser (os); // </Interest>
}

static void *SIGNATURE_Block = 0;
static void *SINATURE_INFO_PublisherPublicKeyDigest = reinterpret_cast<void *> (1);
static void *SINATURE_INFO_KeyLocator = reinterpret_cast<void *> (2);

static const char TYPES [][3] =  {
  {0x0C, 0x04, 0xC0},
  {0x10, 0xD0, 0x91},
  {0x18, 0xE3, 0x44},
  {0x28, 0x46, 0x3F},
  {0x2C, 0x83, 0x4A},
  {0x34, 0x00, 0x8A}
};

void
Ccnb::appendSignature (std::ostream &os, const signature::Sha256WithRsa &signature, void *userData)
{
  if (userData == SIGNATURE_Block)
    {
      Ccnb::appendBlockHeader (os, Ccnb::CCN_DTAG_Signature, Ccnb::CCN_DTAG); // <Signature>
      // if (signature.getDigestAlgorithm () != "2.16.840.1.101.3.4.2.1")
      //   {
      //     appendString (os, Ccnb::CCN_DTAG_DigestAlgorithm, signature.getDigestAlgorithm ());
      //   }
      appendTaggedBlob (os, Ccnb::CCN_DTAG_SignatureBits, signature.getSignatureBits ());
      Ccnb::appendCloser (os); // </Signature>
    }
  else if (userData == SINATURE_INFO_PublisherPublicKeyDigest)
    {
      Ccnb::appendTaggedBlob (os, Ccnb::CCN_DTAG_PublisherPublicKeyDigest, signature.getPublisherKeyDigest ());
    }
  else if (userData == SINATURE_INFO_KeyLocator)
    {
      Ccnb::appendBlockHeader (os, Ccnb::CCN_DTAG_Signature, Ccnb::CCN_DTAG); // <Signature>
      switch (signature.getKeyLocator ().getType ())
        {
        case KeyLocator::NOTSET:
          break;
        case KeyLocator::KEY:
          Ccnb::appendTaggedBlob (os, Ccnb::CCN_DTAG_Key, signature.getKeyLocator ().getKey ());
          break;
        case KeyLocator::CERTIFICATE:
          Ccnb::appendTaggedBlob (os, Ccnb::CCN_DTAG_Key, signature.getKeyLocator ().getCertificate ());
          break;
        case KeyLocator::KEYNAME:
          Ccnb::appendBlockHeader (os, Ccnb::CCN_DTAG_KeyName, Ccnb::CCN_DTAG); // <KeyName>
          Ccnb::appendName (os, signature.getKeyLocator ().getKeyName ());
          Ccnb::appendCloser (os); // </KeyName>
          break;
        }
      Ccnb::appendCloser (os); // </Signature>
    }
  // other cases should not be possible, but don't do anything
}

void
Ccnb::appendData (std::ostream &os, const Data &data)
{
  if (!data.getSignature ())
    BOOST_THROW_EXCEPTION (error::wire::Ccnb ()
                           << error::msg ("Signature is required, but not set"));

  Ccnb::appendBlockHeader (os, Ccnb::CCN_DTAG_ContentObject, Ccnb::CCN_DTAG); // <ContentObject>

  // necessary for now, because of the changed storage order
  data.getSignature ()->doubleDispatch (os, *this, SIGNATURE_Block);

  Ccnb::appendName (os, data.getName ());

  Ccnb::appendBlockHeader (os, Ccnb::CCN_DTAG_SignedInfo, Ccnb::CCN_DTAG); // <SignedInfo>
  data.getSignature ()->doubleDispatch (os, *this, SINATURE_INFO_PublisherPublicKeyDigest);

  Ccnb::appendTimestampBlob (os, data.getContent ().getTimestamp ());

  BOOST_ASSERT (sizeof (TYPES) == 3 * (static_cast<int> (Content::NACK)+1));
  Ccnb::appendTaggedBlob (os, Ccnb::CCN_DTAG_Type, TYPES [data.getContent ().getType ()], 3);

  if (data.getContent ().getFreshness () != Content::noFreshness)
    {
      Ccnb::appendTaggedNumber (os, Ccnb::CCN_DTAG_FreshnessSeconds,
                                data.getContent ().getFreshness ().total_seconds ());
    }

  if (data.getContent ().getFinalBlockId () != Content::noFinalBlock)
    {
      Ccnb::appendTaggedBlob (os, Ccnb::CCN_DTAG_FinalBlockID, data.getContent ().getFinalBlockId ());
    }

  data.getSignature ()->doubleDispatch (os, *this, SINATURE_INFO_KeyLocator);
  Ccnb::appendCloser (os); // </SignedInfo>

  Ccnb::appendTaggedBlob (os, Ccnb::CCN_DTAG_Content, data.content ());

  Ccnb::appendCloser (os); // </ContentObject>
}

} // namespace wire
} // namespace ndn
