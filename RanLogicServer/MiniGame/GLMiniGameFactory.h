#pragma once

class GLGaeaServer;
namespace MiniGame
{
	class IMiniGame;
	IMiniGame* createMiniGameField(GLGaeaServer* const pGaeaServer, const DWORD dwGaeaID, const DWORD dwMiniGameID, const float fBeginTime, const SNATIVEID& NPCID, const DWORD dwNPCGaeaID);
	const bool destroyMiniGameField(IMiniGame*& pMiniGame);
}
