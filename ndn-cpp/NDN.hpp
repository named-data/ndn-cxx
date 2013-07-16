/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_NDN_HPP
#define	NDN_NDN_HPP

#include "Closure.hpp"
#include "encoding/BinaryXMLElementReader.hpp"

namespace ndn {

class NDN : public ElementListener {
public:
  NDN(Closure *closure)
  {
    closure_ = closure;
  }
  
  virtual void onReceivedElement(unsigned char *element, unsigned int elementLength);
  
private:
  Closure *closure_;
};

}

#endif
