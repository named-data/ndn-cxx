/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SOCKETTRANSPORT_H
#define NDN_SOCKETTRANSPORT_H

#include <sys/socket.h>
#include "../errors.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  SOCKET_TCP,
  SOCKET_UDP
} ndn_SocketType;
  
struct ndn_SocketTransport {
  int socketDescriptor; /**< -1 if not connected */
};
  
/**
 * Initialize the ndn_SocketTransport struct with default values for no connection yet.
 * @param self A pointer to the ndn_SocketTransport struct.
 */
static inline void ndn_SocketTransport_initialize(struct ndn_SocketTransport *self)
{
  self->socketDescriptor = -1;
}

/**
 * Connect with TCP or UDP to the host:port.
 * @param self A pointer to the ndn_SocketTransport struct.
 * @param socketType SOCKET_TCP or SOCKET_UDP.
 * @param host The host to connect to.
 * @param port The port to connect to.
 * @return 0 for success, else an error code.
 */
ndn_Error ndn_SocketTransport_connect(struct ndn_SocketTransport *self, ndn_SocketType socketType, char *host, unsigned short port);

/**
 * Send data to the socket.
 * @param self A pointer to the ndn_SocketTransport struct.
 * @param data A pointer to the buffer of data to send.
 * @param dataLength The number of bytes in data.
 * @return 0 for success, else an error code.
 */
ndn_Error ndn_SocketTransport_send(struct ndn_SocketTransport *self, unsigned char *data, unsigned int dataLength);

/**
 * Check if there is data ready on the socket to be received with ndn_SocketTransport_receive.
 * This does not block, and returns immediately.
 * @param self A pointer to the ndn_SocketTransport struct.
 * @param receiveIsReady This will be set to 1 if data is ready, 0 if not.
 * @return 0 for success, else an error code.
 */
ndn_Error ndn_SocketTransport_receiveIsReady(struct ndn_SocketTransport *self, int *receiveIsReady);

/**
 * Receive data from the socket.  NOTE: This is a blocking call.  You should first call ndn_SocketTransport_receiveIsReady
 * to make sure there is data ready to receive.
 * @param self A pointer to the ndn_SocketTransport struct.
 * @param buffer A pointer to the buffer to receive the data.
 * @param bufferLength The maximum length of buffer.
 * @param nBytes Return the number of bytes received into buffer.
 * @return 0 for success, else an error code.
 */
ndn_Error ndn_SocketTransport_receive
  (struct ndn_SocketTransport *self, unsigned char *buffer, unsigned int bufferLength, unsigned int *nBytes);

/**
 * Close the socket.
 * @param self A pointer to the ndn_SocketTransport struct.
 * @return 0 for success, else an error code.
 */
ndn_Error ndn_SocketTransport_close(struct ndn_SocketTransport *self);

#ifdef __cplusplus
}
#endif

#endif
