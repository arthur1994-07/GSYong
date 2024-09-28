#include "pch.h"

#include "./MiniGame/GLMiniGameManager.h"
#include "GLGaeaClient.h"

void GLGaeaClient::MsgMiniGame(NET_MSG_GENERIC* pMsg)
{
	if ( m_pMiniGameManager == NULL )
		return;

	m_pMiniGameManager->doMessage(pMsg);
}