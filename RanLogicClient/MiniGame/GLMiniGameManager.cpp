#include "pch.h"

#include "../../RanLogic/Msg/GLContrlMiniGameMsg.h"
#include "../../RanLogic/NpcTalk/NpcTalk.h"
#include "../Land/GLLandManClient.h"
#include "../Char/GLCharacter.h"
#include "../GLGaeaClient.h"
#include "GLMiniGameFactory.h"
#include "GLMiniGameManager.h"

namespace MiniGame
{
	GLManagerClient::GLManagerClient(GLGaeaClient* const pGaeaClient)
		: m_pGaeaClient(pGaeaClient)
		, m_pMiniGame(NULL)
	{
	}

	GLManagerClient::~GLManagerClient(void)
	{
	}

	void GLManagerClient::FrameMove(const float fTime, const float fElapsedTime)
	{
		if ( m_pMiniGame != NULL )
			m_pMiniGame->FrameMove(fTime, fElapsedTime);
	}

	void GLManagerClient::Reset(void)
	{
		if ( m_pMiniGame != NULL )
			MiniGame::destroyMiniGameClient(m_pMiniGame);
	}

	void GLManagerClient::RequestOpen(const DWORD dwMiniGameID, const SNATIVEID& NPCID, const DWORD dwNPCGaeaID) const
	{
		GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
		if ( pCharacter->UserLevel() < USER_GM3 )
		{ // GM이 아닌 경우 해당 NPC가 존재 하고, 반경 내에 있는지 검사;
			GLLandManClient* const pLandManager = m_pGaeaClient->GetActiveMap();
			if ( pLandManager == NULL )
				return;

			const std::tr1::shared_ptr<GLCrowClient> pNPC = pLandManager->GetCrow(dwNPCGaeaID);
			if ( pNPC == NULL )
				return;

            if ( !pNPC->IsNpcActionable(pCharacter->GetPosition(), pCharacter->GETBODYRADIUS()) )
                return;
		}

		GLMSG::NET_MINIGAME_REQUEST_OPEN NetMsg(dwMiniGameID, NPCID, dwNPCGaeaID);
		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
	}

	void GLManagerClient::RequestClose(void) const
	{
		if ( m_pMiniGame == NULL )
			return;

		GLMSG::NET_MINIGAME_REQUEST_CLOSE NetMsg;
		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
	}	

	const IMiniGame* const GLManagerClient::GetMiniGame(void) const
	{
		return m_pMiniGame;
	}

	const IMiniGame* const GLManagerClient::GetMiniGame(const EMMINIGAME_TYPE emMiniGameType) const
	{
		if ( m_pMiniGame == NULL )
			return NULL;

		return m_pMiniGame->m_dwMiniGameID == DWORD(emMiniGameType) ? m_pMiniGame : NULL;
	}

	const bool GLManagerClient::_doMiniGame(const DWORD dwGaeaID, const DWORD dwMiniGameID, const float fBeginTime, const SNATIVEID& NPCID, const DWORD dwNPCGaeaID)
	{
		if ( m_pMiniGame != NULL )
			return false;

		m_pMiniGame = MiniGame::createMiniGameClient(m_pGaeaClient, dwGaeaID, dwMiniGameID, fBeginTime, NPCID, dwNPCGaeaID);
		if ( m_pMiniGame == NULL )
			return false;

		m_pMiniGame->doBegin();

		switch ( dwMiniGameID )
		{
		case MiniGame::EMMINIGAME_ODDEVEN:
			GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_MiniGame_Dice_Open);
			break;
		default:
			return false;
		}			

		return true;
	}

	const bool GLManagerClient::_endMiniGame(void)
	{
		GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_MiniGame_Dice_Close);		

		if( !m_pMiniGame )
            return true;

        m_pMiniGame->doEnd();
		return MiniGame::destroyMiniGameClient(m_pMiniGame);
	}
	
}

