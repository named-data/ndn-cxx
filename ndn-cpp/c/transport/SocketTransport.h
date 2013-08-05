/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SOCKETTRANSPORT_H
#define	NDN_SOCKETTRANSPORT_H

#include <sys/socket.h>
#include "../errors.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum {
  SOCKET_TCP,
  SOCKET_UDP
} ndn_SocketType;
  
struct ndn_SocketTransport {
  int socketDescriptor; /**< -1 if not connected */
};
  
static inline void ndn_SocketTransport_init(struct ndn_SocketTransport *self)
{
  self->socketDescriptor = -1;
}

ndn_Error ndn_SocketTransport_connect(struct ndn_SocketTransport *self, ndn_SocketType socketType, char *host, unsigned short port);

ndn_Error ndn_SocketTransport_send(struct ndn_SocketTransport *self, unsigned char *data, unsigned int dataLength);

ndn_Error ndn_SocketTransport_receive
  (struct ndn_SocketTransport *self, unsigned char *buffer, unsigned int bufferLength, unsigned int *nBytes);

ndn_Error ndn_SocketTransport_close(struct ndn_SocketTransport *self);

#ifdef	__cplusplus
}
#endif

#endif
