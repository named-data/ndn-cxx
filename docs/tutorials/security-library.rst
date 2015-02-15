Security Library Tutorial
=========================

.. contents::

Identity, Key and Certificates
------------------------------

All keys, certificates and their corresponding identities are managed by :ndn-cxx:`KeyChain`.

An real world **identity** can be expressed by a namespace.  (e.g.,
``/ndn/edu/ucla/alice``, or ``/ndn/edu/ucla/BoelterHall/4805``).

**Keys** belonging to an identity are named under the identity's namespace, with a unique
**KeyId**::

    /<identity_name>/[KeyId]

For now, only two types of KeyId are specified: ``ksk-[timestamp]`` and
``dsk-[timestamp]``.  The first type of KeyId is used to denote Key-Signing-Key (KSK)
which is supposed to have a long lifetime.  The second type of KeyId is used to denote
Data-Signing-Key (DSK) which is supposed to have a short lifetime.  Both types of KeyId
use timestamps (number of milliseconds since unix epoch) to provide relative uniqueness of
key names.  Replacing timestamp with key hash can bring stronger uniqueness but on the
cost of longer name.  Therefore, key hash is not used for now.  For example,
``/ndn/edu/ucla/alice/ksk-1234567890`` or
``/ndn/edu/ucla/BoelterHall/4805/dsk-1357924680``.

An identity may have more than one keys associated with it.  For example, one may have a
KSK to sign other keys and a DSK to sign data packets, or one may periodically replace its
expired DSK/KSK with a new key.

The private part of a key ("private key"), is stored in a :ndn-cxx:`Trusted Platform
Module (TPM) <SecTpm>`.  The public part ("public key"), is managed in a
:ndn-cxx:`Public-key Information Base (PIB) <SecPublicInfo>`.  The most important
information managed by PIB is **certificates** of public keys.  A certificate binds a
public key to its key name or the corresponding identity.  The signer (or issuer) of a
certificate vouches for the binding through its own signature.  With different signers
vouching for the binding, a public key may have more than one certificates.

The certificate name follows the naming convention of `NDNS (NDN Domain Name Service) <http://lasr.cs.ucla.edu/afanasyev/data/files/Afanasyev/afanasyev-phd-thesis.pdf>`_.  The
public key name will be broken into two parts:

- The first part ("authoritative namespace") will be put before a name component ``KEY``
  which serves as an application tag
- The second part ("label") will be put between ``KEY`` and ``ID-CERT`` which serves as an
  indicator of certificate.

A version number of the certificate is appended after ``ID-CERT``.  For example,
``/ndn/edu/ucla/KEY/alice/ksk-1234567890/ID-CERT/%FD%01`` or
``/ndn/edu/ucla/BoelterHall/4805/KEY/dsk-1357924680/ID-CERT/%FD%44``.

The :ndn-cxx:`NDN certificate <IdentityCertificate>` is just an ordinary `NDN-TLV Data
packet <http://named-data.net/doc/ndn-tlv/data.html>`_, with the content part in DER
encoding that resembles X.509 certificate:

.. code-block:: cpp

    // NDN-TLV Encoding
    Certificate ::= DATA-TLV TLV-LENGTH
                      Name
                      MetaInfo (= CertificateMetaInfo)
                      Content (= CertificateContent)
                      Signature

    CertificateMetaInfo ::= META-INFO-TYPE TLV-LENGTH
                              ContentType (= KEY)
                              FreshnessPeriod (= ?)


    CertificateContent ::= CONTENT-TYPE TLV-LENGTH
                             CertificateDerPayload


    // DER Encoding
    CertificateDerPayload ::= SEQUENCE {
        validity            Validity,
        subject             Name,
        subjectPubKeyInfo   SubjectPublicKeyInfo,
        extension           Extensions OPTIONAL   }

    Validity ::= SEQUENCE {
        notBefore           Time,
        notAfter            Time   }

    Time ::= CHOICE {
        GeneralizedTime   }

    Name ::= CHOICE {
        RDNSequence   }

    RDNSequence ::= SEQUENCE OF RelativeDistinguishedName

    RelativeDistinguishedName ::=
        SET OF AttributeTypeAndValue

    SubjectPublicKeyInfo ::= SEQUENCE {
        algorithm           AlgorithmIdentifier
        keybits             BIT STRING   }

    Extensions ::= SEQUENCE SIZE (1..MAX) OF Extension

