/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include "wire-format.hpp"

using namespace std;

namespace ndn {

static bool gotInitialDefaultWireFormat = false;

WireFormat *WireFormat::defaultWireFormat_ = 0;

WireFormat *WireFormat::getDefaultWireFormat()
{
  if (!defaultWireFormat_ && !gotInitialDefaultWireFormat) {
    // There is no defaultWireFormat_ and we have not yet initialized initialDefaultWireFormat_, so initialize and use it.
    gotInitialDefaultWireFormat = true;
    // NOTE: This allocates one object which we never free for the life of the application.
    defaultWireFormat_ = newInitialDefaultWireFormat();
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

ptr_lib::shared_ptr<vector<unsigned char> > WireFormat::encodeData
  (const Data &data, unsigned int *signedFieldsBeginOffset, unsigned int *signedFieldsEndOffset) 
{
  throw logic_error("unimplemented");
}
void WireFormat::decodeData
  (Data &data, const unsigned char *input, unsigned int inputLength, unsigned int *signedFieldsBeginOffset, unsigned int *signedFieldsEndOffset) 
{
  throw logic_error("unimplemented");
}

ptr_lib::shared_ptr<vector<unsigned char> > WireFormat::encodeForwardingEntry(const ForwardingEntry &forwardingEntry) 
{
  throw logic_error("unimplemented");
}
void WireFormat::decodeForwardingEntry(ForwardingEntry &forwardingEntry, const unsigned char *input, unsigned int inputLength) 
{
  throw logic_error("unimplemented");
}

}
