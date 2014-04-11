Security Library Tutorial
=========================

Key Management
--------------

Identity, Key and Certificates
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

All keys, certificates and their corresponding identities are managed by
KeyChain.

Before signing a packet, you need to assure that the signing key and its
corresponding identity certificate exist in the KeyChain. The private
part of the signing key is used to generate signature, while the
identity certificate is used to constructed the KeyLocator.

In KeyChain, keys and certificates are managed in terms of identities
which are expressed by namespaces (e.g., ``/ndn/edu/ucla/irl/yingdi``, or
``/ndn/edu/ucla/boelter\_hall/room\_4805``). Each pair of keys belongs to
only one identity, and it is named by the identity name appended with a
key ID (e.g., ``/ndn/edu/ucla/irl/yingdi/ksk-1234567890``, or
``/ndn/edu/ucla/boelter\_hall/room\_4805/ksk-1357924680``). However, one
identity may have more than one pair of keys, but only one of them is
the **default key** of the identity. A key pair without any identity
certificates is not quite useful. A key pair may have more than one
identity certificates, but only one of them is the **default
certificate**. Therefore, for a given identity, there is at only one
default identity certificate, which is the default certificates of its
default key.

While keys and certificates can be created offline using NDN security
tools **ndnsec**, they can be created online using the KeyChain API. The
simplest way is to call ``KeyChain::createIdentity``.

.. code-block:: cpp

    KeyChain keyChain;
    Name identity("/ndn/test/alice");

    Name certificateName = keyChain.createIdentity(identity);

``KeyChain::createIdentity`` returns the default certificate name of the
supplied identity, and always assures that the supplied identity has a
default key and a default certificate. If the default key of the
identity does not exist, ``createIdentity`` will create one. If the
default certificate of the identity does not exist, ``createIdentity``
will generate a self-signed certificate of the default key as the
default certificate.

System Default Identity
~~~~~~~~~~~~~~~~~~~~~~~

There is a default key for a particular identity, and a default identity
certificate for a particular key. And, there is also a **default
identity** for the system, which is the user of the system. The default
identity can be configured using ndnsec tools only. You cannot configure
through the security library API.

Get and Set Default Keys/Certificates
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you know the exact names of keys and certificates, you can call
``KeyChain::getPublicKey`` and ``KeyChain::getCertificate``.

.. code-block:: cpp

    KeyChain keyChain;
    Name aliceKeyName("/ndn/test/alice/ksk-1394129695025");
    Name aliceCertName("/ndn/test/KEY/alice/ksk-1394129695025/ID-CERT/%FD%01D%98%9A%F2%3F");

    shared_ptr<PublicKey> aliceKey = keyChain.getPublicKey(aliceKeyName);
    shared_ptr<IdentityCertificate> aliceCert = keyChain.getCertificate(aliceCertName);

It might be difficult to remember the exact name of keys and
certificates, but it might be easier to remember identity names. The
security library provides a list of methods to locate the default key
name and certificate name of an identity.

.. code-block:: cpp

    KeyChain keyChain;
    Name alice("/ndn/test/alice");

    Name aliceKeyName = keyChain.getDefaultKeyNameForIdentity(alice);
    Name aliceCertName = keyChain.getDefaultCertificateNameForKey(aliceKeyName);

    /* following code is equivalent to the two lines above */
    Name aliceCertName2 = keyChain.getDefaultCertificateNameForIdentity(alice);

You can also manually set default key for an identity and default
certificate for a key through KeyChain.

.. code-block:: cpp

    KeyChain keyChain;

    Name aliceKeyName("/ndn/test/alice/ksk-1394129695025");
    Name aliceCertName("/ndn/test/KEY/alice/ksk-1394129695025/ID-CERT/%FD%01D%98%9A%F2%3F");

    keyChain.setDefaultKeyNameForIdentity(aliceKeyName);
    keyChain.getDefaultCertificateNameForKey(aliceCertName);

Create Keys Manually
~~~~~~~~~~~~~~~~~~~~

You can call ``KeyChain::generateRSAKeyPair`` to generate an RSA key
pair. Note that generated key pair is not set as the default key of the
identity, so you need to set it manually by calling
``KeyChain::setDefaultKeyNameForIdentity``. There is also a helper
method "KeyChain::generateRSAKeyPairAsDefault", which combines the two
steps into one.

