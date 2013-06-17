/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_TRIE_TRIE_H_
#define NDN_TRIE_TRIE_H_

#include <boost/intrusive/unordered_set.hpp>
#include <boost/intrusive/list.hpp>
#include <boost/intrusive/set.hpp>
#include <boost/functional/hash.hpp>
#include <boost/interprocess/smart_ptr/unique_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/foreach.hpp>
#include <boost/mpl/if.hpp>

#include "payload-traits/pointer.h"
#include "payload-traits/ptr.h"

namespace ndn {
namespace trie {

////////////////////////////////////////////////////
// forward declarations
//
template<typename FullKey,
         typename PayloadTraits,
         typename PolicyHook >
class trie;

template<typename FullKey, typename PayloadTraits, typename PolicyHook>
inline std::ostream&
operator << (std::ostream &os,
             const trie<FullKey, PayloadTraits, PolicyHook> &trie_node);

template<typename FullKey, typename PayloadTraits, typename PolicyHook>
bool
operator== (const trie<FullKey, PayloadTraits, PolicyHook> &a,
            const trie<FullKey, PayloadTraits, PolicyHook> &b);

template<typename FullKey, typename PayloadTraits, typename PolicyHook >
std::size_t
hash_value (const trie<FullKey, PayloadTraits, PolicyHook> &trie_node);

///////////////////////////////////////////////////
// actual definition
//
template<class T, class NonConstT>
class trie_iterator;

template<class T>
class trie_point_iterator;

template<typename FullKey,
	 typename PayloadTraits,
         typename PolicyHook >
class trie
{
public:
  typedef typename FullKey::partial_type Key;

  typedef trie*       iterator;
  typedef const trie* const_iterator;

  typedef trie_iterator<trie, trie> recursive_iterator;
  typedef trie_iterator<const trie, trie> const_recursive_iterator;

  typedef trie_point_iterator<trie> point_iterator;
  typedef trie_point_iterator<const trie> const_point_iterator;

  typedef PayloadTraits payload_traits;

  inline
  trie (const Key &key, size_t bucketSize = 10, size_t bucketIncrement = 10)
    : key_ (key)
    , initialBucketSize_ (bucketSize)
    , bucketIncrement_ (bucketIncrement)
    , bucketSize_ (initialBucketSize_)
    , buckets_ (new bucket_type [bucketSize_]) //cannot use normal pointer, because lifetime of buckets should be larger than lifetime of the container
    , children_ (bucket_traits (buckets_.get (), bucketSize_))
    , payload_ (PayloadTraits::empty_payload)
    , parent_ (0)
  {
  }

  inline
  ~trie ()
  {
    payload_ = PayloadTraits::empty_payload; // necessary for smart pointers...
    children_.clear_and_dispose (trie_delete_disposer ());
  }

  void
  clear ()
  {
    children_.clear_and_dispose (trie_delete_disposer ());
  }

  template<class Predicate>
  void
  clear_if (Predicate cond)
  {
    recursive_iterator trieNode (this);
    recursive_iterator end (0);

    while (trieNode != end)
      {
        if (cond (*trieNode))
          {
            trieNode = recursive_iterator (trieNode->erase ());
          }
        trieNode ++;
      }
  }

  // actual entry
  friend bool
  operator== <> (const trie<FullKey, PayloadTraits, PolicyHook> &a,
                 const trie<FullKey, PayloadTraits, PolicyHook> &b);

  friend std::size_t
  hash_value <> (const trie<FullKey, PayloadTraits, PolicyHook> &trie_node);

  inline std::pair<iterator, bool>
  insert (const FullKey &key,
          typename PayloadTraits::insert_type payload)
  {
    trie *trieNode = this;

    BOOST_FOREACH (const Key &subkey, key)
      {
        typename unordered_set::iterator item = trieNode->children_.find (subkey);
        if (item == trieNode->children_.end ())
          {
            trie *newNode = new trie (subkey, initialBucketSize_, bucketIncrement_);
            // std::cout << "new " << newNode << "\n";
            newNode->parent_ = trieNode;

            if (trieNode->children_.size () >= trieNode->bucketSize_)
              {
                trieNode->bucketSize_ += trieNode->bucketIncrement_;
                trieNode->bucketIncrement_ *= 2; // increase bucketIncrement exponentially

                buckets_array newBuckets (new bucket_type [trieNode->bucketSize_]);
                trieNode->children_.rehash (bucket_traits (newBuckets.get (), trieNode->bucketSize_));
                trieNode->buckets_.swap (newBuckets);
              }

            std::pair< typename unordered_set::iterator, bool > ret =
              trieNode->children_.insert (*newNode);

            trieNode = &(*ret.first);
          }
        else
          trieNode = &(*item);
      }

    if (trieNode->payload_ == PayloadTraits::empty_payload)
      {
        trieNode->payload_ = payload;
        return std::make_pair (trieNode, true);
      }
    else
      return std::make_pair (trieNode, false);
  }

