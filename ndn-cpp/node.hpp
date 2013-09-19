/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_NODE_HPP
#define NDN_NODE_HPP

#include "common.hpp"
#include "interest.hpp"
#include "data.hpp"
#include "transport/tcp-transport.hpp"
#include "encoding/binary-xml-element-reader.hpp"

namespace ndn {

/**
 * An OnData function object is used to pass a callback to expressInterest.
 */
typedef func_lib::function<void(const ptr_lib::shared_ptr<const Interest>&, const ptr_lib::shared_ptr<Data>&)> OnData;

/**
 * An OnTimeout function object is used to pass a callback to expressInterest.
 */
typedef func_lib::function<void(const ptr_lib::shared_ptr<const Interest>&)> OnTimeout;

/**
 * An OnInterest function object is used to pass a callback to registerPrefix.
 */
typedef func_lib::function<void
  (const ptr_lib::shared_ptr<const Name>&, const ptr_lib::shared_ptr<const Interest>&, Transport&)> OnInterest;

/**
 * An OnRegisterFailed function object is used to report when registerPrefix fails.
 */
typedef func_lib::function<void(const ptr_lib::shared_ptr<const Name>&)> OnRegisterFailed;

class Face;
class KeyChain;
    
class Node : public ElementListener {
public:
  /**
   * Create a new Node for communication with an NDN hub with the given Transport object and connectionInfo.
   * @param transport A shared_ptr to a Transport object used for communication.
   * @param transport A shared_ptr to a Transport::ConnectionInfo to be used to connect to the transport.
   */
  Node(const ptr_lib::shared_ptr<Transport>& transport, const ptr_lib::shared_ptr<const Transport::ConnectionInfo>& connectionInfo);
  
  /**
   * Send the Interest through the transport, read the entire response and call onData(interest, data).
   * @param interest A reference to the Interest.  This copies the Interest.
   * @param onData A function object to call when a matching data packet is received.  This copies the function object, so you may need to
   * use func_lib::ref() as appropriate.
   * @param onTimeout A function object to call if the interest times out.  If onTimeout is an empty OnTimeout(), this does not use it.
   * This copies the function object, so you may need to use func_lib::ref() as appropriate.
   */
  void 
  expressInterest(const Interest& interest, const OnData& onData, const OnTimeout& onTimeout);
  
  /**
   * Register prefix with the connected NDN hub and call onInterest when a matching interest is received.
   * @param prefix A reference to a Name for the prefix to register.  This copies the Name.
   * @param onInterest A function object to call when a matching interest is received.  This copies the function object, so you may need to
   * use func_lib::ref() as appropriate.
   * @param onRegisterFailed A function object to call if failed to retrieve the connected hub’s ID or failed to register the prefix.
   * This calls onRegisterFailed(prefix) where prefix is the prefix given to registerPrefix.
   * @param flags The flags for finer control of which interests are forward to the application.
   * @param wireFormat A WireFormat object used to encode the input. If omitted, use WireFormat getDefaultWireFormat().
   */
  void 
  registerPrefix
    (const Name& prefix, const OnInterest& onInterest, const OnRegisterFailed& onRegisterFailed, int flags, WireFormat& wireFormat);

  /**
   * Process any data to receive.  For each element received, call onReceivedElement.
   * This is non-blocking and will return immediately if there is no data to receive.
   * You should repeatedly call this from an event loop, with calls to sleep as needed so that the loop doesn't use 100% of the CPU.
   * @throw This may throw an exception for reading data or in the callback for processing the data.  If you
   * call this from an main event loop, you may want to catch and log/disregard all exceptions.
   */
  void 
  processEvents();
  
  const ptr_lib::shared_ptr<Transport>& 
  getTransport() { return transport_; }
  
  const ptr_lib::shared_ptr<const Transport::ConnectionInfo>& 
  getConnectionInfo() { return connectionInfo_; }

  void 
  onReceivedElement(const unsigned char *element, unsigned int elementLength);
  
  void 
  shutdown();

private:
  class PitEntry {
  public:
    /**
     * Create a new PitEntry and set the timeoutTime_ based on the current time and the interest lifetime.
     * @param interest A shared_ptr for the interest.
     * @param onData A function object to call when a matching data packet is received.
     * @param onTimeout A function object to call if the interest times out.  If onTimeout is an empty OnTimeout(), this does not use it.
     */
    PitEntry(const ptr_lib::shared_ptr<const Interest>& interest, const OnData& onData, const OnTimeout& onTimeout);
    
    const ptr_lib::shared_ptr<const Interest>& 
    getInterest() { return interest_; }
    
    const OnData& 
    getOnData() { return onData_; }
    
    /**
     * Get the struct ndn_Interest for the interest_.
     * WARNING: Assume that this PitEntry was created with new, so that no copy constructor is invoked between calls.
     * This class is private to Node and only used by its methods, so this should be OK.
     * TODO: Doesn't this functionality belong in the Interest class?
     * @return A reference to the ndn_Interest struct.
     * WARNING: The resulting pointers in are invalid uses getInterest() to manipulate the object which could reallocate memory.
     */
    const struct ndn_Interest& 
    getInterestStruct()
    {
      return interestStruct_;
    }
    
