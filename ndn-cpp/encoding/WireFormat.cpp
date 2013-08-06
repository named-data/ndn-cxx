/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include "WireFormat.hpp"

using namespace std;

namespace ndn {

ptr_lib::shared_ptr<WireFormat> WireFormat::initialDefaultWireFormat_;

WireFormat *WireFormat::defaultWireFormat_ = 0;

WireFormat *WireFormat::getDefaultWireFormat()
{
  if (!defaultWireFormat_ && !initialDefaultWireFormat_) {
    // There is no defaultWireFormat_ and we have not yet initialized initialDefaultWireFormat_, so initialize and use it.
    // NOTE: This could have been done with a static initializer on initialDefaultWireFormat_, but this does not have
    //   good cross-platform support, especially for dynamic shared libraries.
    initialDefaultWireFormat_.reset(newInitialDefaultWireFormat());
    defaultWireFormat_ = initialDefaultWireFormat_.get();
  }
  
  return defaultWireFormat_;
}

ptr_lib::shared_ptr<vector<unsigned char> > WireFormat::encodeInterest(const Interest &interest) 
{
  throw logic_error("unimplemented");
}
void WireFormat::decodeInterest(Interest &interest, const unsigned char *input, unsigned int inputLength) 
{
  throw logic_error("unimplemented");
}

ptr_lib::shared_ptr<vector<unsigned char> > WireFormat::encodeData(const Data &data) 
{
  throw logic_error("unimplemented");
}
void WireFormat::decodeData(Data &data, const unsigned char *input, unsigned int inputLength) 
{
  throw logic_error("unimplemented");
}

}
