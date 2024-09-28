#include "pch.h"
#include "./GLFreePK.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../AgentServer/GLAgentServer.h"
#include "../../RanLogic/Msg/GLContrlServerMsg.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../SigmaCore/DebugInclude.h"

GLFreePK::GLFreePK(GLAgentServer* pServer)
    : m_pServer(pServer)
    , m_fTIMER(0.0f)
    , m_fPKTIME(0.0f)
    , m_fUPDATETIMER(300.0f)
{
}

GLFreePK::~GLFreePK()
{
}

void GLFreePK::StartFREEPKMODE( DWORD dwTIME )
{
	m_fPKTIME      = static_cast<float>( dwTIME );
	m_fUPDATETIMER = 300.0f;
} // end of decl

void GLFreePK::EndFREEPKMODE()
{
	m_fTIMER	   = 0.0f;
	m_fPKTIME	   = 0.0f;
	m_fUPDATETIMER = 300.0f;
} // end of decl

bool GLFreePK::FrameMove(float fElapsedAppTime)
{
	if ( IsFREEPKMODE() )
	{
		m_fUPDATETIMER -= fElapsedAppTime;
		if ( m_fUPDATETIMER < 0.0f )
		{
			// �����̺�Ʈ ���� ���� ��ο��� �˸�
			GLMSG::NET_CHAT_LINK_FB NetMsg;	
			NetMsg.nType	= NET_MSG_CHAT_LINK_FB;
			NetMsg.emType	= CHAT_TYPE_GLOBAL;
			NetMsg.SetChatMsg(ID2SERVERTEXT("BRIGHT_EVENT_MSG", 0));
			m_pServer->SENDTOALLCLIENT ( &NetMsg );
			NetMsg.SetChatMsg(ID2SERVERTEXT("BRIGHT_EVENT_MSG", 1));
			m_pServer->SENDTOALLCLIENT ( &NetMsg );
			NetMsg.SetChatMsg(ID2SERVERTEXT("BRIGHT_EVENT_MSG", 2 ));
			m_pServer->SENDTOALLCLIENT ( &NetMsg );

			m_fUPDATETIMER = 300.0f;
		}

		m_fPKTIME -= fElapsedAppTime;
		if ( m_fPKTIME <= 0.0f )
		{
			m_fPKTIME = 0.0f;

			//	Note : ���� �̺�Ʈ ����˸�
			GLMSG::SNET_GM_FREEPK NetMsgFld;
			NetMsgFld.dwPKTIME = 0;

			m_pServer->SENDTOALLCHANNEL ( &NetMsgFld ); // �ʵ忡 �˸���

			GLMSG::SNET_GM_FREEPK_BRD NetMsgBrd;
			NetMsgBrd.dwPKTIME	   = 0;
			NetMsgBrd.bSTATEUPDATE = false;
			m_pServer->SENDTOALLCLIENT ( &NetMsgBrd );  // ��ο��� �˸�
		}
		return true;
	}
	return false;
} // end of decl