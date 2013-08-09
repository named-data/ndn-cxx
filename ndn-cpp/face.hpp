/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_FACE_HPP
#define NDN_FACE_HPP

#include "closure.hpp"
#include "interest.hpp"
#include "transport/udp-transport.hpp"
#include "encoding/binary-xml-element-reader.hpp"

using namespace std;

namespace ndn {

class Face : public ElementListener {
public:
  Face(const char *host, unsigned short port, const ptr_lib::shared_ptr<Transport> &transport)
  : host_(host), port_(port), transport_(transport), tempClosure_(0)
  {
  }
  
  Face(const char *host, unsigned short port)
  : host_(host), port_(port), transport_(new UdpTransport()), tempClosure_(0)
  {
  }
  
  Face(const char *host)
  : host_(host), port_(9695), transport_(new UdpTransport()), tempClosure_(0)
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
  
  void shutdown();
  
  const char *getHost() const { return host_.c_str(); }
  
  unsigned short getPort() const { return port_; }
  
  const ptr_lib::shared_ptr<Transport> &getTransport() { return transport_; }
  
  virtual void onReceivedElement(unsigned char *element, unsigned int elementLength);
  
private:
  ptr_lib::shared_ptr<Transport> transport_;
  string host_;
  unsigned short port_;
  Closure *tempClosure_;
};

}

#endif
