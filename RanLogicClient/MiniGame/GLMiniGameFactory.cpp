#include "pch.h"

#include "../../SigmaCore/SSE2/SSE2.h"

#include "../GLGaeaClient.h"
#include "./Dice/DiceLogic/DiceLogic.h"
#include "./Dice/DiceLogicSSE2/DiceLogicSSE2.h"
#include "./OddEven/GLOddEven.h"
#include "GLMiniGameFactory.h"

namespace MiniGame
{
	IDice* createDice(const unsigned int nDice, const unsigned int nStageWidth, const unsigned int nStageHeight)
	{
		SSE2::CPUInformation _CPUInfo;
		getCPUInformation(_CPUInfo);
		if ( _CPUInfo.bSSE2 == true )
			return (IDice*)new DiceLogicSSE2(nDice, nStageWidth, nStageHeight);

		return (IDice*)new DiceLogic(nDice, nStageWidth, nStageHeight);
	}
	void destroyDice(IDice*& pInstance)
	{
		if ( pInstance == NULL )
			return;

		delete pInstance;
		pInstance = 0;
	}


	IMiniGame* createMiniGameClient(GLGaeaClient* const pGaeaClient, const DWORD dwGaeaID, const DWORD dwMiniGameID, const float fBeginTime, const SNATIVEID& NPCID, const DWORD dwNPCGaeaID)
	{
		IMiniGame* pMiniGame = NULL;
		switch ( dwMiniGameID )
		{
		case MiniGame::EMMINIGAME_ODDEVEN:
			pMiniGame = (IMiniGame*)new MiniGame::GLOddEvenClient(pGaeaClient, dwGaeaID, dwMiniGameID, fBeginTime, NPCID, dwNPCGaeaID);
			break;
		}

		return pMiniGame;
	}

	const bool destroyMiniGameClient(IMiniGame*& pMiniGame)
	{
		if ( pMiniGame == NULL )
			return true;

		delete pMiniGame;
		pMiniGame = NULL;
		return true;
	}
}

