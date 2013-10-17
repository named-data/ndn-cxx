// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2005-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

#ifndef NDNBOOST_IOSTREAMS_FILTER_TEST_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <ndnboost/config.hpp>               // NDNBOOST_MSVC,put size_t in std.
#include <ndnboost/detail/workaround.hpp>
#include <algorithm>                      // min.
#include <cstddef>                        // size_t.
#if NDNBOOST_WORKAROUND(NDNBOOST_MSVC, <= 1300) || \
    NDNBOOST_WORKAROUND(__BORLANDC__, NDNBOOST_TESTED_AT(0x564)) || \
    NDNBOOST_WORKAROUND(__MWERKS__, <= 0x3003) \
    /**/
# include <cstdlib>                       // rand.
#endif
#include <cstring>                        // memcpy, strlen.
#include <iterator>
#include <string>
#include <vector>
#if !NDNBOOST_WORKAROUND(NDNBOOST_MSVC, <= 1300) && \
    !NDNBOOST_WORKAROUND(__BORLANDC__, NDNBOOST_TESTED_AT(0x564)) && \
    !NDNBOOST_WORKAROUND(__MWERKS__, <= 0x3003) \
    /**/
# include <ndnboost/random/linear_congruential.hpp>
# include <ndnboost/random/uniform_smallint.hpp>
#endif
#include <ndnboost/iostreams/categories.hpp>
#include <ndnboost/iostreams/compose.hpp>
#include <ndnboost/iostreams/copy.hpp>
#include <ndnboost/iostreams/detail/bool_trait_def.hpp>
#include <ndnboost/iostreams/detail/ios.hpp>
#include <ndnboost/iostreams/device/array.hpp>
#include <ndnboost/iostreams/device/back_inserter.hpp>
#include <ndnboost/iostreams/operations.hpp>
#include <ndnboost/mpl/bool.hpp>
#include <ndnboost/type_traits/is_array.hpp>
#include <ndnboost/type_traits/is_same.hpp>

#undef memcpy
#undef rand
#undef strlen

#if defined(NDNBOOST_NO_STDC_NAMESPACE) && !defined(__LIBCOMO__)
namespace std { 
    using ::memcpy; 
    using ::strlen; 
    #if NDNBOOST_WORKAROUND(NDNBOOST_MSVC, <= 1300) || \
        NDNBOOST_WORKAROUND(__BORLANDC__, NDNBOOST_TESTED_AT(0x564)) || \
        NDNBOOST_WORKAROUND(__MWERKS__, <= 0x3003) \
        /**/
        using ::rand; 
    #endif
}
#endif

namespace ndnboost { namespace iostreams {

NDNBOOST_IOSTREAMS_BOOL_TRAIT_DEF(is_string, std::basic_string, 3)

const std::streamsize default_increment = 5;

#if !NDNBOOST_WORKAROUND(NDNBOOST_MSVC, <= 1300) && \
    !NDNBOOST_WORKAROUND(__BORLANDC__, NDNBOOST_TESTED_AT(0x564)) && \
    !NDNBOOST_WORKAROUND(__MWERKS__, <= 0x3003) \
    /**/
    std::streamsize rand(int inc)
    {
        static rand48                random_gen;
        static uniform_smallint<int> random_dist(0, inc);
        return random_dist(random_gen);
    }
#else
    std::streamsize rand(int inc) 
    { 
        return (std::rand() * inc + 1) / RAND_MAX; 
    }
#endif

class non_blocking_source {
public:
    typedef char char_type;
    struct category
        : source_tag,
          peekable_tag
        { };
    explicit non_blocking_source( const std::string& data, 
                                  std::streamsize inc = default_increment ) 
        : data_(data), inc_(inc), pos_(0)
        { }
    std::streamsize read(char* s, std::streamsize n)
    {
        if (pos_ == static_cast<std::streamsize>(data_.size()))
            return -1;
        std::streamsize avail = 
            (std::min) (n, static_cast<std::streamsize>(data_.size() - pos_));
        std::streamsize amt = (std::min) (rand(inc_), avail);
        if (amt)
            std::memcpy(s, data_.c_str() + pos_, amt);
        pos_ += amt;
        return amt;
    }

