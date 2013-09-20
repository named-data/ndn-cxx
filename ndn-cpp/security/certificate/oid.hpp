/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_OID_HPP
#define	NDN_OID_HPP

#include <vector>
#include <string>

namespace ndn {

class OID {
public:
  OID () 
  {
  }
    
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
  toString();

  bool operator == (const OID& oid)
  {
    return equal(oid);
  }

  bool operator != (const OID& oid)
  {
    return !equal(oid);
  }
  
private:
  bool equal(const OID& oid);

  std::vector<int> oid_;
};

}

#endif
