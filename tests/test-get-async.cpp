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
#include <ndn-cpp/NDN.hpp>

using namespace std;
using namespace ndn;
using namespace ptr_lib;

class MyClosure : public Closure {
public:
  MyClosure()
  : gotContent_(false)
  {  
  }
  
  virtual UpcallResult upcall(UpcallKind kind, UpcallInfo &upcallInfo)
  {
    if (kind == UPCALL_CONTENT || kind == UPCALL_CONTENT_UNVERIFIED) {
      gotContent_ = true;
      cout << "Got content with name " << upcallInfo.getContentObject()->getName().to_uri() << endl;
      for (unsigned int i = 0; i < upcallInfo.getContentObject()->getContent().size(); ++i)
        cout << upcallInfo.getContentObject()->getContent()[i];
      cout << endl;
      
      return CLOSURE_RESULT_OK;
    }
    else
      return CLOSURE_RESULT_OK;
  }
  
  bool gotContent_;
};

int main(int argc, char** argv)
{
  try {
    shared_ptr<TcpTransport> transport(new TcpTransport());
    shared_ptr<MyClosure> closure(new MyClosure());
    NDN ndn(transport, "E.hub.ndn.ucla.edu", 9695);
    ndn.expressInterest(Name("/ndn/ucla.edu/apps/ndn-js-test/hello.txt/level2/%FD%05%0B%16%7D%95%0E"), closure, 0);
    
    // Pump the receive process.  This should really be done by a socket listener.
    while (!closure->gotContent_)
      transport->tempReceive();    
  } catch (std::exception &e) {
    cout << "exception: " << e.what() << endl;
  }
  return 0;
}
