/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <cstdlib>
#include <sstream>
#include <iostream>
#include <ndn-cpp/Interest.hpp>

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

/*
 * 
 */
int main(int argc, char** argv)
{
  try {
    Interest interest;
    interest.decode(Interest1, sizeof(Interest1));
    cout << "Interest name " << interest.getName().to_uri() << endl;
    cout << "Interest minSuffixComponents " << interest.getMinSuffixComponents() << endl;
    cout << "Interest excludeEntryCount " << interest.getExclude().getEntryCount() << endl;
    cout << "InterestLifetime " << interest.getInterestLifetime() << endl;
    
    vector<unsigned char> encoding;
    interest.encode(encoding);
    cout << "Interest encoding length " << encoding.size() << " vs. sizeof(Interest1) " << sizeof(Interest1) << endl;
    
    Interest reDecodedInterest;
    reDecodedInterest.decode(encoding);
    cout << "Re-decoded Interest name " << reDecodedInterest.getName().to_uri() << endl;
    cout << "Re-decoded Interest minSuffixComponents " << reDecodedInterest.getMinSuffixComponents() << endl;
    cout << "Re-decoded Interest excludeEntryCount " << reDecodedInterest.getExclude().getEntryCount() << endl;
    cout << "Re-decoded InterestLifetime " << reDecodedInterest.getInterestLifetime() << endl;
  } catch (exception &e) {
    cout << "exception: " << e.what() << endl;
  }
  return 0;
}
