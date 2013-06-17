/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef LRU_POLICY_H_
#define LRU_POLICY_H_

#include <boost/intrusive/options.hpp>
#include <boost/intrusive/list.hpp>

namespace ndn {
namespace trie {

/**
 * @brief Traits for Least Recently Used replacement policy
 */
struct lru_policy_traits
{
  /// @brief Name that can be used to identify the policy (e.g., for logging)
  static std::string GetName () { return "Lru"; }
  
  struct policy_hook_type : public boost::intrusive::list_member_hook<> {};

  template<class Container>
  struct container_hook
  {
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
        , max_size_ (100)
      {
      }

      inline void
      update (typename parent_trie::iterator item)
      {
        // do relocation
        policy_container::splice (policy_container::end (),
                                  *this,
                                  policy_container::s_iterator_to (*item));
      }
  
      inline bool
      insert (typename parent_trie::iterator item)
      {
        if (max_size_ != 0 && policy_container::size () >= max_size_)
          {
            base_.erase (&(*policy_container::begin ()));
          }
      
        policy_container::push_back (*item);
        return true;
      }
  
      inline void
      lookup (typename parent_trie::iterator item)
      {
        // do relocation
        policy_container::splice (policy_container::end (),
                                  *this,
                                  policy_container::s_iterator_to (*item));
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

      inline void
      set_max_size (size_t max_size)
      {
        max_size_ = max_size;
      }

      inline size_t
      get_max_size () const
      {
        return max_size_;
      }

    private:
      type () : base_(*((Base*)0)) { };

    private:
      Base &base_;
      size_t max_size_;
    };
  };
};

} // trie
} // ndn

#endif
