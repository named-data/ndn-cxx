/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_OID_HPP
#define NDN_OID_HPP

#include <vector>
#include <string>

namespace CryptoPP { class BufferedTransformation; }

namespace ndn {

class OID {
public:
  OID () 
  {
  }
    
  OID(const char *oid);

  OID(const std::string& oid);

  OID(const std::vector<int>& oid)
  : oid_(oid)
  {
  }

  const std::vector<int> &
  getIntegerList() const
  {
    return oid_;
  }

  void
  setIntegerList(const std::vector<int>& value){
    oid_ = value;
  }

  std::string 
  toString() const;

  bool operator == (const OID& oid) const
  {
    return equal(oid);
  }

  bool operator != (const OID& oid) const
  {
    return !equal(oid);
  }

  void
  encode(CryptoPP::BufferedTransformation &out) const;

  void
  decode(CryptoPP::BufferedTransformation &in);


private:
  void
  construct(const std::string &value);
  
  bool
  equal(const OID& oid) const;

private:
  std::vector<int> oid_;
};

}

#endif
