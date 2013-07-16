/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <cstdlib>
#include <sstream>
#include <iostream>
#include <ndn-cpp/Interest.hpp>
#include <ndn-cpp/ContentObject.hpp>
#include <ndn-cpp/transport/TcpTransport.hpp>
#include <ndn-cpp/c/encoding/BinaryXMLElementReader.h>
#include <ndn-cpp/NDN.hpp>

using namespace std;
using namespace ndn;

class MyClosure : public Closure {
public:
  virtual UpcallResult upcall(UpcallKind kind, UpcallInfo &upcallInfo)
  {
    if (kind == UPCALL_CONTENT || kind == UPCALL_CONTENT_UNVERIFIED) {
      cout << "Got content with name " << upcallInfo.getContentObject()->getName().to_uri() << endl;
      for (unsigned int i = 0; i < upcallInfo.getContentObject()->getContent().size(); ++i)
        cout << upcallInfo.getContentObject()->getContent()[i];
      cout << endl;
      
      return CLOSURE_RESULT_OK;
    }
    else
      return CLOSURE_RESULT_OK;
  }
};

int main(int argc, char** argv)
{
  try {
    NDN ndn(ptr_lib::make_shared<TcpTransport>(), "E.hub.ndn.ucla.edu", 9695, ptr_lib::make_shared<MyClosure>());
    ndn.expressInterest(Name("/ndn/ucla.edu/apps/ndn-js-test/hello.txt/level2/%FD%05%0B%16%7D%95%0E"), ptr_lib::make_shared<MyClosure>(), 0);
    
    ndn_BinaryXMLElementReader elementReader;
    ndn_BinaryXMLElementReader_init(&elementReader, (struct ndn_ElementListener *)&ndn);
    
    unsigned char buffer[8000];
    unsigned int nBytes = ndn.tempGetTransport().receive(buffer, sizeof(buffer));
    ndn_BinaryXMLElementReader_onReceivedData(&elementReader, buffer, nBytes);    
  } catch (exception &e) {
    cout << "exception: " << e.what() << endl;
  }
  return 0;
}
