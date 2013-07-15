/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_TRANSPORT_HPP
#define	NDN_TRANSPORT_HPP

namespace ndn {
  
class Transport {
public:
  virtual void connect(char *host, unsigned short port);
  
  virtual void send(unsigned char *data, unsigned int dataLength);
  
  virtual unsigned int receive(unsigned char *buffer, unsigned int bufferLength);
};

}

#endif
