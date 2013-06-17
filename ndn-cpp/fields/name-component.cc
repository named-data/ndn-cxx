/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#include "name-component.h"

#include "ndn-cpp/error.h"
#include "ndn-cpp/helpers/uri.h"

using namespace std;

namespace ndn
{
namespace name
{
  
Component::Component ()
{
}

Component::Component (const std::string &uri)
{
  try
    {
      Uri::fromEscaped (uri.begin (), uri.end (), back_inserter (*this));
    }
  catch (error::Uri &err)
    {
      // re-throwing different exception
      BOOST_THROW_EXCEPTION (error::name::Component ()
                             << error::msg (uri)
                             << error::pos (error::get_pos (err)));
    }
}

Component::Component (std::string::const_iterator begin, std::string::const_iterator end)
{
  try
    {
      Uri::fromEscaped (begin, end, back_inserter (*this));
    }
  catch (error::Uri &err)
    {
      // re-throwing different exception
      BOOST_THROW_EXCEPTION (error::name::Component ()
                             << error::msg (string (begin, end))
                             << error::pos (error::get_pos (err)));
    }
}

Component::Component (const void *buf, size_t length)
{
  copy (static_cast<const char*> (buf),
        static_cast<const char*> (buf)+length,
        back_inserter (*this));
}

int
Component::compare (const Component &other) const
{
  if (size () < other.size ())
    return -1;

  if (size () > other.size ())
    return +1;

  // now we know that sizes are equal

  pair<const_iterator, const_iterator> diff = mismatch (begin (), end (), other.begin ());
  if (diff.first == end ()) // components are actually equal
    return 0;

  return (std::lexicographical_compare (diff.first, end (), diff.second, other.end ())) ? -1 : +1;    
}

Component
Component::fromNumber (uint64_t number)
{
  Component comp;
  while (number > 0)
    {
      comp.push_back (static_cast<unsigned char> (number & 0xFF));
      number >>= 8;
    }
  std::reverse (comp.begin (), comp.end ());
  return comp;
}

Component
Component::fromNumberWithMarker (uint64_t number, unsigned char marker)
{
  Component comp;
  comp.push_back (marker);

  while (number > 0)
    {
      comp.push_back (static_cast<unsigned char> (number & 0xFF));
      number >>= 8;
    }

  std::reverse (comp.begin () + 1, comp.end ());
  return comp;
}

std::string
Component::toBlob () const
{
  return std::string (begin (), end ());
}

void
Component::toBlob (std::ostream &os) const
{
  os.write (buf (), size ());
}

std::string
Component::toUri () const
{
  ostringstream os;
  toUri (os);
  return os.str ();  
}

void
Component::toUri (std::ostream &os) const
{
  Uri::toEscaped (begin (), end (), ostream_iterator<char> (os));
}

uint64_t
Component::toNumber () const
{
  uint64_t ret = 0;
  for (const_iterator i = begin (); i != end (); i++)
    {
      ret <<= 8;
      ret |= static_cast<unsigned char> (*i);
    }
  return ret;
}

uint64_t
Component::toNumberWithMarker (unsigned char marker) const
{
  if (empty () ||
      static_cast<unsigned char> (*(begin ())) != marker)
    {
      BOOST_THROW_EXCEPTION (error::name::Component ()
                             << error::msg ("Name component does not have required marker [" + toUri () + "]"));
    }

  uint64_t ret = 0;
  for (const_iterator i = begin () + 1; i != end (); i++)
    {
      ret <<= 8;
      ret |= static_cast<unsigned char> (*i);
    }
  return ret;
}


} // name
} // ndn
