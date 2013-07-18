/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_UDPTRANSPORT_H
#define	NDN_UDPTRANSPORT_H

#include "../errors.h"

#ifdef	__cplusplus
extern "C" {
#endif

struct ndn_UdpTransport {
  int socketDescriptor; /**< -1 if not connected */
};
  
static inline void ndn_UdpTransport_init(struct ndn_UdpTransport *self)
{
  self->socketDescriptor = -1;
}

ndn_Error ndn_UdpTransport_connect(struct ndn_UdpTransport *self, char *host, unsigned short port);

ndn_Error ndn_UdpTransport_send(struct ndn_UdpTransport *self, unsigned char *data, unsigned int dataLength);

ndn_Error ndn_UdpTransport_receive
  (struct ndn_UdpTransport *self, unsigned char *buffer, unsigned int bufferLength, unsigned int *nBytes);

#ifdef	__cplusplus
}
#endif

#endif
