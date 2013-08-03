/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include "WireFormat.hpp"
#include "BinaryXMLWireFormat.hpp"

using namespace std;

namespace ndn {

ptr_lib::shared_ptr<WireFormat> WireFormat::initialDefaultWireFormat_(newInitialDefaultWireFormat());

WireFormat *WireFormat::defaultWireFormat_ = initialDefaultWireFormat_.get();

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
