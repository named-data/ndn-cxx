/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef COUNTING_POLICY_H_
#define COUNTING_POLICY_H_

#include <boost/intrusive/options.hpp>
#include <boost/intrusive/list.hpp>

namespace ndn {
namespace trie {

/**
 * @brief Traits for policy that just keeps track of number of elements
 * It's doing a rather expensive job, but just in case it needs to be extended later
 */
struct counting_policy_traits
{
  /// @brief Name that can be used to identify the policy (e.g., for logging)
  static std::string GetName () { return "Counting"; }

  struct policy_hook_type : public boost::intrusive::list_member_hook<> {};

  template<class Container>
  struct container_hook
  {
    // could be class/struct implementation
    typedef boost::intrusive::member_hook< Container,
                             policy_hook_type,
                             &Container::policy_hook_ > type;
  };

  template<class Base,
           class Container,
           class Hook>
  struct policy 
  {
    typedef typename boost::intrusive::list< Container, Hook > policy_container;
    
    // could be just typedef
    class type : public policy_container
    {
    public:
      typedef Container parent_trie;

      type (Base &base)
        : base_ (base)
      {
      }

      inline void
      update (typename parent_trie::iterator item)
      {
        // do nothing
      }
  
      inline bool
      insert (typename parent_trie::iterator item)
      {
        policy_container::push_back (*item);
        return true;
      }
  
      inline void
      lookup (typename parent_trie::iterator item)
      {
        // do nothing
      }
  
      inline void
      erase (typename parent_trie::iterator item)
      {
        policy_container::erase (policy_container::s_iterator_to (*item));
      }

      inline void
      clear ()
      {
        policy_container::clear ();
      }

    private:
      type () : base_(*((Base*)0)) { };

    private:
      Base &base_;
    };
  };
};

} // trie
} // ndn

#endif // COUNTING_POLICY_H_