.. code-block:: cpp

    KeyChain keyChain;
    Name alice("/ndn/test/alice");

    Name aliceKeyName = keyChain.generateRSAKeyPair(alice);
    keyChain.setDefaultKeyNameForIdentity(aliceKeyName);

    Name aliceKeyName2 = keyChain.generateRSAKeyPairAsDefault(alice); // Now the key with the name aliceKeyName2 becomes alice's default key

Create Identity Certificate Manually
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you have created a key pair, you can generate a self-signed
certificate for the key by calling ``KeyChain::selfSign``.

.. code-block:: cpp

    KeyChain keyChain;
    Name aliceKeyName("/ndn/test/alice/ksk-1394129695025");

    shared_ptr<IdentityCertificate> aliceCert = keyChain.selfSign(aliceKeyName);

You can sign a public key using a different key, but this signing
process may take several steps. Note that this time, the signing key is
vouching for the signed key, so you need to specify more details, such
as the validity, subject descriptions. The first step is to prepare the
unsigned identity certificate by calling
``KeyChain::prepareUnsignedIdentityCertificate``. And the second step is
to sign the identity certificate. We will talk about the signing methods
in `Packet Signing <#packet_signing>`__.

.. code-block:: cpp

    KeyChain keyChain;

    Name signingIdentity("/ndn/test");
    Name aliceKeyName("/ndn/test/alice/ksk-1394129695025");
    MillisecondsSince1970 notBefore = getNow();
    MillisecondsSince1970 notAfter = notBefore + 630720000;
    vector<CertificateSubjectDescription> subjectDescription;
    subjectDescription.push_back(CertificateSubjectDescription("2.5.4.41", "Alice")); // push any subject description into the list.

    shared_ptr<IdentityCertificate> = aliceCert
      keyChain.prepareUnsignedIdentityCertificate(aliceKeyName, signingIdentity, notBefore, notAfter, subjectDescription);

    keyChain.signByIdentity(*aliceCert, signingIdentity);

Packet Signing
--------------

When keys and certificates are ready for use, you can sign packet using
them. There are two ways to sign a packet:

1. by specifying the name of the identity certificate belonging to the
   signing key.
2. by specifying the identity to which the signing key belongs

Sign With Certificate Name
~~~~~~~~~~~~~~~~~~~~~~~~~~

If we specify the exact certificate name when signing a packet, the
certificate name (without version number) is put into the KeyLocator TLV
in the SignatureInfo. KeyChain will look up the corresponding private
key in the Trusted Platform Module (TPM), and use the private key to
generate the signature.

.. code-block:: cpp

    KeyChain keyChain;

    Name aliceCertName("/ndn/test/KEY/alice/ksk-1394129695025/ID-CERT/%FD%01D%98%9A%F2%3F");
    Data data("/ndn/test/alice/test-data");

    keyChain.sign(data, aliceCertName);

When ``KeyChain::sing`` returns, the SignatureInfo and SignatureValue
TLVs of the supplied data will be set.

Sign With Identity Name
~~~~~~~~~~~~~~~~~~~~~~~

If we only specify the identity name when signing a packet, the name of
the identity's default certificate will be put into the KeyLocator TLV
in the SingatureInfo, and the identity's default key is used to sign the
packet. Please make sure the default key and certificates of the signing
identity is initialized correctly before signing, otherwise, KeyChain
will create key and self-signed certificate for signing (which is not
quite useful).

.. code-block:: cpp

    KeyChain keyChain;

    Name alice("/ndn/test/alice");
    Data data("/ndn/test/alice/test-data");

    keyChain.signByIdentity(data, alice);

Sign Interest Packet
~~~~~~~~~~~~~~~~~~~~

Signing an interest packet is the same as signing a Data packet. The
only difference is that the SignatureInfo And SignatureValue TLV are
encoded as the last two components of the interest name.

Packet Validation
-----------------

Packet validation is done through a **Validator**. Validator is a
virtual class, two pure virtual methods must be implemented in order to
construct a working validator:

