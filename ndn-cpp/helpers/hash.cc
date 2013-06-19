/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013, Regents of the University of California
 *                     Alexander Afanasyev
 *                     Zhenkai Zhu
 *
 * BSD license, See the LICENSE file for more information
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 *         Zhenkai Zhu <zhenkai@cs.ucla.edu>
 */

#include "hash.h"
#include "ndn-cpp/helpers/uri.h"

#include <boost/lexical_cast.hpp>
#include <openssl/evp.h>
#include <fstream>

using namespace boost;
using namespace std;

// Other options: VP_md2, EVP_md5, EVP_sha, EVP_sha1, EVP_sha256, EVP_dss, EVP_dss1, EVP_mdc2, EVP_ripemd160
#define HASH_FUNCTION EVP_sha256

namespace ndn
{

std::ostream &
operator << (std::ostream &os, const Hash &hash)
{
  if (hash.m_length == 0)
    return os;

  ostreambuf_iterator<char> out_it (os); // ostream iterator
  // need to encode to base64
  copy (detail::string_from_binary (reinterpret_cast<const char*> (hash.m_buf)),
        detail::string_from_binary (reinterpret_cast<const char*> (hash.m_buf+hash.m_length)),
        out_it);

  return os;
}

std::string
Hash::shortHash () const
{
  return lexical_cast<string> (*this).substr (0, 10);
}


unsigned char Hash::_origin = 0;
ptr_lib::shared_ptr<Hash> Hash::Origin(new Hash(&Hash::_origin, sizeof(unsigned char)));

ptr_lib::shared_ptr<Hash>
Hash::FromString (const std::string &hashInTextEncoding)
{
  ptr_lib::shared_ptr<Hash> retval = make_shared<Hash> (reinterpret_cast<void*> (0), 0);

  if (hashInTextEncoding.size () == 0)
    {
      return retval;
    }

  if (hashInTextEncoding.size () > EVP_MAX_MD_SIZE * 2)
    {
      cerr << "Input hash is too long. Returning an empty hash" << endl;
      return retval;
    }

  retval->m_buf = new unsigned char [EVP_MAX_MD_SIZE];

  unsigned char *end = copy (detail::string_to_binary (hashInTextEncoding.begin ()),
                             detail::string_to_binary (hashInTextEncoding.end ()),
                             retval->m_buf);

  retval->m_length = end - retval->m_buf;

  return retval;
}

ptr_lib::shared_ptr<Hash>
Hash::FromFileContent (const char *filename)
{
  ptr_lib::shared_ptr<Hash> retval = make_shared<Hash> (reinterpret_cast<void*> (0), 0);
  retval->m_buf = new unsigned char [EVP_MAX_MD_SIZE];

  EVP_MD_CTX *hash_context = EVP_MD_CTX_create ();
  EVP_DigestInit_ex (hash_context, HASH_FUNCTION (), 0);

  ifstream iff (filename, std::ios::in | std::ios::binary);
  while (iff.good ())
    {
      char buf[1024];
      iff.read (buf, 1024);
      EVP_DigestUpdate (hash_context, buf, iff.gcount ());
    }

  retval->m_buf = new unsigned char [EVP_MAX_MD_SIZE];

  EVP_DigestFinal_ex (hash_context,
                      retval->m_buf, &retval->m_length);

  EVP_MD_CTX_destroy (hash_context);

  return retval;
}

ptr_lib::shared_ptr<Hash>
Hash::FromBytes (const ndn::Bytes &bytes)
{
  ptr_lib::shared_ptr<Hash> retval = make_shared<Hash> (reinterpret_cast<void*> (0), 0);
  retval->m_buf = new unsigned char [EVP_MAX_MD_SIZE];

  EVP_MD_CTX *hash_context = EVP_MD_CTX_create ();
  EVP_DigestInit_ex (hash_context, HASH_FUNCTION (), 0);

  // not sure whether it's bad to do so if bytes.size is huge
  EVP_DigestUpdate(hash_context, ndn::head(bytes), bytes.size());

  retval->m_buf = new unsigned char [EVP_MAX_MD_SIZE];

  EVP_DigestFinal_ex (hash_context,
                      retval->m_buf, &retval->m_length);

  EVP_MD_CTX_destroy (hash_context);

  return retval;
}

} // ndn
