/* 
 * File:   TcpTransport.h
 * Author: jefft0
 *
 * Created on July 14, 2013, 4:15 PM
 */

#ifndef NDN_TCPTRANSPORT_H
#define	NDN_TCPTRANSPORT_H

#include "socket-transport.h"

#ifdef	__cplusplus
extern "C" {
#endif

struct ndn_TcpTransport {
  struct ndn_SocketTransport base;
};
  
static inline void ndn_TcpTransport_init(struct ndn_TcpTransport *self)
{
  ndn_SocketTransport_init(&self->base);
}

static inline ndn_Error ndn_TcpTransport_connect(struct ndn_TcpTransport *self, char *host, unsigned short port)
{
  return ndn_SocketTransport_connect(&self->base, SOCKET_TCP, host, port);
}

static inline ndn_Error ndn_TcpTransport_send(struct ndn_TcpTransport *self, unsigned char *data, unsigned int dataLength)
{
  return ndn_SocketTransport_send(&self->base, data, dataLength);
}

static inline ndn_Error ndn_TcpTransport_receive
  (struct ndn_TcpTransport *self, unsigned char *buffer, unsigned int bufferLength, unsigned int *nBytes)
{
  return ndn_SocketTransport_receive(&self->base, buffer, bufferLength, nBytes);
}

static inline ndn_Error ndn_TcpTransport_close(struct ndn_TcpTransport *self)
{
  return ndn_SocketTransport_close(&self->base);
}

#ifdef	__cplusplus
}
#endif

#endif
