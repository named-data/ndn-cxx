/* 
 * File:   UdpTransport.h
 * Author: jefft0
 *
 * Created on July 14, 2013, 4:15 PM
 */

#ifndef NDN_UDPTRANSPORT_H
#define NDN_UDPTRANSPORT_H

#include "socket-transport.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ndn_UdpTransport {
  struct ndn_SocketTransport base;
};
  
static inline void ndn_UdpTransport_init(struct ndn_UdpTransport *self)
{
  ndn_SocketTransport_init(&self->base);
}

static inline ndn_Error ndn_UdpTransport_connect(struct ndn_UdpTransport *self, char *host, unsigned short port)
{
  return ndn_SocketTransport_connect(&self->base, SOCKET_UDP, host, port);
}

static inline ndn_Error ndn_UdpTransport_send(struct ndn_UdpTransport *self, unsigned char *data, unsigned int dataLength)
{
  return ndn_SocketTransport_send(&self->base, data, dataLength);
}

static inline ndn_Error ndn_UdpTransport_receive
  (struct ndn_UdpTransport *self, unsigned char *buffer, unsigned int bufferLength, unsigned int *nBytes)
{
  return ndn_SocketTransport_receive(&self->base, buffer, bufferLength, nBytes);
}

static inline ndn_Error ndn_UdpTransport_close(struct ndn_UdpTransport *self)
{
  return ndn_SocketTransport_close(&self->base);
}

#ifdef __cplusplus
}
#endif

#endif
