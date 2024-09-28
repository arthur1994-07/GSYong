#include "pch.h"

#include "../RanLogic/Character/GLCharDefine.h"

#include "./EmulServer.h"
#include "./Database/DbDefine.h"
#include "./FieldServer/GLGaeaServer.h"
#include "./AgentServer/GLAgentServer.h"
#include "../SigmaCore/DebugInclude.h"


#include "../SigmaCore/DebugInclude.h"


EmulServerField::EmulServerField(
	GLGaeaServer* pServer )
	: m_pServer(pServer)
{
}

void EmulServerField::_update_emul_msg(
	DWORD dwClientID,
	DWORD dwGaeaID,
	float fElapsTime )
{
	while (1)
	{
		if ( g_vecMsg.empty() ) 
		{
			return;
		}
		else 
		{
			CEMULMSG &sMsg = *(*g_vecMsg.begin());
			sMsg.fDelay -= fElapsTime;
			if ( sMsg.fDelay > 0.0f )
			{
				return;
			}
			else
			{
				m_pServer->MsgProcess(
					(NET_MSG_GENERIC*) sMsg.pBuffer,
					dwClientID,
					dwGaeaID );
				delete (*g_vecMsg.begin());
				g_vecMsg.pop_front();
			}
		}
	}
}

EmulServerAgent::EmulServerAgent(
	GLAgentServer* pServer )
	: m_pServer(pServer)
{	
}

void EmulServerAgent::_update_emul_msg(
	DWORD dwClientID,
	DWORD dwGaeaID,
	float fElapsTime )
{
	while (1)
	{
		if ( g_vecMsg.empty() ) 
		{
			return;
		}
		else 
		{
			CEMULMSG &sMsg = *(*g_vecMsg.begin());
			sMsg.fDelay -= fElapsTime;
			if ( sMsg.fDelay > 0.0f )
			{
				return;
			}
			else
			{
				m_pServer->MsgProcess(
					(NET_MSG_GENERIC*) sMsg.pBuffer,
					dwClientID,
					dwGaeaID );
				delete (*g_vecMsg.begin());
				g_vecMsg.pop_front();
			}
		}
	}
}