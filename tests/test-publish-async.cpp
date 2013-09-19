/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#include <cstdlib>
#include <sstream>
#include <iostream>
#include <time.h>
#include "../ndn-cpp/face.hpp"
#include "../ndn-cpp/security/key-chain.hpp"

using namespace std;
using namespace ndn;
using namespace ptr_lib;
using namespace func_lib;

class Echo {
public:
  Echo(KeyChain &keyChain)
  : keyChain_(keyChain), responseCount_(0)
  { 
  }
  
  // onInterest.
  void operator()
     (const shared_ptr<const Name>& prefix, const shared_ptr<const Interest>& interest, Transport& transport) 
  {
    ++responseCount_;
    
    // Make and sign a Data packet.
    Data data(interest->getName());
    string content(string("Echo ") + interest->getName().toUri());
    data.setContent((const unsigned char *)&content[0], content.size());
    data.getMetaInfo().setTimestampMilliseconds(time(NULL) * 1000.0);
    keyChain_.signData(data);
    Blob encodedData = data.wireEncode();

    cout << "Sent content " << content << endl;
    transport.send(*encodedData);
  }
  
  // onRegisterFailed.
  void operator()(const ptr_lib::shared_ptr<const Name>& prefix)
  {
    ++responseCount_;
    cout << "Register failed for prefix " << prefix->toUri() << endl;
  }

  KeyChain keyChain_;
  int responseCount_;
};

int main(int argc, char** argv)
{
  try {
    Face face("localhost");
    
    shared_ptr<PrivateKeyStorage> privateKeyStorage(new PrivateKeyStorage());
    shared_ptr<IdentityManager> identityManager(new IdentityManager(privateKeyStorage));
    KeyChain keyChain(identityManager);
    keyChain.setFace(&face);
   
    Echo echo(keyChain);
    Name prefix("/testecho");
    cout << "Register prefix  " << prefix.toUri() << endl;
    face.registerPrefix(prefix, ref(echo), ref(echo), keyChain, Name());
    
    // The main event loop.  
    // Wait forever to receive one interest for the prefix.
    while (echo.responseCount_ < 1) {
      face.processEvents();
      // We need to sleep for a few milliseconds so we don't use 100% of the CPU.
      usleep(10000);
    }
  } catch (std::exception& e) {
    cout << "exception: " << e.what() << endl;
  }
  return 0;
}
