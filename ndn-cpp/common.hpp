/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_COMMON_HPP
#define NDN_COMMON_HPP

#include <vector>
#include "../config.h"

// Depending on where ./configure found shared_ptr, define the ptr_lib namespace.
// We always use ndn::ptr_lib.
#if HAVE_STD_SHARED_PTR
#include <memory>
namespace ndn { namespace ptr_lib = std; }
#elif HAVE_BOOST_SHARED_PTR
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
namespace ndn { namespace ptr_lib = boost; }
#else
// Use the boost header files in this distribution that were extracted with:
// bcp --namespace=ndnboost shared_ptr make_shared ~/bcp_temp
// Since HAVE_BOOST_SHARED_PTR failed, assume that there is no boost subdirectory on the INCLUDE path, so that
//   <boost> is the boost subdirectory in this distribution.
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
namespace ndn { namespace ptr_lib = ndnboost; }
#endif

namespace ndn {
  
/**
 * Clear the vector and copy valueLength bytes from value.
 * @param v the vector to copy to
 * @param value the array of bytes, or 0 to not copy
 * @param valueLength the length of value
 */
static inline void setVector(std::vector<unsigned char> &vec, const unsigned char *value, unsigned int valueLength) 
{
	vec.clear();
  if (value)
    vec.insert(vec.begin(), value, value + valueLength);  
}
  
}

#endif
