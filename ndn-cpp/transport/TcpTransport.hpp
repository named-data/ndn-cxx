/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_TCPTRANSPORT_HPP
#define	NDN_TCPTRANSPORT_HPP

#include "../c/transport/TcpTransport.h"
#include "Transport.hpp"

namespace ndn {
  
class TcpTransport : public Transport {
public:
  TcpTransport() 
  {
    ndn_TcpTransport_init(&transport_);
  }
  
  virtual void connect(char *host, unsigned short port);
  
  virtual void send(unsigned char *data, unsigned int dataLength);

  virtual unsigned int receive(unsigned char *buffer, unsigned int bufferLength);
  
private:
  struct ndn_TcpTransport transport_;
};

}

#endif
