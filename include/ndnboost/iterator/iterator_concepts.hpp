// (C) Copyright Jeremy Siek 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NDNBOOST_ITERATOR_CONCEPTS_HPP
#define NDNBOOST_ITERATOR_CONCEPTS_HPP

#include <ndnboost/concept_check.hpp>
#include <ndnboost/iterator/iterator_categories.hpp>

// Use ndnboost::detail::iterator_traits to work around some MSVC/Dinkumware problems.
#include <ndnboost/detail/iterator.hpp>

#include <ndnboost/type_traits/is_same.hpp>
#include <ndnboost/type_traits/is_integral.hpp>

#include <ndnboost/mpl/bool.hpp>
#include <ndnboost/mpl/if.hpp>
#include <ndnboost/mpl/and.hpp>
#include <ndnboost/mpl/or.hpp>

#include <ndnboost/static_assert.hpp>

// Use ndnboost/limits to work around missing limits headers on some compilers
#include <ndnboost/limits.hpp>
#include <ndnboost/config.hpp>

#include <algorithm>

#include <ndnboost/concept/detail/concept_def.hpp>

namespace ndnboost_concepts
{
  // Used a different namespace here (instead of "boost") so that the
  // concept descriptions do not take for granted the names in
  // namespace ndnboost.

  //===========================================================================
  // Iterator Access Concepts

  NDNBOOST_concept(ReadableIterator,(Iterator))
    : ndnboost::Assignable<Iterator>
    , ndnboost::CopyConstructible<Iterator>

  {
      typedef NDNBOOST_DEDUCED_TYPENAME ndnboost::detail::iterator_traits<Iterator>::value_type value_type;
      typedef NDNBOOST_DEDUCED_TYPENAME ndnboost::detail::iterator_traits<Iterator>::reference reference;

      NDNBOOST_CONCEPT_USAGE(ReadableIterator)
      {

          value_type v = *i;
          ndnboost::ignore_unused_variable_warning(v);
      }
  private:
      Iterator i;
  };
  
  template <
      typename Iterator
    , typename ValueType = NDNBOOST_DEDUCED_TYPENAME ndnboost::detail::iterator_traits<Iterator>::value_type
  >
  struct WritableIterator
    : ndnboost::CopyConstructible<Iterator>
  {
      NDNBOOST_CONCEPT_USAGE(WritableIterator)
      {
          *i = v;
      }
  private:
      ValueType v;
      Iterator i;
  };

  template <
      typename Iterator
    , typename ValueType = NDNBOOST_DEDUCED_TYPENAME ndnboost::detail::iterator_traits<Iterator>::value_type
  >
  struct WritableIteratorConcept : WritableIterator<Iterator,ValueType> {};
  
  NDNBOOST_concept(SwappableIterator,(Iterator))
  {
      NDNBOOST_CONCEPT_USAGE(SwappableIterator)
      {
          std::iter_swap(i1, i2);
      }
  private:
      Iterator i1;
      Iterator i2;
  };

  NDNBOOST_concept(LvalueIterator,(Iterator))
  {
      typedef typename ndnboost::detail::iterator_traits<Iterator>::value_type value_type;
      
      NDNBOOST_CONCEPT_USAGE(LvalueIterator)
      {
        value_type& r = const_cast<value_type&>(*i);
        ndnboost::ignore_unused_variable_warning(r);
      }
  private:
      Iterator i;
  };

  
  //===========================================================================
  // Iterator Traversal Concepts

  NDNBOOST_concept(IncrementableIterator,(Iterator))
    : ndnboost::Assignable<Iterator>
    , ndnboost::CopyConstructible<Iterator>
  {
      typedef typename ndnboost::iterator_traversal<Iterator>::type traversal_category;

      NDNBOOST_CONCEPT_ASSERT((
        ndnboost::Convertible<
            traversal_category
          , ndnboost::incrementable_traversal_tag
        >));

      NDNBOOST_CONCEPT_USAGE(IncrementableIterator)
      {
          ++i;
          (void)i++;
      }
  private:
      Iterator i;
  };

  NDNBOOST_concept(SinglePassIterator,(Iterator))
    : IncrementableIterator<Iterator>
    , ndnboost::EqualityComparable<Iterator>

  {
      NDNBOOST_CONCEPT_ASSERT((
          ndnboost::Convertible<
             NDNBOOST_DEDUCED_TYPENAME SinglePassIterator::traversal_category
           , ndnboost::single_pass_traversal_tag
          > ));
  };

