/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_TRANSPORT_HPP
#define	NDN_TRANSPORT_HPP

namespace ndn {

class NDN;  
class Transport {
public:
  virtual void connect(NDN &ndn);
  
  virtual void send(unsigned char *data, unsigned int dataLength);
};

}

#endif
