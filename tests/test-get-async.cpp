/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <cstdlib>
#include <sstream>
#include <iostream>
#include <ndn-cpp/Interest.hpp>
#include <ndn-cpp/ContentObject.hpp>
#include <ndn-cpp/encoding/BinaryXMLStructureDecoder.hpp>
#include <ndn-cpp/transport/TcpTransport.hpp>

using namespace std;
using namespace ndn;
int main(int argc, char** argv)
{
  try {
    Interest interest;    
    interest.getName() = Name("/ndn/ucla.edu/apps/ndn-js-test/hello.txt/level2/%FD%05%0B%16%7D%95%0E");
    vector<unsigned char> encoding;
    interest.encode(encoding);

    TcpTransport transport;
    transport.connect((char *)"E.hub.ndn.ucla.edu", 9695);
    transport.send(&encoding[0], encoding.size());

    BinaryXMLStructureDecoder structureDecoder;
    vector<unsigned char> element;
    while (true) {
      while (true) {
        unsigned char buffer[8000];
        unsigned int nBytes = transport.receive(buffer, sizeof(buffer));
        element.insert(element.end(), buffer, buffer + nBytes);

        if (structureDecoder.findElementEnd(&element[0], element.size()))
          break;
      }
      
      if (element[0] == 0x04)
        // Assume this is a ContentObject.
        break;
      
      // Erase this element and try again.
      element.erase(element.begin(), element.begin() + structureDecoder.getOffset());
    }
        
    ContentObject contentObject;
    contentObject.decode(&element[0], structureDecoder.getOffset());
    
    cout << "Got content with name " << contentObject.getName().to_uri() << endl;
    for (unsigned int i = 0; i < contentObject.getContent().size(); ++i)
      cout << contentObject.getContent()[i];
    cout << endl;
  } catch (exception &e) {
    cout << "exception: " << e.what() << endl;
  }
  return 0;
}
