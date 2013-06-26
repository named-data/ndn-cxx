/* 
 * Author: Jeff Thompson
 *
 * BSD license, See the LICENSE file for more information.
 */

#ifndef NDN_BINARYXML_H
#define	NDN_BINARYXML_H

#ifdef	__cplusplus
extern "C" {
#endif

const int ndn_BinaryXML_EXT = 0x00; 	
const int ndn_BinaryXML_TAG = 0x01; 
const int ndn_BinaryXML_DTAG = 0x02; 
const int ndn_BinaryXML_ATTR = 0x03; 
const int ndn_BinaryXML_DATTR = 0x04; 
const int ndn_BinaryXML_BLOB = 0x05; 
const int ndn_BinaryXML_UDATA = 0x06; 
const int ndn_BinaryXML_CLOSE = 0x0;

const int ndn_BinaryXML_TT_BITS = 3;
const int ndn_BinaryXML_TT_MASK = 0x07;
const int ndn_BinaryXML_TT_VALUE_BITS = 4;
const int ndn_BinaryXML_TT_VALUE_MASK = 0x0f;
const int ndn_BinaryXML_REGULAR_VALUE_BITS = 7;
const int ndn_BinaryXML_REGULAR_VALUE_MASK = 0x7f;
const int ndn_BinaryXML_TT_FINAL = 0x80;

const int ndn_BinaryXML_DTag_Any = 13;
const int ndn_BinaryXML_DTag_Name = 14;
const int ndn_BinaryXML_DTag_Component = 15;
const int ndn_BinaryXML_DTag_Certificate = 16;
const int ndn_BinaryXML_DTag_Collection = 17;
const int ndn_BinaryXML_DTag_CompleteName = 18;
const int ndn_BinaryXML_DTag_Content = 19;
const int ndn_BinaryXML_DTag_SignedInfo = 20;
const int ndn_BinaryXML_DTag_ContentDigest = 21;
const int ndn_BinaryXML_DTag_ContentHash = 22;
const int ndn_BinaryXML_DTag_Count = 24;
const int ndn_BinaryXML_DTag_Header = 25;
const int ndn_BinaryXML_DTag_Interest = 26;	/* 20090915 */
const int ndn_BinaryXML_DTag_Key = 27;
const int ndn_BinaryXML_DTag_KeyLocator = 28;
const int ndn_BinaryXML_DTag_KeyName = 29;
const int ndn_BinaryXML_DTag_Length = 30;
const int ndn_BinaryXML_DTag_Link = 31;
const int ndn_BinaryXML_DTag_LinkAuthenticator = 32;
const int ndn_BinaryXML_DTag_NameComponentCount = 33;	/* DeprecatedInInterest */
const int ndn_BinaryXML_DTag_RootDigest = 36;
const int ndn_BinaryXML_DTag_Signature = 37;
const int ndn_BinaryXML_DTag_Start = 38;
const int ndn_BinaryXML_DTag_Timestamp = 39;
const int ndn_BinaryXML_DTag_Type = 40;
const int ndn_BinaryXML_DTag_Nonce = 41;
const int ndn_BinaryXML_DTag_Scope = 42;
const int ndn_BinaryXML_DTag_Exclude = 43;
const int ndn_BinaryXML_DTag_Bloom = 44;
const int ndn_BinaryXML_DTag_BloomSeed = 45;
const int ndn_BinaryXML_DTag_AnswerOriginKind = 47;
const int ndn_BinaryXML_DTag_InterestLifetime = 48;
const int ndn_BinaryXML_DTag_Witness = 53;
const int ndn_BinaryXML_DTag_SignatureBits = 54;
const int ndn_BinaryXML_DTag_DigestAlgorithm = 55;
const int ndn_BinaryXML_DTag_BlockSize = 56;
const int ndn_BinaryXML_DTag_FreshnessSeconds = 58;
const int ndn_BinaryXML_DTag_FinalBlockID = 59;
const int ndn_BinaryXML_DTag_PublisherPublicKeyDigest = 60;
const int ndn_BinaryXML_DTag_PublisherCertificateDigest = 61;
const int ndn_BinaryXML_DTag_PublisherIssuerKeyDigest = 62;
const int ndn_BinaryXML_DTag_PublisherIssuerCertificateDigest = 63;
const int ndn_BinaryXML_DTag_ContentObject = 64;	/* 20090915 */
const int ndn_BinaryXML_DTag_WrappedKey = 65;
const int ndn_BinaryXML_DTag_WrappingKeyIdentifier = 66;
const int ndn_BinaryXML_DTag_WrapAlgorithm = 67;
const int ndn_BinaryXML_DTag_KeyAlgorithm = 68;
const int ndn_BinaryXML_DTag_Label = 69;
const int ndn_BinaryXML_DTag_EncryptedKey = 70;
const int ndn_BinaryXML_DTag_EncryptedNonceKey = 71;
const int ndn_BinaryXML_DTag_WrappingKeyName = 72;
const int ndn_BinaryXML_DTag_Action = 73;
const int ndn_BinaryXML_DTag_FaceID = 74;
const int ndn_BinaryXML_DTag_IPProto = 75;
const int ndn_BinaryXML_DTag_Host = 76;
const int ndn_BinaryXML_DTag_Port = 77;
const int ndn_BinaryXML_DTag_MulticastInterface = 78;
const int ndn_BinaryXML_DTag_ForwardingFlags = 79;
const int ndn_BinaryXML_DTag_FaceInstance = 80;
const int ndn_BinaryXML_DTag_ForwardingEntry = 81;
const int ndn_BinaryXML_DTag_MulticastTTL = 82;
const int ndn_BinaryXML_DTag_MinSuffixComponents = 83;
const int ndn_BinaryXML_DTag_MaxSuffixComponents = 84;
const int ndn_BinaryXML_DTag_ChildSelector = 85;
const int ndn_BinaryXML_DTag_RepositoryInfo = 86;
const int ndn_BinaryXML_DTag_Version = 87;
const int ndn_BinaryXML_DTag_RepositoryVersion = 88;
const int ndn_BinaryXML_DTag_GlobalPrefix = 89;
const int ndn_BinaryXML_DTag_LocalName = 90;
const int ndn_BinaryXML_DTag_Policy = 91;
const int ndn_BinaryXML_DTag_Namespace = 92;
const int ndn_BinaryXML_DTag_GlobalPrefixName = 93;
const int ndn_BinaryXML_DTag_PolicyVersion = 94;
const int ndn_BinaryXML_DTag_KeyValueSet = 95;
const int ndn_BinaryXML_DTag_KeyValuePair = 96;
const int ndn_BinaryXML_DTag_IntegerValue = 97;
const int ndn_BinaryXML_DTag_DecimalValue = 98;
const int ndn_BinaryXML_DTag_StringValue = 99;
const int ndn_BinaryXML_DTag_BinaryValue = 100;
const int ndn_BinaryXML_DTag_NameValue = 101;
const int ndn_BinaryXML_DTag_Entry = 102;
const int ndn_BinaryXML_DTag_ACL = 103;
const int ndn_BinaryXML_DTag_ParameterizedName = 104;
const int ndn_BinaryXML_DTag_Prefix = 105;
const int ndn_BinaryXML_DTag_Suffix = 106;
const int ndn_BinaryXML_DTag_Root = 107;
const int ndn_BinaryXML_DTag_ProfileName = 108;
const int ndn_BinaryXML_DTag_Parameters = 109;
const int ndn_BinaryXML_DTag_InfoString = 110;
const int ndn_BinaryXML_DTag_StatusResponse = 112;
const int ndn_BinaryXML_DTag_StatusCode = 113;
const int ndn_BinaryXML_DTag_StatusText = 114;
const int ndn_BinaryXML_DTag_SyncNode = 115;
const int ndn_BinaryXML_DTag_SyncNodeKind = 116;
const int ndn_BinaryXML_DTag_SyncNodeElement = 117;
const int ndn_BinaryXML_DTag_SyncVersion = 118;
const int ndn_BinaryXML_DTag_SyncNodeElements = 119;
const int ndn_BinaryXML_DTag_SyncContentHash = 120;
const int ndn_BinaryXML_DTag_SyncLeafCount = 121;
const int ndn_BinaryXML_DTag_SyncTreeDepth = 122;
const int ndn_BinaryXML_DTag_SyncByteCount = 123;
const int ndn_BinaryXML_DTag_SyncConfigSlice = 124;
const int ndn_BinaryXML_DTag_SyncConfigSliceList = 125;
const int ndn_BinaryXML_DTag_SyncConfigSliceOp = 126;
const int ndn_BinaryXML_DTag_SyncNodeDeltas = 127;
const int ndn_BinaryXML_DTag_SequenceNumber = 256;
const int ndn_BinaryXML_DTag_CCNProtocolDataUnit = 17702112;

#ifdef	__cplusplus
}
#endif

#endif
