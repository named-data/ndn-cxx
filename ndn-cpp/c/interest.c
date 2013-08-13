/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include "util/ndn_memory.h"
#include "interest.h"

int ndn_Exclude_compareComponents(struct ndn_NameComponent *component1, struct ndn_NameComponent *component2)
{
  if (component1->valueLength < component2->valueLength)
    return -1;
  if (component1->valueLength > component2->valueLength)
    return 1;

  // The components are equal length.  Just do a byte compare.  
  return ndn_memcmp(component1->value, component2->value, component1->valueLength);
}

int ndn_Exclude_matches(struct ndn_Exclude *self, struct ndn_NameComponent *component)
{
  unsigned int i;
  for (i = 0; i < self->nEntries; ++i) {
    if (self->entries[i].type == ndn_Exclude_ANY) {
      struct ndn_ExcludeEntry *lowerBound = 0;
      if (i > 0)
        lowerBound = self->entries + (i - 1);
      
      // Find the upper bound, possibly skipping over multiple ANY in a row.
      unsigned int iUpperBound;
      struct ndn_ExcludeEntry *upperBound = 0;
      for (iUpperBound = i + 1; iUpperBound < self->nEntries; ++iUpperBound) {
        if (self->entries[iUpperBound].type == ndn_Exclude_COMPONENT) {
          upperBound = self->entries + iUpperBound;
          break;
        }
      }
      
      // If lowerBound != 0, we already checked component equals lowerBound on the last pass.
      // If upperBound != 0, we will check component equals upperBound on the next pass.
      if (upperBound != 0) {
        if (lowerBound != 0) {
          if (ndn_Exclude_compareComponents(component, &lowerBound->component) > 0 &&
              ndn_Exclude_compareComponents(component, &upperBound->component) < 0)
            return 1;
        }
        else {
          if (ndn_Exclude_compareComponents(component, &upperBound->component) < 0)
            return 1;
        }
        
        // Make i equal iUpperBound on the next pass.
        i = iUpperBound - 1;
      }
      else {
        if (lowerBound != 0) {
          if (ndn_Exclude_compareComponents(component, &lowerBound->component) > 0)
            return 1;
        }
        else
          // this.values has only ANY.
          return 1;
      }
    }
    else {
      if (ndn_Exclude_compareComponents(component, &self->entries[i].component) == 0)
        return 1;
    }
  }
  
  return 0;  
}

int ndn_Interest_matchesName(struct ndn_Interest *self, struct ndn_Name *name)
{
  if (!ndn_Name_match(&self->name, name))
    return 0;
  
  if (self->minSuffixComponents >= 0 &&
    // Add 1 for the implicit digest.
    !(name->nComponents + 1 - self->name.nComponents >= self->minSuffixComponents))
    return 0;
  if (self->maxSuffixComponents >= 0 &&
    // Add 1 for the implicit digest.
    !(name->nComponents + 1 - self->name.nComponents <= self->maxSuffixComponents))
    return 0;
  if (self->exclude.nEntries > 0 && name->nComponents > self->name.nComponents &&
      ndn_Exclude_matches(&self->exclude, name->components + self->name.nComponents))
    return 0;
  
  return 1; 
}
