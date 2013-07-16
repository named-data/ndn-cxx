/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_NDN_HPP
#define	NDN_NDN_HPP

#include "encoding/BinaryXMLElementReader.hpp"

namespace ndn {

class NDN : public ElementListener {
public:
  virtual void onReceivedElement(unsigned char *element, unsigned int elementLength);
};

}

#endif