  /**
   * @brief Removes payload (if it exists) and if there are no children, prunes parents trie
   */
  inline iterator
  erase ()
  {
    payload_ = PayloadTraits::empty_payload;
    return prune ();
  }

  /**
   * @brief Do exactly as erase, but without erasing the payload
   */
  inline iterator
  prune ()
  {
    if (payload_ == PayloadTraits::empty_payload &&
        children_.size () == 0)
      {
        if (parent_ == 0) return this;

        trie *parent = parent_;
        parent->children_.erase_and_dispose (*this, trie_delete_disposer ()); // delete this; basically, committing a suicide

        return parent->prune ();
      }
    return this;
  }

  /**
   * @brief Perform prune of the node, but without attempting to parent of the node
   */
  inline void
  prune_node ()
  {
    if (payload_ == PayloadTraits::empty_payload &&
        children_.size () == 0)
      {
        if (parent_ == 0) return;

        trie *parent = parent_;
        parent->children_.erase_and_dispose (*this, trie_delete_disposer ()); // delete this; basically, committing a suicide
      }
  }

  // inline boost::tuple<const iterator, bool, const iterator>
  // find (const FullKey &key) const
  // {
  //   return const_cast<trie*> (this)->find (key);
  // }

  /**
   * @brief Perform the longest prefix match
   * @param key the key for which to perform the longest prefix match
   *
   * @return ->second is true if prefix in ->first is longer than key
   */
  inline boost::tuple<iterator, bool, iterator>
  find (const FullKey &key)
  {
    trie *trieNode = this;
    iterator foundNode = (payload_ != PayloadTraits::empty_payload) ? this : 0;
    bool reachLast = true;

    BOOST_FOREACH (const Key &subkey, key)
      {
        typename unordered_set::iterator item = trieNode->children_.find (subkey);
        if (item == trieNode->children_.end ())
          {
            reachLast = false;
            break;
          }
        else
          {
            trieNode = &(*item);

            if (trieNode->payload_ != PayloadTraits::empty_payload)
              foundNode = trieNode;
          }
      }

    return boost::make_tuple (foundNode, reachLast, trieNode);
  }

  /**
   * @brief Perform the longest prefix match satisfying preficate
   * @param key the key for which to perform the longest prefix match
   *
   * @return ->second is true if prefix in ->first is longer than key
   */
  template<class Predicate>
  inline boost::tuple<iterator, bool, iterator>
  find_if (const FullKey &key, Predicate pred)
  {
    trie *trieNode = this;
    iterator foundNode = (payload_ != PayloadTraits::empty_payload) ? this : 0;
    bool reachLast = true;

    BOOST_FOREACH (const Key &subkey, key)
      {
        typename unordered_set::iterator item = trieNode->children_.find (subkey);
        if (item == trieNode->children_.end ())
          {
            reachLast = false;
            break;
          }
        else
          {
            trieNode = &(*item);

            if (trieNode->payload_ != PayloadTraits::empty_payload &&
                pred (trieNode->payload_))
              {
                foundNode = trieNode;
              }
          }
      }

    return boost::make_tuple (foundNode, reachLast, trieNode);
  }

  /**
   * @brief Find next payload of the sub-trie
   * @returns end() or a valid iterator pointing to the trie leaf (order is not defined, enumeration )
   */
  inline iterator
  find ()
  {
    if (payload_ != PayloadTraits::empty_payload)
      return this;

    typedef trie<FullKey, PayloadTraits, PolicyHook> trie;
    for (typename trie::unordered_set::iterator subnode = children_.begin ();
         subnode != children_.end ();
         subnode++ )
      // BOOST_FOREACH (trie &subnode, children_)
      {
        iterator value = subnode->find ();
        if (value != 0)
          return value;
      }

    return 0;
  }

