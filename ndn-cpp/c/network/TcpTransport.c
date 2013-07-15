/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include "TcpTransport.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

// get sockaddr, IPv4 or IPv6:
static inline void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr);

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

ndn_Error ndn_TcpTransport_connect(ndn_TcpTransport *self, char *host, int port)
{
  
}

int testTcpTransport(unsigned char *data, unsigned int dataLength) 
{
	struct addrinfo hints, *serverInfo;

  printf("starting\n");

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo("E.hub.ndn.ucla.edu", "9695", &hints, &serverInfo) != 0)
		return 1;

	// loop through all the results and connect to the first we can
	struct addrinfo *p;
	for(p = serverInfo; p != NULL; p = p->ai_next) {
		if ((socketDescriptor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
			continue;

		if (connect(socketDescriptor, p->ai_addr, p->ai_addrlen) == -1) {
			close(socketDescriptor);
			continue;
		}

		break;
	}

	if (p == NULL)
		return 2;

	freeaddrinfo(serverInfo); // all done with this structure

  int nBytes;
  while (1) {
    if ((nBytes = send(socketDescriptor, data, dataLength, 0)) < 0)
      return 1;
    if (nBytes >= dataLength)
      break;
    
    dataLength -= nBytes;
  }
  
	unsigned char buffer[1000];
	if ((nBytes = recv(socketDescriptor, buffer, sizeof(buffer) - 1, 0)) == -1)
    return 1;

	printf("received %d bytes\n", nBytes);
  int i;
  for (i = 0; i < nBytes; ++i)
    printf("%02X ", (unsigned int)buffer[i]);
  printf("\n");

	close(socketDescriptor);

	return 0;
}
