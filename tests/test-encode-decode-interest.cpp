/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <cstdlib>
#include <sstream>
#include <iostream>
#include "../ndn-cpp/interest.hpp"

using namespace std;
using namespace ndn;

unsigned char Interest1[] = {
0x01, 0xd2,
  0xf2, 0xfa, 0x9d, 0x6e, 0x64, 0x6e, 0x00, 0xfa, 0x9d, 0x61, 0x62, 0x63, 0x00, 0x00, 
  0x05, 0x9a, 0x9e, 0x31, 0x32, 0x33, 0x00, 
  0x05, 0xa2, 0x8e, 0x34, 0x00,
  0x03, 0xe2, 
    0x02, 0x85, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 
  0x00,
  0x02, 0xda, 0xfa, 0x9d, 0x61, 0x62, 0x63, 0x00, 0xea, 0x00, 0x00, 
  0x05, 0xaa, 0x8e, 0x31, 0x00, 
  0x02, 0xfa, 0x8e, 0x34, 0x00, 
  0x02, 0xd2, 0x8e, 0x32, 0x00, 
  0x03, 0x82, 0x9d, 0x01, 0xe0, 0x00, 0x00, 
  0x02, 0xca, 0xb5, 0x61, 0x62, 0x61, 0x62, 0x61, 0x62, 0x00, 
0x00, 
1
};

static void dumpInterest(const Interest& interest)
{
  cout << "name: " << interest.getName().to_uri() << endl;
  cout << "minSuffixComponents: ";
  if (interest.getMinSuffixComponents() >= 0)
    cout << interest.getMinSuffixComponents() << endl;
  else
    cout << "<none>" << endl;
  cout << "maxSuffixComponents: ";
  if (interest.getMaxSuffixComponents() >= 0)
    cout << interest.getMaxSuffixComponents() << endl;
  else
    cout << "<none>" << endl;
  cout << "publisherPublicKeyDigest: " 
       << (interest.getPublisherPublicKeyDigest().getPublisherPublicKeyDigest().size() > 0 ? toHex(*interest.getPublisherPublicKeyDigest().getPublisherPublicKeyDigest()) : "<none>") << endl;
  cout << "exclude: " 
       << (interest.getExclude().getEntryCount() > 0 ? interest.getExclude().toUri() : "<none>") << endl;
  cout << "lifetimeMilliseconds: ";
  if (interest.getInterestLifetimeMilliseconds() >= 0)
    cout << interest.getInterestLifetimeMilliseconds() << endl;
  else
    cout << "<none>" << endl;
  cout << "childSelector: ";
  if (interest.getChildSelector() >= 0)
    cout << interest.getChildSelector() << endl;
  else
    cout << "<none>" << endl;
  cout << "answerOriginKind: ";
  if (interest.getAnswerOriginKind() >= 0)
    cout << interest.getAnswerOriginKind() << endl;
  else
    cout << "<none>" << endl;
  cout << "scope: ";
  if (interest.getScope() >= 0)
    cout << interest.getScope() << endl;
  else
    cout << "<none>" << endl;
  cout << "nonce: " 
       << (interest.getNonce().size() > 0 ? toHex(*interest.getNonce()) : "<none>") << endl;
}

int main(int argc, char** argv)
{
  try {
    Interest interest;
    interest.wireDecode(Interest1, sizeof(Interest1));
    cout << "Interest:" << endl;
    dumpInterest(interest);
    
    Blob encoding = interest.wireEncode();
    cout << endl << "Re-encoded interest " << toHex(*encoding) << endl;

    Interest reDecodedInterest;
    reDecodedInterest.wireDecode(*encoding);
    cout << "Re-decoded Interest:" << endl;
    dumpInterest(reDecodedInterest);
  } catch (exception& e) {
    cout << "exception: " << e.what() << endl;
  }
  return 0;
}
