/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <cstdlib>
#include <sstream>
#include <iostream>
#include <ndn-cpp/Interest.hpp>
#include <ndn-cpp/ContentObject.hpp>
#include <ndn-cpp/c/network/TcpTransport.h>

using namespace std;
using namespace ndn;
int main(int argc, char** argv)
{
  try {
    Interest interest;    
    interest.getName() = Name("/ndn/ucla.edu/apps/ndn-js-test/hello.txt/level2/%FD%05%0B%16%7D%95%0E");
    vector<unsigned char> encoding;
    interest.encode(encoding);

    struct ndn_TcpTransport transport;
    ndn_TcpTransport_init(&transport);
    ndn_Error error;
    if (error = ndn_TcpTransport_connect(&transport, (char *)"E.hub.ndn.ucla.edu", 9695))
      return error;
    if (error = ndn_TcpTransport_send(&transport, &encoding[0], encoding.size()))
      return error;

    unsigned char buffer[8000];
    unsigned int nBytes;
    while (1) {
      if (error = ndn_TcpTransport_receive(&transport, buffer, sizeof(buffer), &nBytes))
        return error;
      if (buffer[0] == 0x04)
        break;    
    }
    
    for (int i = 0; i < nBytes; ++i)
      printf("%02X ", (unsigned int)buffer[i]);
    
    ContentObject contentObject;
    contentObject.decode(buffer, nBytes);
  } catch (exception &e) {
    cout << "exception: " << e.what() << endl;
  }
  return 0;
}
