/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_BLOB_STREAM_HPP
#define NDN_BLOB_STREAM_HPP

#include <ndn-cpp/common.hpp>

#if NDN_CPP_USE_SYSTEM_BOOST
#include <boost/iostreams/detail/ios.hpp>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/stream.hpp>
namespace ndnboost = boost;
#else
// We can use ndnboost::iostreams because this is internal and will not conflict with the application if it uses boost::iostreams.
#include <ndnboost/iostreams/detail/ios.hpp>
#include <ndnboost/iostreams/categories.hpp>
#include <ndnboost/iostreams/stream.hpp>
#endif

namespace ndn {

class blob_append_device {
public:
  typedef char  char_type;
  typedef ndnboost::iostreams::sink_tag category;
  
  blob_append_device(std::vector<uint8_t>& container)
  : container_(container)
  {
  }
  
  std::streamsize
  write(const char_type* s, std::streamsize n)
  {
    std::copy(s, s+n, std::back_inserter(container_));
    return n;
  }
  
protected:
  std::vector<uint8_t>& container_;
};

/**
 * This is called "blob_stream" but it doesn't use an ndn::Blob which is immutable.  It uses a pointer to a vector<uint8_t>.
 * This is inteded for internal library use, not exported in the API.
 */
struct blob_stream : public ndnboost::iostreams::stream<blob_append_device>
{
  blob_stream()
    : buffer_(new std::vector<uint8_t>())
    , device_(*buffer_)
  {
    open(device_);
  }

  ptr_lib::shared_ptr<std::vector<uint8_t> >
  buf()
  {
    flush();
    return buffer_;
  }

private:
  ptr_lib::shared_ptr<std::vector<uint8_t> > buffer_;
  blob_append_device device_;
};

}

#endif
