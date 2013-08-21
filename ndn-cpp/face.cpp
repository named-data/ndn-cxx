/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include "face.hpp"

using namespace std;

namespace ndn {
  
void Face::shutdown()
{
  node_.shutdown();
}

}