.. code-block:: cpp

    class Validator {
      ...
    protected:
      virtual void
      checkPolicy (const Data& data,
                   int stepCount,
                   const OnDataValidated &onValidated,
                   const OnDataValidationFailed &onValidationFailed,
                   std::vector<shared_ptr<ValidationRequest> >& nextSteps) = 0;

      virtual void
      checkPolicy (const Interest& interest,
                   int stepCount,
                   const OnInterestValidated &onValidated,
                   const OnInterestValidationFailed &onValidationFailed,
                   std::vector<shared_ptr<ValidationRequest> >& nextSteps) = 0;
    ...

What you need to do inside these two methods is to check whether the
packet and signer comply with your policies, and whether their signature
can be verified. If the packet can be validated, you should call the
``onValidated`` callback function to trigger packet processing,
otherwise the ``onValidationFailed`` callback should be invoked. If you
need more information (e.g., other certificates), you can construct
several ``ValidationRequest`` and push them into nextSteps.

.. code-block:: cpp

    class ValidationRequest {
    public:
      Interest m_interest;                      // An interest packet to fetch the requested data.
      OnDataValidated m_onValidated;            // A callback function if the requested certificate is validated.
      OnDataValidationFailed m_onDataValidated; // A callback function if the requested certificate validation fails.
      int m_retry;                              // The number of retrials when there is an interest timeout.
      int m_stepCount;                          // The count of validation steps.
    };

Security library also provides an ``Validator``, ``ValidatorRegex``
which has already implemented the two methods (basically for Data policy
checking, the Interest policy checking method always calls
``onValidationFailed``).

.. code-block:: cpp

    class ValidatorRegex : public Validator
    {
    public:
      ...

      void
      addDataVerificationRule(shared_ptr<SecRuleRelative> rule);

      void
      addTrustAnchor(shared_ptr<IdentityCertificate> certificate);

      ...
    };

With ``ValidatorRegex``, you can specify the validation rules in terms
of [[Regex\|NDN Regular Expression]] via
``ValidatorRegex::addDataVerificationRule``, and set trust anchor via
``ValidatorRegex::addTrustAnchor``.

How to specify regex-based validation rule
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To specify a ``SecRuleRelative``, you needs to specify two NDN regular
expressions: one for data name matching, and the other for KeyLocator
matching. For each regex, you also need to specify the back reference
pattern to extract parts of the name. Moreover, you need to specify the
relation between two extracted patterns. For example, a typical
hierarchical rule can be written as

.. code-block:: cpp

    SecRuleRelative rule("^(<>*)$", "^([^<KEY>]*)<KEY>(<>*)<ksk-.*><ID-CERT>$",
                         ">", "\\1", "\\1\\2", true);

This rule indicates that the data name must be under the signer's
namespace.

How to use validator
~~~~~~~~~~~~~~~~~~~~

Here is an example of how to use the validator

.. code-block:: cpp

    class Example {
    public:
      Example(ndn::shared_ptr<ndn::Face> face>)
        : m_face(face)
      {
        ndn::shared_ptr<ndn::ValidatorRegex> validator(new ndn::ValidatorRegex(m_face));
        validator->addDataVerificationRule(ndn::make_shared<ndn::SecRuleRelative>("^(<>*)$",
                                                                                  "^([^<KEY>]*)<KEY>(<>*)<ksk-.*><ID-CERT>$",
                                                                                  ">", "\\1", "\\1\\2", true));
        ndn::shared_ptr<ndn::IdentityCertificate> anchor = ndn::io::load<IdentityCertificate>("ndn-root.cert");
        validator->addTrustAnchor(anchor);
        m_validator = validator;
      }

      virtual
      ~Example() {}

      void
      sendInterest()
      {
        Interest interest ("/ndn/test/data");
        m_face->expressInterest(interest,
                                bind(&Example::onData, this, _1, _2),
                                bind(&Example::onTimeout, this, _1));
      }

      void
      onData(const ndn::Interest& interest, const ndn::Data& data)
      {
        m_validator->validate(data,
                              ndn::bind(&Example::onValidated, this, _1),
                              ndn::bind(&Example::onValidationFailed, this, _1, _2));
      }

      void onTimeout(const ndn::Interest& interest) {}

      void onValidated(const ndn::shared_ptr<const ndn::Data>& data) {}

      void onValidationFailed(const ndn::shared_ptr<const ndn::Data>& data, const std::string& failInfo) {}

    private:
      ndn::shared_ptr<ndn::Face> m_face;
      ndn::shared_ptr<ndn::Validator> m_validator;
    };