  /**
   * @brief Find next payload of the sub-trie satisfying the predicate
   * @param pred predicate
   * @returns end() or a valid iterator pointing to the trie leaf (order is not defined, enumeration )
   */
  template<class Predicate>
  inline const iterator
  find_if (Predicate pred)
  {
    if (payload_ != PayloadTraits::empty_payload && pred (payload_))
      return this;

    typedef trie<FullKey, PayloadTraits, PolicyHook> trie;
    for (typename trie::unordered_set::iterator subnode = children_.begin ();
         subnode != children_.end ();
         subnode++ )
      // BOOST_FOREACH (const trie &subnode, children_)
      {
        iterator value = subnode->find_if (pred);
        if (value != 0)
          return value;
      }

    return 0;
  }

  iterator end ()
  {
    return 0;
  }

  const_iterator end () const
  {
    return 0;
  }

  typename PayloadTraits::const_return_type
  payload () const
  {
    return payload_;
  }

  typename PayloadTraits::return_type
  payload ()
  {
    return payload_;
  }

  void
  set_payload (typename PayloadTraits::insert_type payload)
  {
    payload_ = payload;
  }

  Key key () const
  {
    return key_;
  }

  inline void
  PrintStat (std::ostream &os) const;

private:
  //The disposer object function
  struct trie_delete_disposer
  {
    void operator() (trie *delete_this)
    {
      delete delete_this;
    }
  };

  template<class D>
  struct array_disposer
  {
    void operator() (D *array)
    {
      delete [] array;
    }
  };

  friend
  std::ostream&
  operator<< < > (std::ostream &os, const trie &trie_node);

public:
  PolicyHook policy_hook_;

private:
  boost::intrusive::unordered_set_member_hook<> unordered_set_member_hook_;

  // necessary typedefs
  typedef trie self_type;
  typedef boost::intrusive::member_hook< trie,
                                         boost::intrusive::unordered_set_member_hook< >,
                                         &trie::unordered_set_member_hook_ > member_hook;

  typedef boost::intrusive::unordered_set< trie, member_hook > unordered_set;
  typedef typename unordered_set::bucket_type   bucket_type;
  typedef typename unordered_set::bucket_traits bucket_traits;

  template<class T, class NonConstT>
  friend class trie_iterator;

  template<class T>
  friend class trie_point_iterator;

  ////////////////////////////////////////////////
  // Actual data
  ////////////////////////////////////////////////

  Key key_; ///< name component

  size_t initialBucketSize_;
  size_t bucketIncrement_;

  size_t bucketSize_;
  typedef boost::interprocess::unique_ptr< bucket_type, array_disposer<bucket_type> > buckets_array;
  buckets_array buckets_;
  unordered_set children_;

  typename PayloadTraits::storage_type payload_;
  trie *parent_; // to make cleaning effective
};




template<typename FullKey, typename PayloadTraits, typename PolicyHook>
inline std::ostream&
operator << (std::ostream &os, const trie<FullKey, PayloadTraits, PolicyHook> &trie_node)
{
  os << "# " << trie_node.key_ << ((trie_node.payload_ != PayloadTraits::empty_payload)?"*":"") << std::endl;
  typedef trie<FullKey, PayloadTraits, PolicyHook> trie;

  for (typename trie::unordered_set::const_iterator subnode = trie_node.children_.begin ();
       subnode != trie_node.children_.end ();
       subnode++ )
  // BOOST_FOREACH (const trie &subnode, trie_node.children_)
    {
      os << "\"" << &trie_node << "\"" << " [label=\"" << trie_node.key_ << ((trie_node.payload_ != PayloadTraits::empty_payload)?"*":"") << "\"]\n";
      os << "\"" << &(*subnode) << "\"" << " [label=\"" << subnode->key_ << ((subnode->payload_ != PayloadTraits::empty_payload)?"*":"") << "\"]""\n";

      os << "\"" << &trie_node << "\"" << " -> " << "\"" << &(*subnode) << "\"" << "\n";
      os << *subnode;
    }

  return os;
}

template<typename FullKey, typename PayloadTraits, typename PolicyHook>
inline void
trie<FullKey, PayloadTraits, PolicyHook>
::PrintStat (std::ostream &os) const
{
  os << "# " << key_ << ((payload_ != PayloadTraits::empty_payload)?"*":"") << ": " << children_.size() << " children" << std::endl;
  for (size_t bucket = 0, maxbucket = children_.bucket_count ();
       bucket < maxbucket;
       bucket++)
    {
      os << " " << children_.bucket_size (bucket);
    }
  os << "\n";

  typedef trie<FullKey, PayloadTraits, PolicyHook> trie;
  for (typename trie::unordered_set::const_iterator subnode = children_.begin ();
       subnode != children_.end ();
       subnode++ )
  // BOOST_FOREACH (const trie &subnode, children_)
    {
      subnode->PrintStat (os);
    }
}


template<typename FullKey, typename PayloadTraits, typename PolicyHook>
inline bool
operator == (const trie<FullKey, PayloadTraits, PolicyHook> &a,
             const trie<FullKey, PayloadTraits, PolicyHook> &b)
{
  return a.key_ == b.key_;
}

template<typename FullKey, typename PayloadTraits, typename PolicyHook>
inline std::size_t
hash_value (const trie<FullKey, PayloadTraits, PolicyHook> &trie_node)
{
  return boost::hash_value (trie_node.key_);
}



template<class Trie, class NonConstTrie> // hack for boost < 1.47
class trie_iterator
{
public:
  trie_iterator () : trie_ (0) {}
  trie_iterator (typename Trie::iterator item) : trie_ (item) {}
  trie_iterator (Trie &item) : trie_ (&item) {}

