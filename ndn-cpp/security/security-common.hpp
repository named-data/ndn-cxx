/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Yingdi Yu <yingdi@cs.ucla.edu>
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_SECURITY_COMMON_HPP
#define	NDN_SECURITY_COMMON_HPP

namespace ndn {

enum KeyType {
  KEY_TYPE_RSA,
  // KEY_TYPE_DSA,
  KEY_TYPE_AES,
  // KEY_TYPE_DES,
  // KEY_TYPE_RC4,
  // KEY_TYPE_RC2
};

enum KeyClass {
  KEY_CLASS_PUBLIC,
  KEY_CLASS_PRIVATE,
  KEY_CLASS_SYMMETRIC
};
  
enum KeyFormat {
  KEY_FORMAT_PUBLIC_OPENSSL,
};

enum DigestAlgorithm {
  // DIGEST_ALGORITHM_MD2,
  // DIGEST_ALGORITHM_MD5,
  // DIGEST_ALGORITHM_SHA1,
  DIGEST_ALGORITHM_SHA256
};

enum EncryptMode {
  ENCRYPT_MODE_DEFAULT,
  ENCRYPT_MODE_CFB_AES,
  // ENCRYPT_MODE_CBC_AES
};

}

#endif
