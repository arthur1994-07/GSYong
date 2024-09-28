#pragma once

struct NET_MSG_GENERIC;
class GLAgentServer;
#include <map>
#include "../../../RanLogic/Network/NetLogicDefine.h"

namespace StateFieldServer
{
	// key : fieldServerID, usedMemory;
	typedef std::map<unsigned int, int> StateFieldMap;
	typedef StateFieldMap::const_iterator StateFieldMapConstIterator;
	typedef StateFieldMap::iterator StateFieldMapIterator;
	typedef StateFieldMap::value_type StateFieldValue;

	// key : mapID, value : usedMemory;
	typedef std::map<unsigned int, int> UsedMemoryPerLandMap;
	typedef UsedMemoryPerLandMap::const_iterator UsedMemoryPerLandMapConstIterator;
	typedef UsedMemoryPerLandMap::iterator UsedMemoryPerLandMapIterator;
	typedef UsedMemoryPerLandMap::value_type UsedMemoryPerLandValue;
}