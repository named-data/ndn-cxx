/* 
 * File:   TcpTransport.h
 * Author: jefft0
 *
 * Created on July 14, 2013, 4:15 PM
 */

#ifndef NDN_TCPTRANSPORT_H
#define	NDN_TCPTRANSPORT_H

#ifdef	__cplusplus
extern "C" {
#endif

struct ndn_TcpTransport {
  int socketDescriptor; /**< -1 if not connected */
};
  
static inline void ndn_TcpTransport_init(ndn_TcpTransport *self)
{
  self->socketDescriptor = -1;
}

int testTcpTransport(unsigned char *data, unsigned int dataLength);

#ifdef	__cplusplus
}
#endif

#endif
