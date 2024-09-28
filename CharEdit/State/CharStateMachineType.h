#ifndef SynchronizerType_H
#define SynchronizerType_H

#include <map>
#include "NotifierGroup.h"

typedef std::map<int, CNotifierGroup>		NotifierGroupContainer;
typedef NotifierGroupContainer::iterator	NotifierGroupContainerIterator;

#endif