//  ndnboost/filesystem/convenience.hpp  ----------------------------------------//

//  Copyright Beman Dawes, 2002-2005
//  Copyright Vladimir Prus, 2002
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/filesystem

//----------------------------------------------------------------------------// 

#ifndef NDNBOOST_FILESYSTEM3_CONVENIENCE_HPP
#define NDNBOOST_FILESYSTEM3_CONVENIENCE_HPP

#include <ndnboost/config.hpp>

# if defined( NDNBOOST_NO_STD_WSTRING )
#   error Configuration not supported: Boost.Filesystem V3 and later requires std::wstring support
# endif

#include <ndnboost/filesystem/operations.hpp>
#include <ndnboost/system/error_code.hpp>

#include <ndnboost/config/abi_prefix.hpp> // must be the last #include

namespace ndnboost
{
  namespace filesystem
  {

# ifndef NDNBOOST_FILESYSTEM_NO_DEPRECATED

    inline std::string extension(const path & p)
    {
      return p.extension().string();
    }

    inline std::string basename(const path & p)
    {
      return p.stem().string();
    }

    inline path change_extension( const path & p, const path & new_extension )
    { 
      path new_p( p );
      new_p.replace_extension( new_extension );
      return new_p;
    }

# endif


  } // namespace filesystem
} // namespace ndnboost

#include <ndnboost/config/abi_suffix.hpp> // pops abi_prefix.hpp pragmas
#endif // NDNBOOST_FILESYSTEM3_CONVENIENCE_HPP
