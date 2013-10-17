// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2005-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

// Recent changes to Boost.Optional involving assigment broke Boost.Iostreams,
// in a way which could be remedied only by relying on the deprecated reset
// functions; with VC6, even reset didn't work. Until this problem is 
// understood, Iostreams will use a private version of optional with a smart 
// pointer interface.

#ifndef NDNBOOST_IOSTREAMS_DETAIL_OPTIONAL_HPP_INCLUDED
#define NDNBOOST_IOSTREAMS_DETAIL_OPTIONAL_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <ndnboost/assert.hpp>
#include <ndnboost/mpl/int.hpp>
#include <ndnboost/type_traits/aligned_storage.hpp>
#include <ndnboost/type_traits/alignment_of.hpp>

namespace ndnboost { namespace iostreams { namespace detail {

// Taken from <ndnboost/optional.hpp>.
template<class T>
class aligned_storage
{
    // Borland ICEs if unnamed unions are used for this!
    union dummy_u
    {
        char data[ sizeof(T) ];
        NDNBOOST_DEDUCED_TYPENAME type_with_alignment<
          ::ndnboost::alignment_of<T>::value >::type aligner_;
    } dummy_ ;

  public:

    void const* address() const { return &dummy_.data[0]; }
    void      * address()       { return &dummy_.data[0]; }
};

template<typename T>
class optional {
public:
    typedef T element_type;
    optional() : initialized_(false) { }
    optional(const T& t) : initialized_(false) { reset(t); }
    ~optional() { reset(); }
    T& operator*() 
    { 
        NDNBOOST_ASSERT(initialized_);
        return *static_cast<T*>(address()); 
    }
    const T& operator*() const
    { 
        NDNBOOST_ASSERT(initialized_);
        return *static_cast<const T*>(address()); 
    }
    T* operator->() 
    { 
        NDNBOOST_ASSERT(initialized_);
        return static_cast<T*>(address()); 
    }
    const T* operator->() const
    { 
        NDNBOOST_ASSERT(initialized_);
        return static_cast<const T*>(address()); 
    }
    T* get() 
    { 
        NDNBOOST_ASSERT(initialized_);
        return static_cast<T*>(address()); 
    }
    const T* get() const
    { 
        NDNBOOST_ASSERT(initialized_);
        return static_cast<const T*>(address()); 
    }
    void reset() 
    {
        if (initialized_) { 
        #if NDNBOOST_WORKAROUND(__BORLANDC__, NDNBOOST_TESTED_AT(0x564)) || \
            NDNBOOST_WORKAROUND(__IBMCPP__, NDNBOOST_TESTED_AT(600)) \
            /**/
            T* t = static_cast<T*>(address());
            t->~T();
        #else
            static_cast<T*>(address())->T::~T();
        #endif
            initialized_ = false;
        }
    }
    void reset(const T& t) 
    {
        reset();
        new (address()) T(t); 
        initialized_ = true;
    }
private:
    optional(const optional&);
    optional& operator=(const optional&);
    void* address() { return &storage_; }
    const void* address() const { return &storage_; }
    aligned_storage<T>  storage_;
    bool                initialized_;
};

} } } // End namespaces detail, iostreams, boost.

#endif // #ifndef NDNBOOST_IOSTREAMS_DETAIL_OPTIONAL_HPP_INCLUDED
