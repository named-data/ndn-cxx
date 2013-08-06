/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <cstdlib>
#include <sstream>
#include <iostream>
#include <ndn-cpp/interest.hpp>
#include <ndn-cpp/data.hpp>
#include <ndn-cpp/transport/udp-transport.hpp>
#include <ndn-cpp/face.hpp>

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
    if (kind == UPCALL_DATA || kind == UPCALL_DATA_UNVERIFIED) {
      gotContent_ = true;
      cout << "Got data packet with name " << upcallInfo.getData()->getName().to_uri() << endl;
      for (unsigned int i = 0; i < upcallInfo.getData()->getContent().size(); ++i)
        cout << upcallInfo.getData()->getContent()[i];
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
    shared_ptr<MyClosure> closure(new MyClosure());
    Face face("E.hub.ndn.ucla.edu", 9695, shared_ptr<UdpTransport>(new UdpTransport()));
    face.expressInterest(Name("/ndn/ucla.edu/apps/ndn-js-test/hello.txt/level2/%FD%05%0B%16%7D%95%0E"), closure);
    
    // Pump the receive process.  This should really be done by a socket listener.
    while (!closure->gotContent_)
      face.getTransport()->tempReceive();    
  } catch (std::exception &e) {
    cout << "exception: " << e.what() << endl;
  }
  return 0;
}
