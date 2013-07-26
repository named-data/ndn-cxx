/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <stdexcept>
#include "WireFormat.hpp"

using namespace std;

namespace ndn {
void WireFormat::encodeInterest(const Interest &interest, vector<unsigned char> &output) 
{
  throw logic_error("unimplemented");
}
void WireFormat::decodeInterest(Interest &interest, const unsigned char *input, unsigned int inputLength) 
{
  throw logic_error("unimplemented");
}

void WireFormat::encodeContentObject(const ContentObject &contentObject, vector<unsigned char> &output) 
{
  throw logic_error("unimplemented");
}
void WireFormat::decodeContentObject(ContentObject &contentObject, const unsigned char *input, unsigned int inputLength) 
{
  throw logic_error("unimplemented");
}

}
