/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_NODE_HPP
#define NDN_NODE_HPP

#include "interest.hpp"
#include "closure.hpp"
#include "transport/udp-transport.hpp"
#include "encoding/binary-xml-element-reader.hpp"

namespace ndn {

class Face;
  
class Node : public ElementListener {
public:
  /**
   * Create a new Node for communication with an NDN hub at host:port with the given Transport object.
   * @param host The host of the NDN hub.
   * @param port The port of the NDN hub.
   * @param transport A pointer to a Transport object used for communication.
   */
  Node(const char *host, unsigned short port, const ptr_lib::shared_ptr<Transport> &transport)
  : host_(host), port_(port), transport_(transport)
  {
  }
  
  /**
   * Create a new Node for communication with an NDN hub at host:port using the default UdpTransport.
   * @param host The host of the NDN hub.
   * @param port The port of the NDN hub.
   */
  Node(const char *host, unsigned short port)
  : host_(host), port_(port), transport_(new UdpTransport())
  {
  }
  
  /**
   * Create a new Node for communication with an NDN hub at host with the default port 9695 and using the default UdpTransport.
   * @param host The host of the NDN hub.
   */
  Node(const char *host)
  : host_(host), port_(9695), transport_(new UdpTransport())
  {
  }

  /**
   * Encode name as an Interest. If interestTemplate is not 0, use its interest selectors.
   * Send the interest through the transport, read the entire response and call
   * closure->upcall(UPCALL_DATA (or UPCALL_DATA_UNVERIFIED),
   *                 UpcallInfo(this, interest, 0, data)).
   * @param name reference to a Name for the interest.  This does not keep a pointer to the Name object.
   * @param closure a pointer for the Closure.  The caller must manage the memory for the Closure.  This will not try to delete it.
   * @param interestTemplate if not 0, copy interest selectors from the template.   This does not keep a pointer to the Interest object.
   */
  void expressInterest(const Name &name, Closure *closure, const Interest *interestTemplate);
  
  void expressInterest(const Name &name, Closure *closure)
  {
    expressInterest(name, closure, 0);
  }

  /**
   * Process any data to receive.  For each element received, call onReceivedElement.
   * This is non-blocking and will return immediately if there is no data to receive.
   * You should repeatedly call this from an event loop, with calls to sleep as needed so that the loop doesn't use 100% of the CPU.
   * @throw This may throw an exception for reading data or in the callback for processing the data.  If you
   * call this from an main event loop, you may want to catch and log/disregard all exceptions.
   */
  void processEvents();
  
  const char *getHost() const { return host_.c_str(); }
  
  unsigned short getPort() const { return port_; }
  
  const ptr_lib::shared_ptr<Transport> &getTransport() { return transport_; }
  
  void onReceivedElement(const unsigned char *element, unsigned int elementLength);
  
  void shutdown();

private:
  class PitEntry {
  public:
    /**
     * Create a new PitEntry and set the timeoutTime_ based on the current time and the interest lifetime.
     * @param interest A shared_ptr for the interest.
     * @param closure A pointer to the closure with the callbacks to call on match. 
     * The caller must manage the memory for the Closure.  This will not try to delete it.
     */
    PitEntry(const ptr_lib::shared_ptr<const Interest> &interest, Closure *closure);
    
    const ptr_lib::shared_ptr<const Interest> &getInterest() { return interest_; }
    
    Closure *getClosure() { return closure_; }
    
    /**
     * Get the struct ndn_Interest for the interest_.
     * WARNING: Assume that this PitEntry was created with new, so that no copy constructor is invoked between calls.
     * This class is private to Node and only used by its methods, so this should be OK.
     * TODO: Doesn't this functionality belong in the Interest class?
     * @return A reference to the ndn_Interest struct.
     * WARNING: The resulting pointers in are invalid uses getInterest() to manipulate the object which could reallocate memory.
     */
    const struct ndn_Interest &getInterestStruct()
    {
      return interestStruct_;
    }
    
    /**
     * If this interest is timed out, call the timeout callback and return true.
     * @param parent The parent Node for the UpcallInfo.
     * @param nowMilliseconds The current time in milliseconds from gettimeofday.
     * @return true if this interest timed out and the timeout callback was called, otherwise false.
     */
    bool checkTimeout(Node *parent, double nowMilliseconds);
    
  private:
    ptr_lib::shared_ptr<const Interest> interest_;
    std::vector<struct ndn_NameComponent> nameComponents_;
    std::vector<struct ndn_ExcludeEntry> excludeEntries_;
    struct ndn_Interest interestStruct_;
  
    Closure *closure_;
    double timeoutTimeMilliseconds_; /**< The time when the interest times out in milliseconds according to gettimeofday, or -1 for no timeout. */
  };
  
  /**
   * Find the entry from the pit_ where the name conforms to the entry's interest selectors, and
   * the entry interest name is the longest that matches name.
   * @param name The name to find the interest for (from the incoming data packet).
   * @return The index in pit_ of the pit entry, or -1 if not found.
   */
  int getEntryIndexForExpressedInterest(const Name &name);
  
  ptr_lib::shared_ptr<Transport> transport_;
  std::vector<ptr_lib::shared_ptr<PitEntry> > pit_;
  std::string host_;
  unsigned short port_;
};

}

#endif