See `RFC 3280 <http://www.ietf.org/rfc/rfc3280.txt>`_ for more details about DER field
definitions.

Signing
-------

Key Management
%%%%%%%%%%%%%%

Create Identity/Keys/Certificate
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The simplest way to initialize an identity and its key and certificate is to call
:ndn-cxx:`KeyChain::createIdentity`

.. code-block:: cpp

    KeyChain keyChain;
    Name defaultCertName = keyChain.createIdentity(identity);

This method guarantees that the default key and certificate of the supplied identity
always exist in the KeyChain.  This method checks if the supplied identity has already had
a default key and a default certificate and returns the default certificate name if
exists.  If the default certificate is missing, KeyChain will automatically create a
self-signed certificate of the default key.  If the default key is missing, KeyChain will
automatically create a new key and set it as the default key and create a self-signed
certificate as well.

Create Keys Manually
~~~~~~~~~~~~~~~~~~~~

One can call :ndn-cxx:`KeyChain::generateRsaKeyPair` to generate an RSA key pair or
:ndn-cxx:`KeyChain::generateEcdsaKeyPair` to generate an ECDSA key.  Note that generated
key pair is not set as the default key of the identity, so you need to set it manually by
calling :ndn-cxx:`KeyChain::setDefaultKeyNameForIdentity`. There is also a helper method
:ndn-cxx:`KeyChain::generateRsaKeyPairAsDefault`, which combines the two steps into one.

.. code-block:: cpp

    KeyChain keyChain;
    Name alice("/ndn/test/alice");

    Name aliceKeyName = keyChain.generateRsaKeyPair(alice);
    keyChain.setDefaultKeyNameForIdentity(aliceKeyName);

    // Now the key with the name aliceKeyName2 becomes alice's default key
    Name aliceKeyName2 = keyChain.generateRsaKeyPairAsDefault(alice);

Create Identity Certificate Manually
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you have created a key pair, you can generate a self-signed certificate for the key by
calling :ndn-cxx:`KeyChain::selfSign`.

.. code-block:: cpp

    KeyChain keyChain;
    Name aliceKeyName("/ndn/test/alice/ksk-1394129695025");

    shared_ptr<IdentityCertificate> aliceCert = keyChain.selfSign(aliceKeyName);

You can sign a public key using a different key:

