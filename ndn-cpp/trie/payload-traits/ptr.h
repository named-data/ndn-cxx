/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_TRIE_PAYLOAD_TRAITS_PTR_H
#define NDN_TRIE_PAYLOAD_TRAITS_PTR_H

namespace ndn {
namespace trie {

template<typename Payload, typename BasePayload = Payload>
struct ptr_payload_traits
{
  typedef Payload            payload_type;
  typedef Ptr<Payload>       storage_type;
  typedef Ptr<Payload>       insert_type;

  typedef Ptr<Payload>       return_type;
  typedef Ptr<const Payload> const_return_type;

  typedef Ptr<BasePayload> base_type;
  typedef Ptr<const BasePayload> const_base_type;

  static Ptr<Payload> empty_payload;
};

template<typename Payload, typename BasePayload>
Ptr<Payload>
ptr_payload_traits<Payload, BasePayload>::empty_payload; // = Ptr<Payload> ();

} // trie
} // ndn

#endif // NDN_TRIE_PAYLOAD_TRAITS_PTR_H