  Trie & operator* () { return *trie_; }
  const Trie & operator* () const { return *trie_; }
  Trie * operator-> () { return trie_; }
  const Trie * operator-> () const { return trie_; }
  bool operator== (trie_iterator<const Trie, NonConstTrie> &other) const { return (trie_ == other.trie_); }
  bool operator== (trie_iterator<Trie, NonConstTrie> &other) { return (trie_ == other.trie_); }
  bool operator!= (trie_iterator<const Trie, NonConstTrie> &other) const { return !(*this == other); }
  bool operator!= (trie_iterator<Trie, NonConstTrie> &other) { return !(*this == other); }

  trie_iterator<Trie,NonConstTrie> &
  operator++ (int)
  {
    if (trie_->children_.size () > 0)
      trie_ = &(*trie_->children_.begin ());
    else
      trie_ = goUp ();
    return *this;
  }

  trie_iterator<Trie,NonConstTrie> &
  operator++ ()
  {
    (*this)++;
    return *this;
  }

private:
  typedef typename boost::mpl::if_< boost::is_same<Trie, NonConstTrie>,
                                    typename Trie::unordered_set::iterator,
                                    typename Trie::unordered_set::const_iterator>::type set_iterator;

  Trie* goUp ()
  {
    if (trie_->parent_ != 0)
      {
        // typename Trie::unordered_set::iterator item =
        set_iterator item = const_cast<NonConstTrie*>(trie_)->parent_->children_.iterator_to (const_cast<NonConstTrie&> (*trie_));
        item++;
        if (item != trie_->parent_->children_.end ())
          {
            return &(*item);
          }
        else
          {
            trie_ = trie_->parent_;
            return goUp ();
          }
      }
    else
      return 0;
  }
private:
  Trie *trie_;
};


template<class Trie>
class trie_point_iterator
{
private:
  typedef typename boost::mpl::if_< boost::is_same<Trie, const Trie>,
                                    typename Trie::unordered_set::const_iterator,
                                    typename Trie::unordered_set::iterator>::type set_iterator;

public:
  trie_point_iterator () : trie_ (0) {}
  trie_point_iterator (typename Trie::iterator item) : trie_ (item) {}
  trie_point_iterator (Trie &item)
  {
    if (item.children_.size () != 0)
      trie_ = &*item.children_.begin ();
    else
      trie_ = 0;
  }

  Trie & operator* () { return *trie_; }
  const Trie & operator* () const { return *trie_; }
  Trie * operator-> () { return trie_; }
  const Trie * operator-> () const { return trie_; }
  bool operator== (trie_point_iterator<const Trie> &other) const { return (trie_ == other.trie_); }
  bool operator== (trie_point_iterator<Trie> &other) { return (trie_ == other.trie_); }
  bool operator!= (trie_point_iterator<const Trie> &other) const { return !(*this == other); }
  bool operator!= (trie_point_iterator<Trie> &other) { return !(*this == other); }

  trie_point_iterator<Trie> &
  operator++ (int)
  {
    if (trie_->parent_ != 0)
      {
        set_iterator item = trie_->parent_->children_.iterator_to (*trie_);
        item ++;
        if (item == trie_->parent_->children_.end ())
          trie_ = 0;
        else
          trie_ = &*item;
      }
    else
      {
        trie_ = 0;
      }
    return *this;
  }

  trie_point_iterator<Trie> &
  operator++ ()
  {
    (*this)++;
    return *this;
  }

private:
  Trie *trie_;
};


} // trie
} // ndn

#endif // NDN_TRIE_TRIE_H_
