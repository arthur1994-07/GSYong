#ifndef WorldStateMachineType_H
#define WorldStateMachineType_H

#include <map>
#include "NotifierGroup.h"

typedef std::map<int, CNotifierGroup>		NotifierGroupContainer;
typedef NotifierGroupContainer::iterator	NotifierGroupContainerIterator;

#endif