  NDNBOOST_concept(ForwardTraversal,(Iterator))
    : SinglePassIterator<Iterator>
    , ndnboost::DefaultConstructible<Iterator>
  {
      typedef typename ndnboost::detail::iterator_traits<Iterator>::difference_type difference_type;
      
      NDNBOOST_MPL_ASSERT((ndnboost::is_integral<difference_type>));
      NDNBOOST_MPL_ASSERT_RELATION(std::numeric_limits<difference_type>::is_signed, ==, true);

      NDNBOOST_CONCEPT_ASSERT((
          ndnboost::Convertible<
             NDNBOOST_DEDUCED_TYPENAME ForwardTraversal::traversal_category
           , ndnboost::forward_traversal_tag
          > ));
  };
  
  NDNBOOST_concept(BidirectionalTraversal,(Iterator))
    : ForwardTraversal<Iterator>
  {
      NDNBOOST_CONCEPT_ASSERT((
          ndnboost::Convertible<
             NDNBOOST_DEDUCED_TYPENAME BidirectionalTraversal::traversal_category
           , ndnboost::bidirectional_traversal_tag
          > ));

      NDNBOOST_CONCEPT_USAGE(BidirectionalTraversal)
      {
          --i;
          (void)i--;
      }
   private:
      Iterator i;
  };

  NDNBOOST_concept(RandomAccessTraversal,(Iterator))
    : BidirectionalTraversal<Iterator>
  {
      NDNBOOST_CONCEPT_ASSERT((
          ndnboost::Convertible<
             NDNBOOST_DEDUCED_TYPENAME RandomAccessTraversal::traversal_category
           , ndnboost::random_access_traversal_tag
          > ));

      NDNBOOST_CONCEPT_USAGE(RandomAccessTraversal)
      {
          i += n;
          i = i + n;
          i = n + i;
          i -= n;
          i = i - n;
          n = i - j;
      }
      
   private:
      typename BidirectionalTraversal<Iterator>::difference_type n;
      Iterator i, j;
  };

  //===========================================================================
  // Iterator Interoperability 

  namespace detail
  {
    template <typename Iterator1, typename Iterator2>
    void interop_single_pass_constraints(Iterator1 const& i1, Iterator2 const& i2)
    {
        bool b;
        b = i1 == i2;
        b = i1 != i2;

        b = i2 == i1;
        b = i2 != i1;
        ndnboost::ignore_unused_variable_warning(b);
    }

    template <typename Iterator1, typename Iterator2>
    void interop_rand_access_constraints(
        Iterator1 const& i1, Iterator2 const& i2,
        ndnboost::random_access_traversal_tag, ndnboost::random_access_traversal_tag)
    {
        bool b;
        typename ndnboost::detail::iterator_traits<Iterator2>::difference_type n;
        b = i1 <  i2;
        b = i1 <= i2;
        b = i1 >  i2;
        b = i1 >= i2;
        n = i1 -  i2;

        b = i2 <  i1;
        b = i2 <= i1;
        b = i2 >  i1;
        b = i2 >= i1;
        n = i2 -  i1;
        ndnboost::ignore_unused_variable_warning(b);
        ndnboost::ignore_unused_variable_warning(n);
    }

    template <typename Iterator1, typename Iterator2>
    void interop_rand_access_constraints(
        Iterator1 const&, Iterator2 const&,
        ndnboost::single_pass_traversal_tag, ndnboost::single_pass_traversal_tag)
    { }

  } // namespace detail

  NDNBOOST_concept(InteroperableIterator,(Iterator)(ConstIterator))
  {
   private:
      typedef typename ndnboost::detail::pure_traversal_tag<
          typename ndnboost::iterator_traversal<
              Iterator
          >::type
      >::type traversal_category;

      typedef typename ndnboost::detail::pure_traversal_tag<
          typename ndnboost::iterator_traversal<
              ConstIterator
          >::type
      >::type const_traversal_category;
      
  public:
      NDNBOOST_CONCEPT_ASSERT((SinglePassIterator<Iterator>));
      NDNBOOST_CONCEPT_ASSERT((SinglePassIterator<ConstIterator>));

      NDNBOOST_CONCEPT_USAGE(InteroperableIterator)
      {
          detail::interop_single_pass_constraints(i, ci);
          detail::interop_rand_access_constraints(i, ci, traversal_category(), const_traversal_category());

          ci = i;
      }
      
   private:
      Iterator      i;
      ConstIterator ci;
  };

} // namespace ndnboost_concepts

#include <ndnboost/concept/detail/concept_undef.hpp>

#endif // NDNBOOST_ITERATOR_CONCEPTS_HPP
