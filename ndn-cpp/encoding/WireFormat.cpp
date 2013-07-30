/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include "WireFormat.hpp"

using namespace std;

namespace ndn {
ptr_lib::shared_ptr<vector<unsigned char> > WireFormat::encodeInterest(const Interest &interest) 
{
  throw logic_error("unimplemented");
}
void WireFormat::decodeInterest(Interest &interest, const unsigned char *input, unsigned int inputLength) 
{
  throw logic_error("unimplemented");
}

ptr_lib::shared_ptr<vector<unsigned char> > WireFormat::encodeContentObject(const ContentObject &contentObject) 
{
  throw logic_error("unimplemented");
}
void WireFormat::decodeContentObject(ContentObject &contentObject, const unsigned char *input, unsigned int inputLength) 
{
  throw logic_error("unimplemented");
}

}
