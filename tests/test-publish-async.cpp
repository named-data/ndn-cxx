/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <cstdlib>
#include <sstream>
#include <iostream>
#include <time.h>
#include "../ndn-cpp/face.hpp"
#include "../ndn-cpp/key-chain.hpp"

using namespace std;
using namespace ndn;
using namespace ptr_lib;
using namespace func_lib;

class Echo {
public:
  Echo() 
  { 
    interestCount_ = 0;
  }
  
  void operator()(const ptr_lib::shared_ptr<const Name> &prefix, const ptr_lib::shared_ptr<const Interest> &interest) {
    ++interestCount_;
    
    // Make and sign a Data packet.
    Data data(interest->getName());
    string content(string("Echo ") + interest->getName().toUri());
    data.setContent((const unsigned char*)&content[0], sizeof(content));
    data.getSignedInfo().setTimestampMilliseconds(time(NULL) * 1000.0);
    KeyChain::defaultSign(data);

    // TODO: Need to put the Data.
  }

  int interestCount_;
};

int main(int argc, char** argv)
{
  try {
    Face face("localhost");
    
    Echo echo;
    Name prefix("/testecho");
    cout << "Register prefix  " << prefix.toUri() << endl;
    face.registerPrefix(prefix, ref(echo));
    
    // The main event loop.
    while (echo.interestCount_ < 1) {
      face.processEvents();
      // We need to sleep for a few milliseconds so we don't use 100% of the CPU.
      usleep(10000);
    }
  } catch (std::exception &e) {
    cout << "exception: " << e.what() << endl;
  }
  return 0;
}
