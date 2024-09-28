#include "pch.h"

#include "../../RanLogic/MiniGame/IMiniGame.h"
#include "../../RanLogic/Msg/GLContrlMiniGameMsg.h"
#include "../Character/GLChar.h"
#include "../Crow/GLCrow.h"
#include "../FieldServer/GLGaeaServer.h"
#include "GLMiniGameFactory.h"
#include "GLMiniGameManager.h"

namespace MiniGame
{
	GLManagerField::GLManagerField(GLGaeaServer* const pGaeaServer)
		: m_pGaeaServer(pGaeaServer)
		, m_fCurrentTime(0.0f)
	{
	}

	GLManagerField::~GLManagerField(void)
	{
		for ( MiniGameMapCIter _iter = m_MiniGameMap.begin(); _iter != m_MiniGameMap.end(); ++_iter )
		{
			IMiniGame* pMiniGame = _iter->second;
			if ( pMiniGame == NULL )
				continue;

			pMiniGame->doEnd();
			MiniGame::destroyMiniGameField(pMiniGame);
		}

		m_MiniGameMap.clear();
	}

	const bool GLManagerField::doMiniGame(const DWORD dwGaeaID, const DWORD dwMiniGameID)
	{
		GLChar* const pPlayer = m_pGaeaServer->GetChar(dwGaeaID);
		if ( pPlayer == NULL )
		{
			endMiniGame(dwGaeaID);
			return false;
		}

		if ( _doMiniGame(dwGaeaID, dwMiniGameID) == false )
			return false;

		return true;
	}
	const bool GLManagerField::doMiniGame(const DWORD dwGaeaID, const DWORD dwMiniGameID, const SNATIVEID& NPCID, const DWORD dwNPCGaeaID)
	{
		GLChar* const pPlayer = m_pGaeaServer->GetChar(dwGaeaID);
		if ( pPlayer == NULL )
		{
			endMiniGame(dwGaeaID);
			return false;
		}

		GLLandMan* const pLand = pPlayer->GetLandMan();
		if ( pLand == NULL )
		{
			endMiniGame(dwGaeaID, MiniGame::EMERROR_NO_LAND);
			return false;
		}

		GLCrow* const pNPC = pLand->GetCrow(dwNPCGaeaID);
		if ( pNPC == NULL )
		{
			endMiniGame(dwGaeaID, MiniGame::EMERROR_NO_NPC);
			return false;
		}

        if ( !pNPC->IsNpcInteractionAble(pPlayer->GetPosition(), pPlayer->GETBODYRADIUS()))
        {
            endMiniGame(dwGaeaID, MiniGame::EMERROR_NO_ACTIONABLE);
            return false;
        }

		if ( _doMiniGame(dwGaeaID, dwMiniGameID, NPCID, dwNPCGaeaID) == false )
			return false;

		return true;
	}

	const bool GLManagerField::_doMiniGame(const DWORD dwGaeaID, const DWORD dwMiniGameID, const SNATIVEID& NPCID /* = SNATIVEID(false)*/, const DWORD dwNPCGaeaID /* = GAEAID_NULL*/)
	{
		GLChar* const pPlayer = m_pGaeaServer->GetChar(dwGaeaID);

		MiniGameMapCIter iter = m_MiniGameMap.find(dwGaeaID);
		if ( iter != m_MiniGameMap.end() )
		{
			const IMiniGame* const pMiniGame = iter->second;			
			m_pGaeaServer->SENDTOCLIENT(pPlayer->ClientSlot(), 
				&GLMSG::NET_MINIGAME_DO_OPEN(pMiniGame->m_dwMiniGameID, MiniGame::EMERROR_ALREADY_BEGUN));
			return false;
		}

		IMiniGame* pMiniGame = MiniGame::createMiniGameField(m_pGaeaServer, dwGaeaID, dwMiniGameID, m_fCurrentTime, NPCID, dwNPCGaeaID);
		if ( pMiniGame == NULL )
		{
			m_pGaeaServer->SENDTOCLIENT(pPlayer->ClientSlot(), 
				&GLMSG::NET_MINIGAME_DO_OPEN(dwMiniGameID, MiniGame::EMERROR_CREATE_FAIL));
			return false;
		}

		m_MiniGameMap.insert(MiniGameMapValue(dwGaeaID, pMiniGame));		

		m_pGaeaServer->SENDTOCLIENT(pPlayer->ClientSlot(), 
			&GLMSG::NET_MINIGAME_DO_OPEN(dwMiniGameID, NPCID, dwNPCGaeaID, m_fCurrentTime, MiniGame::EMERROR_NO_ERROR));

		pMiniGame->doBegin();		

		return true;
	}

	const bool GLManagerField::endMiniGame(const DWORD dwGaeaID, const DWORD nErrorCode)
	{
		GLChar* const pPlayer = m_pGaeaServer->GetChar(dwGaeaID);
		if ( pPlayer == NULL )
		{
			MiniGameMapCIter iter = m_MiniGameMap.find(dwGaeaID);
			if ( iter == m_MiniGameMap.end() )
				return false;

			IMiniGame* pMiniGame = iter->second;

			pMiniGame->doEnd();
			MiniGame::destroyMiniGameField(pMiniGame);
			m_MiniGameMap.erase(iter);
			return true; // 플레이어가 NULL이면 알리지 않고 종료;			
		}
		
		MiniGameMapCIter iter = m_MiniGameMap.find(dwGaeaID);
		if ( iter == m_MiniGameMap.end() )
		{
			m_pGaeaServer->SENDTOCLIENT(pPlayer->ClientSlot(), 
				&GLMSG::NET_MINIGAME_DO_CLOSE(DWORD(MiniGame::EMMINIGAME_NSIZE), MiniGame::EMERROR_TYPE(nErrorCode)));
			return false;
		}

		IMiniGame* pMiniGame = iter->second;
		m_pGaeaServer->SENDTOCLIENT(pPlayer->ClientSlot(), 
			&GLMSG::NET_MINIGAME_DO_CLOSE(pMiniGame->m_dwMiniGameID, MiniGame::EMERROR_TYPE(nErrorCode)));

		pMiniGame->doEnd();
		MiniGame::destroyMiniGameField(pMiniGame);
		m_MiniGameMap.erase(iter);		

		return true;
	}

	void GLManagerField::FrameMove(const float fTime, const float fElapsedTime)
	{
		m_fCurrentTime = fTime;
		
		for ( MiniGameMapIter iter = m_MiniGameMap.begin(); iter != m_MiniGameMap.end(); ++iter )
		{
			IMiniGame* const pMiniGame = iter->second;
			if ( pMiniGame == NULL )
				continue;

			pMiniGame->FrameMove(fTime, fElapsedTime);
		}
	}	
}