    bool putback(char c)
    {
        if (pos_ > 0) {
            data_[--pos_] = c;
            return true;
        }
        return false;
    }
private:
    std::string      data_;
    std::streamsize  inc_, pos_;
};

class non_blocking_sink : public sink {
public:
    non_blocking_sink( std::string& dest,
                       std::streamsize inc = default_increment ) 
        : dest_(dest), inc_(inc) 
        { }
    std::streamsize write(const char* s, std::streamsize n)
    {
        std::streamsize amt = (std::min) (rand(inc_), n);
        dest_.insert(dest_.end(), s, s + amt);
        return amt;
    }
private:
    non_blocking_sink& operator=(const non_blocking_sink&);
    std::string&     dest_;
    std::streamsize  inc_;
};
                
//--------------Definition of test_input_filter-------------------------------//

template<typename Filter>
bool test_input_filter( Filter filter, 
                        const std::string& input, 
                        const std::string& output, 
                        mpl::true_ )
{
    for ( int inc = default_increment; 
          inc < default_increment * 40; 
          inc += default_increment )
    {
        non_blocking_source  src(input, inc);
        std::string          dest;
        iostreams::copy(compose(filter, src), iostreams::back_inserter(dest));
        if (dest != output)
            return false;
    }
    return true;
}

template<typename Filter, typename Source1, typename Source2>
bool test_input_filter( Filter filter, 
                        const Source1& input, 
                        const Source2& output, 
                        mpl::false_ )
{
    std::string in;
    std::string out;
    iostreams::copy(input, iostreams::back_inserter(in));
    iostreams::copy(output, iostreams::back_inserter(out));
    return test_input_filter(filter, in, out);
}

template<typename Filter, typename Source1, typename Source2>
bool test_input_filter( Filter filter, 
                        const Source1& input, 
                        const Source2& output )
{
    // Use tag dispatch to compensate for bad overload resolution.
    return test_input_filter( filter, input, output,    
                              is_string<Source1>() );
}

//--------------Definition of test_output_filter------------------------------//

template<typename Filter>
bool test_output_filter( Filter filter, 
                         const std::string& input, 
                         const std::string& output, 
                         mpl::true_ )
{
    for ( int inc = default_increment; 
          inc < default_increment * 40; 
          inc += default_increment )
    {
        array_source  src(input.data(), input.data() + input.size());
        std::string   dest;
        iostreams::copy(src, compose(filter, non_blocking_sink(dest, inc)));
        if (dest != output )
            return false;
    }
    return true;
}

template<typename Filter, typename Source1, typename Source2>
bool test_output_filter( Filter filter, 
                         const Source1& input, 
                         const Source2& output, 
                         mpl::false_ )
{
    std::string in;
    std::string out;
    iostreams::copy(input, iostreams::back_inserter(in));
    iostreams::copy(output, iostreams::back_inserter(out));
    return test_output_filter(filter, in, out);
}

template<typename Filter, typename Source1, typename Source2>
bool test_output_filter( Filter filter, 
                         const Source1& input, 
                         const Source2& output )
{
    // Use tag dispatch to compensate for bad overload resolution.
    return test_output_filter( filter, input, output,    
                               is_string<Source1>() );
}

//--------------Definition of test_filter_pair--------------------------------//

template<typename OutputFilter, typename InputFilter>
bool test_filter_pair( OutputFilter out, 
                       InputFilter in, 
                       const std::string& data, 
                       mpl::true_ )
{
    for ( int inc = default_increment; 
          inc <= default_increment * 40; 
          inc += default_increment )
    {
        {
            array_source  src(data.data(), data.data() + data.size());
            std::string   temp;
            std::string   dest;
            iostreams::copy(src, compose(out, non_blocking_sink(temp, inc)));
            iostreams::copy( 
                compose(in, non_blocking_source(temp, inc)),
                iostreams::back_inserter(dest)
            );
            if (dest != data)
                return false;
        }
        {
            array_source  src(data.data(), data.data() + data.size());
            std::string   temp;
            std::string   dest;
            iostreams::copy(src, compose(out, non_blocking_sink(temp, inc)));
            // truncate the file, this should not loop, it may throw
            // std::ios_base::failure, which we swallow.
            try {
                temp.resize(temp.size() / 2);
                iostreams::copy( 
                    compose(in, non_blocking_source(temp, inc)),
                    iostreams::back_inserter(dest)
                );
            } catch(std::ios_base::failure&) {}
        }
        {
            array_source  src(data.data(), data.data() + data.size());
            std::string   temp;
            std::string   dest;
            iostreams::copy(compose(out, src), non_blocking_sink(temp, inc));
            iostreams::copy( 
                non_blocking_source(temp, inc),
                compose(in, iostreams::back_inserter(dest))
            );
            if (dest != data)
                return false;
        }
        {
            array_source  src(data.data(), data.data() + data.size());
            std::string   temp;
            std::string   dest;
            iostreams::copy(compose(out, src), non_blocking_sink(temp, inc));
            // truncate the file, this should not loop, it may throw
            // std::ios_base::failure, which we swallow.
            try {
                temp.resize(temp.size() / 2);
                iostreams::copy( 
                    non_blocking_source(temp, inc),
                    compose(in, iostreams::back_inserter(dest))
                );
            } catch(std::ios_base::failure&) {}
        }
    }
    return true;
}

template<typename OutputFilter, typename InputFilter, typename Source>
bool test_filter_pair( OutputFilter out, 
                       InputFilter in, 
                       const Source& data, 
                       mpl::false_ )
{
    std::string str;
    iostreams::copy(data, iostreams::back_inserter(str));
    return test_filter_pair(out, in, str);
}

template<typename OutputFilter, typename InputFilter, typename Source>
bool test_filter_pair( OutputFilter out, 
                       InputFilter in, 
                       const Source& data )
{
    // Use tag dispatch to compensate for bad overload resolution.
    return test_filter_pair(out, in, data, is_string<Source>());
}

} } // End namespaces iostreams, boost.

#endif // #ifndef NDNBOOST_IOSTREAMS_FILTER_TEST_HPP_INCLUDED
