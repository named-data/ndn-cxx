/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef EMPTY_POLICY_H_
#define EMPTY_POLICY_H_

namespace ndn {
namespace trie {

/**
 * @brief Traits for empty (bogus) replacement policy
 */
struct empty_policy_traits
{
  /// @brief Name that can be used to identify the policy (e.g., for logging)
  static std::string GetName () { return ""; }

  typedef void* policy_hook_type;

  template<class Container> struct container_hook { typedef void* type; };

  template<class Base,
           class Container,
           class Hook>
  struct policy 
  {
    struct type
    {
      inline type (Base &base) {}
      
      inline void update (typename Container::iterator) { }
      inline bool insert (typename Container::iterator) { return true; }
      inline void lookup (typename Container::iterator item) { }
      inline void erase (typename Container::iterator item) { }
      inline void clear () { }
    };
  };
};

} // trie
} // ndn

#endif // EMPTY_POLICY_H_
