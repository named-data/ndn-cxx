/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_WIREFORMAT_HPP
#define NDN_WIREFORMAT_HPP

#include "../common.hpp"
#include <vector>

namespace ndn {
  
class Interest;
class Data;
  
class WireFormat {
public:
  virtual ptr_lib::shared_ptr<std::vector<unsigned char> > encodeInterest(const Interest &interest);
  virtual void decodeInterest(Interest &interest, const unsigned char *input, unsigned int inputLength);

  virtual ptr_lib::shared_ptr<std::vector<unsigned char> > encodeData(const Data &data);
  virtual void decodeData(Data &data, const unsigned char *input, unsigned int inputLength);
  
  /**
   * Set the static default WireFormat used by default encoding and decoding methods.
   * @param wireFormat A Pointer to a object of a subclass of WireFormat.  This does not make a copy and
   * the caller must ensure that the object remains allocated.
   */
  static void setDefaultWireFormat(WireFormat *wireFormat) 
  {
    defaultWireFormat_ = wireFormat;
  }
  
  /**
   * Return the default WireFormat used by default encoding and decoding methods which was set with
   * setDefaultWireFormat.
   * @return A pointer to the WireFormat object.
   */
  static WireFormat *getDefaultWireFormat();
  
private:
  /**
   * This is implemented by only one of the subclasses of WireFormat to return a new object used
   * as the initial value for the default WireFormat.  If the application doesn't include that class, then the application
   * needs to include another subclass which defines WireFormat::newInitialDefaultWireFormat.
   * @return a new object, which is held by a shared_ptr and freed when the application exits.
   */
  static WireFormat *newInitialDefaultWireFormat();
  
  static WireFormat *defaultWireFormat_;
};

}

#endif

