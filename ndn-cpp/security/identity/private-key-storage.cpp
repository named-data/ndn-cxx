/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include "private-key-storage.hpp"

using namespace std;

namespace ndn {

PrivateKeyStorage::~PrivateKeyStorage()
{
}

// TODO: Move to subclass.
Blob 
PrivateKeyStorage::sign(const Blob& blob, const string& keyName, DigestAlgorithm digestAlgorithm)
{
  return Blob();
}

}
