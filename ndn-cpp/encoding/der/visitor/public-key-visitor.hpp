/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_PUBLIC_KEY_VISITOR_HPP
#define NDN_PUBLIC_KEY_VISITOR_HPP

#include "no-arguments-visitor.hpp"

namespace ndn {

namespace der {

class PublicKeyVisitor : public NoArgumentsVisitor {
  public:
    virtual ndnboost::any visit(DerSequence&       );
};

} // der

}

#endif
