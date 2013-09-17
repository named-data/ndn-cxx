/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * See COPYING for copyright and distribution information.
 */

#ifndef NDN_CHANGED_EVENT_HPP
#define	NDN_CHANGED_EVENT_HPP

#include <vector>
#include "../common.hpp"

namespace ndn {

/**
 * An OnChanged function object is called to notify a listener of a changed event.
 */
typedef func_lib::function<void()> OnChanged;

class ChangedEvent {
public:
  /**
   * Add onChanged to the listener list. This does not check if a duplicate is already in the list.
   * @param onChanged The OnChanged function object.
   */
  void 
  add(OnChanged onChanged)
  {
    listeners_.push_back(onChanged);
  }
  
  /**
   * Call all the listeners.
   */
  void 
  fire();

#if 0  
private:
#endif
  std::vector<OnChanged> listeners_;
};

}

#endif
