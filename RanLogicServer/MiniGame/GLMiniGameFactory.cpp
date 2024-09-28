#include "pch.h"

#include "../FieldServer/GLGaeaServer.h"
#include "./OddEven/GLOddEven.h"
#include "GLMiniGameFactory.h"

namespace MiniGame
{
	IMiniGame* createMiniGameField(GLGaeaServer* const pGaeaServer, const DWORD dwGaeaID, const DWORD dwMiniGameID, const float fBeginTime, const SNATIVEID& NPCID, const DWORD dwNPCGaeaID)
	{
		IMiniGame* pMiniGame = NULL;
		switch ( dwMiniGameID )
		{
		case MiniGame::EMMINIGAME_ODDEVEN:
			pMiniGame = (IMiniGame*)new MiniGame::GLOddEvenField(pGaeaServer, dwGaeaID, dwMiniGameID, fBeginTime, NPCID, dwNPCGaeaID);
			break;
		}

		return pMiniGame;
	}

	const bool destroyMiniGameField(IMiniGame*& pMiniGame)
	{
		if ( pMiniGame == NULL )
			return true;

		delete pMiniGame;
		pMiniGame = NULL;
		return true;
	}
}
