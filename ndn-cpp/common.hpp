/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_COMMON_HPP
#define NDN_COMMON_HPP

#include <vector>
#include "../config.h"

// Depending on where ./configure found shared_ptr, define the ptr_lib namespace.
// We always use ndn::ptr_lib::shared_ptr.
#if HAVE_STD_SHARED_PTR
#include <memory>
namespace ndn { namespace ptr_lib = std; }
#elif HAVE_BOOST_SHARED_PTR
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
namespace ndn { namespace ptr_lib = boost; }
#else
#error "Can't find shared_ptr in std or boost"
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