    /**
     * If this interest is timed out, call onTimeout_ (if defined) and return true.
     * @param parent The parent Node for the UpcallInfo.
     * @param nowMilliseconds The current time in milliseconds from gettimeofday.
     * @return true if this interest timed out and the timeout callback was called, otherwise false.
     */
    bool 
    checkTimeout(Node *parent, double nowMilliseconds);
    
  private:
    ptr_lib::shared_ptr<const Interest> interest_;
    std::vector<struct ndn_NameComponent> nameComponents_;
    std::vector<struct ndn_ExcludeEntry> excludeEntries_;
    struct ndn_Interest interestStruct_;
  
    const OnData onData_;
    const OnTimeout onTimeout_;
    double timeoutTimeMilliseconds_; /**< The time when the interest times out in milliseconds according to gettimeofday, or -1 for no timeout. */
  };

  class PrefixEntry {
  public:
    /**
     * Create a new PrefixEntry.
     * @param prefix A shared_ptr for the prefix.
     * @param onInterest A function object to call when a matching data packet is received.
     */
    PrefixEntry(const ptr_lib::shared_ptr<const Name>& prefix, const OnInterest& onInterest)
    : prefix_(prefix), onInterest_(onInterest)
    {
    }
    
    const ptr_lib::shared_ptr<const Name>& 
    getPrefix() { return prefix_; }
    
    const OnInterest& 
    getOnInterest() { return onInterest_; }
    
  private:
    ptr_lib::shared_ptr<const Name> prefix_;
    const OnInterest onInterest_;
  };
  
  /**
   * An NdndIdFetcher receives the Data packet with the publisher public key digest for the connected NDN hub.
   * This class is a function object for the callbacks. It only holds a pointer to an Info object, so it is OK to copy the pointer.
   */
  class NdndIdFetcher {
  public:
    class Info;
    NdndIdFetcher(ptr_lib::shared_ptr<NdndIdFetcher::Info> info)
    : info_(info)
    {      
    }
    
    /**
     * We received the ndnd ID.
     * @param interest
     * @param data
     */
    void 
    operator()(const ptr_lib::shared_ptr<const Interest>& interest, const ptr_lib::shared_ptr<Data>& ndndIdData);

    /**
     * We timed out fetching the ndnd ID.
     * @param interest
     */
    void 
    operator()(const ptr_lib::shared_ptr<const Interest>& timedOutInterest);
    
    class Info {
    public:
      Info(Node *node, const Name& prefix, const OnInterest& onInterest, const OnRegisterFailed& onRegisterFailed, 
           int flags, WireFormat& wireFormat)
      : node_(*node), prefix_(new Name(prefix)), onInterest_(onInterest), onRegisterFailed_(onRegisterFailed), 
        flags_(flags), wireFormat_(wireFormat)
      {      
      }
      
      Node& node_;
      ptr_lib::shared_ptr<const Name> prefix_;
      const OnInterest onInterest_;
      const OnRegisterFailed onRegisterFailed_;
      int flags_;
      WireFormat& wireFormat_;
    };
    
  private:
    ptr_lib::shared_ptr<Info> info_;
  };
  
  /**
   * Find the entry from the pit_ where the name conforms to the entry's interest selectors, and
   * the entry interest name is the longest that matches name.
   * @param name The name to find the interest for (from the incoming data packet).
   * @return The index in pit_ of the pit entry, or -1 if not found.
   */
  int 
  getEntryIndexForExpressedInterest(const Name& name);
  
  /**
   * Find the first entry from the registeredPrefixTable_ where the entry prefix is the longest that matches name.
   * @param name The name to find the PrefixEntry for (from the incoming interest packet).
   * @return A pointer to the entry, or 0 if not found.
   */
  PrefixEntry*
  getEntryForRegisteredPrefix(const Name& name);

  /**
   * Do the work of registerPrefix once we know we are connected with an ndndId_.
   * @param prefix
   * @param onInterest
   * @param onRegisterFailed
   * @param flags
   * @param wireFormat
   */  
  void 
  registerPrefixHelper
    (const ptr_lib::shared_ptr<const Name>& prefix, const OnInterest& onInterest, const OnRegisterFailed& onRegisterFailed, 
     int flags, WireFormat& wireFormat);
  
  ptr_lib::shared_ptr<Transport> transport_;
  ptr_lib::shared_ptr<const Transport::ConnectionInfo> connectionInfo_;
  std::vector<ptr_lib::shared_ptr<PitEntry> > pit_;
  std::vector<ptr_lib::shared_ptr<PrefixEntry> > registeredPrefixTable_;
  Interest ndndIdFetcherInterest_;
  Blob ndndId_;
};

}

#endif