.. code-block:: cpp

    KeyChain keyChain;

    shared_ptr<IdentityCertificate> certificate =
      keyChain.prepareUnsignedIdentityCertificate(publicKeyName, publicKey,
                                                  signingIdentity,
                                                  notBefore, notAfter,
                                                  subjectDescription, prefix

    keyChain.signByIdentity(*certificate, signingIdentity);

Signing Data
%%%%%%%%%%%%

Although the security library does not have the intelligence to automatically determine
the signing key for each data packet, it still provides a mechanism, called **Default
Signing Settings**, to facilitate signing process.

The basic signing process in the security library would be like this: create :ndn-cxx:`KeyChain`
instance and supply the data packet and signing certificate name to :ndn-cxx:`KeyChain::sign`
method.

.. code-block:: cpp

    KeyChain keyChain;
    keyChain.sign(dataPacket, signingCertificateName);

The :ndn-cxx:`KeyChain` instance will

- construct ``SignatureInfo`` using the signing certificate name;
- look up the corresponding private key in :ndn-cxx:`TPM <SecTpm>`;
- sign the data packet if the private key exists.

The basic process, however, requires application developers to supply the exact
certificate name.  Such a process has two shortages: first, it might be difficult to
remember the certificate name; second, application developers have to be aware of
certificate update and key roll-over.  Therefore, the security library provides another
signing process in which application developers only need to supply the signing identity:

.. code-block:: cpp

    KeyChain keyChain;
    keyChain.signByIdentity(dataPacket, signingIdentity);

The :ndn-cxx:`KeyChain` instance will

- determine the default key of the signing identity;
- determine the default certificate of the key;
- construct ``SignatureInfo`` using the default certificate name;
- look up the corresponding private key in :ndn-cxx:`TPM <SecTpm>`;
- sign the data packet if the private key exists.

The process above requires that each identity has a default key and that each key has a
default certificate.  All these default settings is managed in :ndn-cxx:`PIB
<SecPublicInfo>`, one can get/set these default settings through :ndn-cxx:`KeyChain`
directly:

.. code-block:: cpp

    KeyChain keyChain;
    Name defaultKeyName = keyChain.getDefaultKeyNameForIdentity(identity);
    Name defaultCertName = keyChain.getDefaultCertificateNameForKey(keyName);

    keyChain.setDefaultKeyNameForIdentity(keyName);
    keyChain.setDefaultCertificateNameForKey(certificateName);

There is even a default identity which will be used when no identity information is
supplied in signing method:

.. code-block:: cpp

    KeyChain keyChain;
    keyChain.sign(dataPacket);

And default identity can be got/set through :ndn-cxx:`KeyChain` as well:

.. code-block:: cpp

    KeyChain keyChain;
    Name defaultIdentity = keyChain.getDefaultIdentity();
    keyChain.setDefaultIdentity(identity);


Signing Interests
%%%%%%%%%%%%%%%%%

The process of signing Interests according to the :doc:`Signed Interest specification
<signed-interest>` is exactly the same as the process of signing Data packets:

.. code-block:: cpp

    KeyChain keyChain;

    keyChain.sign(interest, signingCertName);
    keyChain.signByIdentity(interest, signingIdentity);
    keyChain.sign(interest);

Validation
----------

Interest and Data validation is done through a **Validator**. :ndn-cxx:`Validator` is a virtual
class, two pure virtual methods must be implemented in order to construct a working
validator:

.. code-block:: cpp

    class Validator
    {
      ...
    protected:
      virtual void
      checkPolicy(const Data& data,
                  int nSteps,
                  const OnDataValidated& onValidated,
                  const OnDataValidationFailed& onValidationFailed,
                  std::vector<shared_ptr<ValidationRequest> >& nextSteps) = 0;

      virtual void
      checkPolicy(const Interest& interest,
                  int nSteps,
                  const OnInterestValidated& onValidated,
                  const OnInterestValidationFailed& onValidationFailed,
                  std::vector<shared_ptr<ValidationRequest> >& nextSteps) = 0;
      ...
    };

What should be implemented in these two methods is to check:

- whether the packet and signer comply with trust policies;
- whether their signature can be verified.

If the packet can be validated, the ``onValidated`` callback should be invoked, otherwise
the ``onValidationFailed`` callback should be invoked.  If more information (e.g., other
certificates) is needed, express the request for missing information in one or more
``ValidationRequest`` and push them into ``nextSteps``.

.. code-block:: cpp

    class ValidationRequest
    {
    public:
      Interest m_interest;                      // The Interest for the requested data/certificate.
      OnDataValidated m_onValidated;            // Callback when the retrieved certificate is authenticated.
      OnDataValidationFailed m_onDataValidated; // Callback when the retrieved certificate cannot be authenticated.
      int m_nRetries;                           // The number of retries when the interest times out.
      int m_nStep;                              // The number of validation steps that have been performed.
    };

Besides the two ``Validator::checkPolicy`` methods, the ``Validator`` also provides three
hooks to control packet validation in a finer granularity.

.. code-block:: cpp

    class Validator
    {
      ...
    protected:
      virtual shared_ptr<const Data>
      preCertificateValidation(const Data& data);

      virtual void
      onTimeout(const Interest& interest,
                int nRemainingRetries,
                const OnFailure& onFailure,
                const shared_ptr<ValidationRequest>& validationRequest);

      virtual void
      afterCheckPolicy(const std::vector<shared_ptr<ValidationRequest> >& nextSteps,
                       const OnFailure& onFailure);
      ...
    };

``Validator::preCertificateValidation`` is triggered before validating requested
certificate.  The Data supplied matches the interest in the ``ValidationRequest``.  It may
be certificate or a data encapsulating certificate.  This hook returns a data (actually
certificate) that is will be passed as Data into ``Validator::validate``;

``Validator::onTimeout`` is triggered when interest for certificate times out.  The logic
to handle the timeout can be implemented in this hook.  One could invoke onFailure or
re-express the interest.

``Validator::afterCheckPolicy`` is invoked after ``Validator::checkPolicy`` is done.  One
can implement the logic of how to process the set of ValidationRequests according to its
trust model.

Configuration-based Validator
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

In most cases, the trust model of applications are simple.  However, it is not trivial to
implement the two ``Validator::checkPolicy`` methods.  Therefore, we provide a more
developer-friendly configuration-based validator, ``ValidatorConfig``.  With
``ValidatorConfig``, one can express the trust model using a policy language in a
configuration file.  See :doc:`security-validator-config` for more details.
