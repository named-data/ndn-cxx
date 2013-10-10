/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_MEMORY_IDENTITY_STORAGE_HPP
#define	NDN_MEMORY_IDENTITY_STORAGE_HPP

#include <vector>
#include "identity-storage.hpp"

namespace ndn {

/**
 * MemoryIdentityStorage extends IdentityStorage and implements its methods to store identity, public key and certificate objects in memory.
 * The application must get the objects through its own means and add the objects to the MemoryIdentityStorage object.
 * To use permanent file-based storage, see BasicIdentityStorage.
 */
class MemoryIdentityStorage : public IdentityStorage {
public:
  /**
   * The virtual Destructor.
   */
  virtual 
  ~MemoryIdentityStorage();

  /**
   * Check if the specified identity already exists.
   * @param identityName The identity name.
   * @return true if the identity exists, otherwise false.
   */
  virtual bool 
  doesIdentityExist(const Name& identityName);

  /**
   * Add a new identity. An exception will be thrown if the identity already exists.
   * @param identityName The identity name to be added.
   */
  virtual void
  addIdentity(const Name& identityName);

  /**
   * Revoke the identity.
   * @return true if the identity was revoked, false if not.
   */
  virtual bool 
  revokeIdentity();

  /**
   * Generate a name for a new key belonging to the identity.
   * @param identityName The identity name.
   * @param useKsk If true, generate a KSK name, otherwise a DSK name.
   * @return The generated key name.
   */
  virtual Name 
  getNewKeyName(const Name& identityName, bool useKsk);

  /**
   * Check if the specified key already exists.
   * @param keyName The name of the key.
   * @return true if the key exists, otherwise false.
   */
  virtual bool 
  doesKeyExist(const Name& keyName);

  /**
   * Extract the key name from the certificate name.
   * @param certificateName The certificate name to be processed.
   */
  virtual Name 
  getKeyNameForCertificate(const Name& certificateName);

  /**
   * Add a public key to the identity storage.
   * @param keyName The name of the public key to be added.
   * @param keyType Type of the public key to be added.
   * @param publicKeyDer A blob of the public key DER to be added.
   */
  virtual void 
  addKey(const Name& keyName, KeyType keyType, const Blob& publicKeyDer);

  /**
   * Get the public key DER blob from the identity storage.
   * @param keyName The name of the requested public key.
   * @return The DER Blob.  If not found, return a Blob with a null pointer.
   */
  virtual Blob
  getKey(const Name& keyName);

  /**
   * Activate a key.  If a key is marked as inactive, its private part will not be used in packet signing.
   * @param keyName name of the key
   */
  virtual void 
  activateKey(const Name& keyName);

  /**
   * Deactivate a key. If a key is marked as inactive, its private part will not be used in packet signing.
   * @param keyName name of the key
   */
  virtual void 
  deactivateKey(const Name& keyName);

  /**
   * Check if the specified certificate already exists.
   * @param certificateName The name of the certificate.
   * @return true if the certificate exists, otherwise false.
   */
  virtual bool
  doesCertificateExist(const Name& certificateName);

  /**
   * Add a certificate to the identity storage.
   * @param certificate The certificate to be added.
   */
  virtual void 
  addCertificate(const Certificate& certificate);

  /**
   * Get a certificate from the identity storage.
   * @param certificateName The name of the requested certificate.
   * @param allowAny If false, only a valid certificate will be returned, otherwise validity is disregarded.
   * @return The requested certificate.  If not found, return a shared_ptr with a null pointer.
   */
  virtual ptr_lib::shared_ptr<Certificate> 
  getCertificate(const Name &certificateName, bool allowAny = false);


  /*****************************************
   *           Get/Set Default             *
   *****************************************/

  /**
   * Get the default identity. 
   * @param return The name of default identity, or an empty name if there is no default.
   */
  virtual Name 
  getDefaultIdentity();

  /**
   * Get the default key name for the specified identity.
   * @param identityName The identity name.
   * @return The default key name.
   */
  virtual Name 
  getDefaultKeyNameForIdentity(const Name& identityName);

  /**
   * Get the default certificate name for the specified key.
   * @param keyName The key name.
   * @return The default certificate name.
   */
  virtual Name 
  getDefaultCertificateNameForKey(const Name& keyName);

  /**
   * Set the default identity.  If the identityName does not exist, then clear the default identity
   * so that getDefaultIdentity() returns an empty name.
   * @param identityName The default identity name.
   */
  virtual void 
  setDefaultIdentity(const Name& identityName);

  /**
   * Set the default key name for the specified identity.
   * @param keyName The key name.
   * @param identityNameCheck (optional) The identity name to check the keyName.
   */
  virtual void 
  setDefaultKeyNameForIdentity(const Name& keyName, const Name& identityNameCheck = Name());

  /**
   * Set the default key name for the specified identity.
   * @param keyName The key name.
   * @param certificateName The certificate name.
   */
  virtual void 
  setDefaultCertificateNameForKey(const Name& keyName, const Name& certificateName);  
  
private:
  std::vector<std::string> identityStore_; /**< A list of name URI. */
  std::string defaultIdentity_;            /**< The default identity in identityStore_, or "" if not defined. */
};

}

#endif
