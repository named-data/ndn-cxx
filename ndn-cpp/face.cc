/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#include "face.h"

namespace ndn {

Face::sent_interest
Face::sendInterest (Ptr<const Interest> interest, const SatisfiedInterestCallback &dataCallback)
{
  // magic to be done in child class

  sent_interest item = m_sentInterests.find_exact (interest->getName ());
  if (item == m_sentInterests.end ())
    {
      std::pair<sent_interest, bool> insertRes =
        m_sentInterests.insert (interest->getName (), Ptr<sent_interest_container::payload_traits::payload_type>::Create ());

      item = insertRes.first;
    }
  item->payload ()->push_back (dataCallback);

  return item;
}

void
Face::clearInterest (Face::sent_interest interest)
{
  m_sentInterests.erase (interest);
}
  
Face::registered_prefix
Face::setInterestFilter (Ptr<const Name> prefix, const ExpectedInterestCallback &interestCallback)
{
  // magic to be done in child class
  
  registered_prefix item = m_registeredPrefixes.find_exact (*prefix);
  if (item == m_registeredPrefixes.end ())
    {
      std::pair<registered_prefix, bool> insertRes =
        m_registeredPrefixes.insert (*prefix, Ptr<registered_prefix_container::payload_traits::payload_type>::Create ());

      item = insertRes.first;
    }
  item->payload ()->push_back (interestCallback);

  return item;
}

void
Face::clearInterestFilter (const Name &prefix)
{
  registered_prefix item = m_registeredPrefixes.find_exact (prefix);
  if (item == m_registeredPrefixes.end ())
    return;

  clearInterestFilter (item);
}

void
Face::clearInterestFilter (Face::registered_prefix filter)
{
  m_registeredPrefixes.erase (filter);
}

} // ndn

// void
// CcnxWrapper::OnInterest (const Ptr<const ndn::Interest> &interest, Ptr<Packet> packet)
// {
//   ndn::App::OnInterest (interest, packet);

//   // the app cannot set several filters for the same prefix
//   CcnxFilterEntryContainer<InterestCallback>::iterator entry = m_interestCallbacks.longest_prefix_match (interest->GetName ());
//   if (entry == m_interestCallbacks.end ())
//     {
//       _LOG_DEBUG ("No Interest callback set");
//       return;
//     }
  
//   entry->payload ()->m_callback (lexical_cast<string> (interest->GetName ()));  
// }

// void
// CcnxWrapper::OnContentObject (const Ptr<const ndn::ContentObject> &contentObject,
//                               Ptr<Packet> payload)
// {
//   ndn::App::OnContentObject (contentObject, payload);
//   NS_LOG_DEBUG ("<< D " << contentObject->GetName ());

//   CcnxFilterEntryContainer<RawDataCallback>::iterator entry = m_dataCallbacks.longest_prefix_match (contentObject->GetName ());
//   if (entry == m_dataCallbacks.end ())
//     {
//       _LOG_DEBUG ("No Data callback set");
//       return;
//     }

//   while (entry != m_dataCallbacks.end ())
//     {
//       ostringstream content;
//       payload->CopyData (&content, payload->GetSize ());
  
//       entry->payload ()->m_callback (lexical_cast<string> (contentObject->GetName ()), content.str ().c_str (), content.str ().size ());
  
//       m_dataCallbacks.erase (entry);

//       entry = m_dataCallbacks.longest_prefix_match (contentObject->GetName ());
//     }
// }

// }
