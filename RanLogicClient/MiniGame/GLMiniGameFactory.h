#pragma once

class GLGaeaClient;

namespace MiniGame
{
	class IDice;
	IDice* createDice(const unsigned int nDice, const unsigned int nStageWidth, const unsigned int nStageHeight);
	void destroyDice(IDice*& pInstance);

	class IMiniGame;
	IMiniGame* createMiniGameClient(GLGaeaClient* const pGaeaClient, const DWORD dwGaeaID, const DWORD dwMiniGameID, const float fBeginTime, const SNATIVEID& NPCID, const DWORD dwNPCGaeaID);
	const bool destroyMiniGameClient(IMiniGame*& pMiniGame);
}