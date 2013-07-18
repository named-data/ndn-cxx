/* 
 * File:   TcpTransport.h
 * Author: jefft0
 *
 * Created on July 14, 2013, 4:15 PM
 */

#ifndef NDN_TCPTRANSPORT_H
#define	NDN_TCPTRANSPORT_H

#include "../errors.h"

#ifdef	__cplusplus
extern "C" {
#endif

struct ndn_TcpTransport {
  int socketDescriptor; /**< -1 if not connected */
};
  
static inline void ndn_TcpTransport_init(struct ndn_TcpTransport *self)
{
  self->socketDescriptor = -1;
}

ndn_Error ndn_TcpTransport_connect(struct ndn_TcpTransport *self, char *host, unsigned short port);

ndn_Error ndn_TcpTransport_send(struct ndn_TcpTransport *self, unsigned char *data, unsigned int dataLength);

ndn_Error ndn_TcpTransport_receive
  (struct ndn_TcpTransport *self, unsigned char *buffer, unsigned int bufferLength, unsigned int *nBytes);

#ifdef	__cplusplus
}
#endif

#endif
