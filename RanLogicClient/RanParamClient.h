#pragma once

#include "../RanLogic/RANPARAM.h"

class GLGaeaClient;

namespace RANPARAM
{
	void INIT_GAMEOPT();
	BOOL LOAD_GAMEOPT(const SUBPATH* pPath,const std::string& charName);
	BOOL SAVE_GAMEOPT(const SUBPATH* pPath);
}
