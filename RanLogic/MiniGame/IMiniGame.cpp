#include "pch.h"

#include "IMiniGame.h"

namespace MiniGame
{
	IMiniGame::IMiniGame(const DWORD dwGaeaID, const DWORD dwMiniGameID, const float fBeginTime, const SNATIVEID& NPCID, const DWORD dwNPCGaeaID)
		: m_dwMiniGameID(dwMiniGameID)
		, m_fBeginTime(fBeginTime)
		, m_NPCID(NPCID)		
		, m_dwNPCGaeaID(dwNPCGaeaID)
		, m_dwOwnerGaeaID(dwGaeaID)
	{
	}
	IMiniGame::~IMiniGame(void)
	{
	}
}

