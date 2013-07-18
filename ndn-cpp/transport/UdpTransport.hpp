/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_TCPTRANSPORT_HPP
#define	NDN_TCPTRANSPORT_HPP

#include "../c/transport/UdpTransport.h"
#include "../c/encoding/BinaryXMLElementReader.h"
#include "Transport.hpp"

namespace ndn {
  
class UdpTransport : public Transport {
public:
  UdpTransport() 
  {
    ndn_UdpTransport_init(&transport_);
    ndn_ = 0;
  }
  
  /**
   * 
   * @param ndn Not a shared_ptr because we assume that it will remain valid during the life of this Transport object.
   */
  virtual void connect(NDN &ndn);
  
  virtual void send(unsigned char *data, unsigned int dataLength);

  virtual void tempReceive();
  
private:
  struct ndn_UdpTransport transport_;
  NDN *ndn_;
  // TODO: This belongs in the socket listener.
  ndn_BinaryXMLElementReader elementReader_;
};

}

#endif
