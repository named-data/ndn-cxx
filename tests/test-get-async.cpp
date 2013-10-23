/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <cstdlib>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <ndn-cpp/face.hpp>

using namespace std;
using namespace ndn;
using namespace ndn::ptr_lib;
using namespace ndn::func_lib;
#if NDN_CPP_HAVE_STD_FUNCTION
// In the std library, the placeholders are in a different namespace than boost.
using namespace func_lib::placeholders;
#endif

class Counter
{
public:
  Counter() {
    callbackCount_ = 0;
  }
  
  void onData(const shared_ptr<const Interest>& interest, const shared_ptr<Data>& data)
  {
    ++callbackCount_;
    cout << "Got data packet with name " << data->getName().to_uri() << endl;
    for (size_t i = 0; i < data->getContent().size(); ++i)
      cout << (*data->getContent())[i];
    cout << endl;  
  }

  void onTimeout(const shared_ptr<const Interest>& interest)
  {
    ++callbackCount_;
    cout << "Time out for interest " << interest->getName().toUri() << endl;    
  }
  
  int callbackCount_;
};

int main(int argc, char** argv)
{
  try {
    // Connect to port 9695 until the testbed hubs use NDNx.
    Face face("C.hub.ndn.ucla.edu", 9695);
    
    // Counter holds data used by the callbacks.
    Counter counter;
    
    Name name1("/ndn/ucla.edu/apps/ndn-js-test/hello.txt/level2/%FD%05%0B%16%7D%95%0E");    
    cout << "Express name  " << name1.toUri() << endl;
    // Use bind to pass the counter object to the callbacks.
    face.expressInterest(name1, bind(&Counter::onData, &counter, _1, _2), bind(&Counter::onTimeout, &counter, _1));
    
    Name name2("/ndn/ucla.edu/apps/lwndn-test/howdy.txt/%FD%05%05%E8%0C%CE%1D");
    cout << "Express name  " << name2.toUri() << endl;
    face.expressInterest(name2, bind(&Counter::onData, &counter, _1, _2), bind(&Counter::onTimeout, &counter, _1));
    
    Name name3("/test/timeout");
    cout << "Express name  " << name3.toUri() << endl;
    face.expressInterest(name3, bind(&Counter::onData, &counter, _1, _2), bind(&Counter::onTimeout, &counter, _1));

    // The main event loop.
    while (counter.callbackCount_ < 3) {
      face.processEvents();
      // We need to sleep for a few milliseconds so we don't use 100% of the CPU.
      usleep(10000);
    }
  } catch (std::exception& e) {
    cout << "exception: " << e.what() << endl;
  }
  return 0;
}
