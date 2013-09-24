/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include "socket-transport.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include "../util/ndn_memory.h"

ndn_Error ndn_SocketTransport_connect(struct ndn_SocketTransport *self, ndn_SocketType socketType, char *host, unsigned short port)
{
  if (self->socketDescriptor >= 0) {
    close(self->socketDescriptor);
    self->socketDescriptor = -1;
  }
  
  struct addrinfo hints;
  ndn_memset((uint8_t *)&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  if (socketType == SOCKET_TCP)
    hints.ai_socktype = SOCK_STREAM;
  else if (socketType == SOCKET_UDP)
    hints.ai_socktype = SOCK_DGRAM;
  else
    return NDN_ERROR_unrecognized_ndn_SocketTransport;

  char portString[10];
  sprintf(portString, "%d", port);
  
  struct addrinfo *serverInfo;
  if (getaddrinfo(host, portString, &hints, &serverInfo) != 0)
    return NDN_ERROR_SocketTransport_error_in_getaddrinfo;

  // loop through all the results and connect to the first we can
  struct addrinfo *p;
  int socketDescriptor;
  for(p = serverInfo; p != NULL; p = p->ai_next) {
    if ((socketDescriptor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
      continue;

    if (connect(socketDescriptor, p->ai_addr, p->ai_addrlen) == -1) {
      close(socketDescriptor);
      continue;
    }

    break;
  }

  if (p == NULL) {
    freeaddrinfo(serverInfo);
    return NDN_ERROR_SocketTransport_cannot_connect_to_socket;
  }

  freeaddrinfo(serverInfo);
  self->socketDescriptor = socketDescriptor;

  return NDN_ERROR_success;
}

ndn_Error ndn_SocketTransport_send(struct ndn_SocketTransport *self, uint8_t *data, unsigned int dataLength)
{
  if (self->socketDescriptor < 0)
    return NDN_ERROR_SocketTransport_socket_is_not_open;
  
  int nBytes;
  while (1) {
    if ((nBytes = send(self->socketDescriptor, data, dataLength, 0)) < 0)
      return NDN_ERROR_SocketTransport_error_in_send;
    if (nBytes >= dataLength)
      break;
    
    // Send more.
    dataLength -= nBytes;
  }

  return NDN_ERROR_success;  
}

ndn_Error ndn_SocketTransport_receiveIsReady(struct ndn_SocketTransport *self, int *receiveIsReady)
{
  // Default to not ready.
  *receiveIsReady = 0;

  if (self->socketDescriptor < 0)
    // The socket is not open.  Just silently return.
    return NDN_ERROR_success;
  
  struct pollfd pollInfo[1];
  pollInfo[0].fd = self->socketDescriptor;
  pollInfo[0].events = POLLIN;
  
  int pollResult = poll(pollInfo, 1, 0);

  if (pollResult < 0)
    return NDN_ERROR_SocketTransport_error_in_poll;
  else if (pollResult == 0)
    // Timeout, so no data ready.
    return NDN_ERROR_success;
  else {
   if (pollInfo[0].revents & POLLIN)
     *receiveIsReady = 1;
  }

  return NDN_ERROR_success;
}

ndn_Error ndn_SocketTransport_receive
  (struct ndn_SocketTransport *self, uint8_t *buffer, unsigned int bufferLength, unsigned int *nBytesOut)
{
  if (self->socketDescriptor < 0)
    return NDN_ERROR_SocketTransport_socket_is_not_open;

  int nBytes;  
  if ((nBytes = recv(self->socketDescriptor, buffer, bufferLength, 0)) == -1)
    return NDN_ERROR_SocketTransport_error_in_recv;

  *nBytesOut = (unsigned int)nBytes;
  
  return NDN_ERROR_success;  
}

ndn_Error ndn_SocketTransport_close(struct ndn_SocketTransport *self)
{
  if (self->socketDescriptor < 0)
    // Already closed.  Do nothing.
    return NDN_ERROR_success;
  
  if (close(self->socketDescriptor) != 0)
    return NDN_ERROR_SocketTransport_error_in_close;
  
  self->socketDescriptor = -1;
  
  return NDN_ERROR_success;
}
