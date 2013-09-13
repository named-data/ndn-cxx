/* 
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_TCPTRANSPORT_H
#define NDN_TCPTRANSPORT_H

#include "socket-transport.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ndn_TcpTransport {
  struct ndn_SocketTransport base;
};
  
/**
 * Initialize the ndn_TcpTransport struct with default values for no connection yet.
 * @param self A pointer to the ndn_TcpTransport struct.
 */
static inline void ndn_TcpTransport_initialize(struct ndn_TcpTransport *self)
{
  ndn_SocketTransport_initialize(&self->base);
}

/**
 * Connect with TCP to the host:port.
 * @param self A pointer to the ndn_TcpTransport struct.
 * @param host The host to connect to.
 * @param port The port to connect to.
 * @return 0 for success, else an error code.
 */
static inline ndn_Error ndn_TcpTransport_connect(struct ndn_TcpTransport *self, char *host, unsigned short port)
{
  return ndn_SocketTransport_connect(&self->base, SOCKET_TCP, host, port);
}

/**
 * Send data to the socket.
 * @param self A pointer to the ndn_TcpTransport struct.
 * @param data A pointer to the buffer of data to send.
 * @param dataLength The number of bytes in data.
 * @return 0 for success, else an error code.
 */
static inline ndn_Error ndn_TcpTransport_send(struct ndn_TcpTransport *self, unsigned char *data, unsigned int dataLength)
{
  return ndn_SocketTransport_send(&self->base, data, dataLength);
}

/**
 * Check if there is data ready on the socket to be received with ndn_TcpTransport_receive.
 * This does not block, and returns immediately.
 * @param self A pointer to the ndn_TcpTransport struct.
 * @param receiveIsReady This will be set to 1 if data is ready, 0 if not.
 * @return 0 for success, else an error code.
 */
static inline ndn_Error ndn_TcpTransport_receiveIsReady(struct ndn_TcpTransport *self, int *receiveIsReady)
{
  return ndn_SocketTransport_receiveIsReady(&self->base, receiveIsReady);
}

/**
 * Receive data from the socket.  NOTE: This is a blocking call.  You should first call ndn_SocketTransport_receiveIsReady
 * to make sure there is data ready to receive.
 * @param self A pointer to the ndn_TcpTransport struct.
 * @param buffer A pointer to the buffer to receive the data.
 * @param bufferLength The maximum length of buffer.
 * @param nBytes Return the number of bytes received into buffer.
 * @return 0 for success, else an error code.
 */
static inline ndn_Error ndn_TcpTransport_receive
  (struct ndn_TcpTransport *self, unsigned char *buffer, unsigned int bufferLength, unsigned int *nBytes)
{
  return ndn_SocketTransport_receive(&self->base, buffer, bufferLength, nBytes);
}

/**
 * Close the socket.
 * @param self A pointer to the ndn_TcpTransport struct.
 * @return 0 for success, else an error code.
 */
static inline ndn_Error ndn_TcpTransport_close(struct ndn_TcpTransport *self)
{
  return ndn_SocketTransport_close(&self->base);
}

#ifdef __cplusplus
}
#endif

#endif
