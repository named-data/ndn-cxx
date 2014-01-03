/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#include <ndn-cpp/exclude.hpp>

namespace ndn
{

Exclude::Exclude ()
{
}

// example: ANY /b /d ANY /f
//
// ordered in map as:
//
// /f (false); /d (true); /b (false); / (true)
//
// lower_bound (/)  -> / (true) <-- excluded (equal)
// lower_bound (/a) -> / (true) <-- excluded (any)
// lower_bound (/b) -> /b (false) <--- excluded (equal)
// lower_bound (/c) -> /b (false) <--- not excluded (not equal and no ANY)
// lower_bound (/d) -> /d (true) <- excluded
// lower_bound (/e) -> /d (true) <- excluded
bool
Exclude::isExcluded (const Name::Component &comp) const
{
  const_iterator lowerBound = m_exclude.lower_bound (comp);
  if (lowerBound == end ())
    return false;

  if (lowerBound->second)
    return true;
  else
    return lowerBound->first == comp;

  return false;
}

Exclude &
Exclude::excludeOne (const Name::Component &comp)
{
  if (!isExcluded (comp))
    {
      m_exclude.insert (std::make_pair (comp, false));
    }
  return *this;
}


// example: ANY /b0 /d0 ANY /f0
//
// ordered in map as:
//
// /f0 (false); /d0 (true); /b0 (false); / (true)
//
// lower_bound (/)  -> / (true) <-- excluded (equal)
// lower_bound (/a0) -> / (true) <-- excluded (any)
// lower_bound (/b0) -> /b0 (false) <--- excluded (equal)
// lower_bound (/c0) -> /b0 (false) <--- not excluded (not equal and no ANY)
// lower_bound (/d0) -> /d0 (true) <- excluded
// lower_bound (/e0) -> /d0 (true) <- excluded


// examples with desired outcomes
// excludeRange (/, /f0) ->  ANY /f0
//                          /f0 (false); / (true)
// excludeRange (/, /f1) ->  ANY /f1
//                          /f1 (false); / (true)
// excludeRange (/a0, /e0) ->  ANY /f0
//                          /f0 (false); / (true)
// excludeRange (/a0, /e0) ->  ANY /f0
//                          /f0 (false); / (true)

// excludeRange (/b1, /c0) ->  ANY /b0 /b1 ANY /c0 /d0 ANY /f0
//                          /f0 (false); /d0 (true); /c0 (false); /b1 (true); /b0 (false); / (true)

Exclude &
Exclude::excludeRange (const Name::Component &from, const Name::Component &to)
{
  if (from >= to)
    {
      throw Error ("Invalid exclude range [" +
                   from.toEscapedString() + ", " +
                   to.toEscapedString() +
                   "] (for single name exclude use Exclude::excludeOne)");
    }

  iterator newFrom = m_exclude.lower_bound (from);
  if (newFrom == end () || !newFrom->second /*without ANY*/)
    {
      std::pair<iterator, bool> fromResult = m_exclude.insert (std::make_pair (from, true));
      newFrom = fromResult.first;
      if (!fromResult.second)
        {
          // this means that the lower bound is equal to the item itself. So, just update ANY flag
          newFrom->second = true;
        }
    }
  // else
  // nothing special if start of the range already exists with ANY flag set

  iterator newTo = m_exclude.lower_bound (to); // !newTo cannot be end ()
  if (newTo == newFrom || !newTo->second)
    {
      std::pair<iterator, bool> toResult = m_exclude.insert (std::make_pair (to, false));
      newTo = toResult.first;
      ++ newTo;
    }
  else
    {
      // nothing to do really
    }

  m_exclude.erase (newTo, newFrom); // remove any intermediate node, since all of the are excluded

  return *this;
}

Exclude &
Exclude::excludeAfter (const Name::Component &from)
{
  iterator newFrom = m_exclude.lower_bound (from);
  if (newFrom == end () || !newFrom->second /*without ANY*/)
    {
      std::pair<iterator, bool> fromResult = m_exclude.insert (std::make_pair (from, true));
      newFrom = fromResult.first;
      if (!fromResult.second)
        {
          // this means that the lower bound is equal to the item itself. So, just update ANY flag
          newFrom->second = true;
        }
    }
  // else
  // nothing special if start of the range already exists with ANY flag set

  if (newFrom != m_exclude.begin ())
    {
      m_exclude.erase (m_exclude.begin (), newFrom); // remove any intermediate node, since all of the are excluded
    }

  return *this;
}


std::ostream&
operator << (std::ostream &os, const Exclude &exclude)
{
  bool empty = true;
  for (Exclude::const_reverse_iterator i = exclude.rbegin (); i != exclude.rend (); i++)
    {
      if (!i->first.empty())
        {
          if (!empty) os << ",";
          os << i->first.toEscapedString ();
          empty = false;
        }
      if (i->second)
        {
          if (!empty) os << ",";
          os << "*";
          empty = false;
        }
    }
  return os;
}

const Block&
Exclude::wireEncode() const
{
  if (wire_.hasWire())
    return wire_;

  wire_ = Block(Tlv::Exclude);

  for (Exclude::const_reverse_iterator i = m_exclude.rbegin (); i != m_exclude.rend (); i++)
    {
      if (!i->first.empty())
        {
          OBufferStream os;
          Tlv::writeVarNumber(os, Tlv::NameComponent);
          Tlv::writeVarNumber(os, i->first.getValue().size());
          os.write(reinterpret_cast<const char *>(i->first.getValue().buf()), i->first.getValue().size());
          
          wire_.push_back(Block(os.buf()));

        }
      if (i->second)
        {
          OBufferStream os;
          Tlv::writeVarNumber(os, Tlv::Any);
          Tlv::writeVarNumber(os, 0);
          wire_.push_back(Block(os.buf()));
        }
    }

  wire_.encode();
  return wire_;
}
  
void 
Exclude::wireDecode(const Block &wire)
{
  wire_ = wire;
  wire_.parse();

  Block::element_const_iterator i = wire_.getAll().begin();
  if (i->type() == Tlv::Any)
    {
      appendExclude("/", true);
      ++i;
    }

  while (i != wire_.getAll().end())
    {
      if (i->type() != Tlv::NameComponent)
        throw Error("Incorrect format of Exclude filter");

      Name::Component excludedComponent (i->value(), i->value_size());
      ++i;

      if (i != wire_.getAll().end())
        {
          if (i->type() == Tlv::Any)
            {
              appendExclude(excludedComponent, true);
              ++i;
            }
          else
            {
              appendExclude(excludedComponent, false);
            }
        }
      else
        {
          appendExclude(excludedComponent, false);
        }
    }
}


} // ndn
