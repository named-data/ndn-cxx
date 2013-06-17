/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_TRIE_PAYLOAD_TRAITS_POINTER_H
#define NDN_TRIE_PAYLOAD_TRAITS_POINTER_H

namespace ndn {
namespace trie {

template<typename Payload, typename BasePayload = Payload>
struct pointer_payload_traits
{
  typedef Payload         payload_type; // general type of the payload
  typedef Payload*        storage_type; // how the payload is actually stored
  typedef Payload*        insert_type;  // what parameter is inserted

  typedef Payload*        return_type;  // what is returned on access
  typedef const Payload*  const_return_type; // what is returned on const access

  typedef BasePayload*       base_type;       // base type of the entry (when implementation details need to be hidden)
  typedef const BasePayload* const_base_type; // const base type of the entry (when implementation details need to be hidden)

  static Payload* empty_payload;
};

template<typename Payload, typename BasePayload>
Payload*
pointer_payload_traits<Payload, BasePayload>::empty_payload = NULL;

} // trie
} // ndn

#endif // NDN_TRIE_PAYLOAD_TRAITS_POINTER_H
