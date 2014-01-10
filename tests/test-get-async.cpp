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

  void onData(const ptr_lib::shared_ptr<const Interest>& interest, const ptr_lib::shared_ptr<Data>& data)
  {
    ++callbackCount_;
    cout << "Got data packet with name " << data->getName().toUri() << endl;
    // cout << string(reinterpret_cast<const char*>(data->getContent().value()), data->getContent().value_size()) << endl;
  }

  void onTimeout(const ptr_lib::shared_ptr<const Interest>& interest)
  {
    ++callbackCount_;
    cout << "Time out for interest " << interest->getName().toUri() << endl;
  }

  int callbackCount_;
};

int main(int argc, char** argv)
{
  try {
    Face face;

    // Counter holds data used by the callbacks.
    Counter counter;

    Name name1("/%C1.M.S.localhost/%C1.M.SRV/ndnd/KEY");
    cout << "Express name " << name1.toUri() << endl;
    // Use bind to pass the counter object to the callbacks.
    face.expressInterest(name1, bind(&Counter::onData, &counter, _1, _2), bind(&Counter::onTimeout, &counter, _1));

    Name name2("/ndnx/ping");
    cout << "Express name " << name2.toUri() << endl;
    face.expressInterest(name2, bind(&Counter::onData, &counter, _1, _2), bind(&Counter::onTimeout, &counter, _1));

    Name name3("/test/timeout");
    cout << "Express name " << name3.toUri() << endl;
    face.expressInterest(name3, bind(&Counter::onData, &counter, _1, _2), bind(&Counter::onTimeout, &counter, _1));

    // The main event loop.
    face.processEvents();

  } catch (std::exception& e) {
    cout << "exception: " << e.what() << endl;
  }
  return 0;
}
