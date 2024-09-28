#include "pch.h"
#include "./NotifyClientGroupChatInvite.h"

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Json/json_spirit_reader.h"
#include "../../SigmaCore/Json/MinJson.h"
#include "../../SigmaCore/DebugInclude.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"

//#include "../../RanLogic/Character/GLCharDefine.h"
#include "../../RanLogic/Network/NetLogicDefine.h"
#include "../../RanLogic/Msg/GLContrlMsg.h"

#include "../GLGaeaClient.h"

NotifyClientGroupChatInvite::NotifyClientGroupChatInvite(
    GLGaeaClient* pGaeaClient,
    const sc::MGUID& Guid, 
    const __time64_t& Date )
    : INotifyClientBase( pGaeaClient, Guid, Date, gamelogic::notify::GROUPCHAR_INVITE )
{
}

NotifyClientGroupChatInvite::~NotifyClientGroupChatInvite()
{
}

bool NotifyClientGroupChatInvite::Parsing( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_GCHAT_INVITE_CAC *msg = (GLMSG::SNET_GCHAT_INVITE_CAC *)nmg;

	if( msg )
	{
		m_dwKey = msg->dwKey;
		std::string  strChaNameOwner(msg->szChaNameOwner);
		m_strChaNameOwner = strChaNameOwner;
		m_dwChaNumWhom =  msg->dwChaNumWhom;

		return true;
	}
    return false;
}

std::string NotifyClientGroupChatInvite::Description() const
{
    return m_strChaNameOwner;
}

std::string NotifyClientGroupChatInvite::GetMessageBoxText() const
{
    return sc::string::format( ID2GAMEWORD("GROUP_MODAL_WINDOW_INVITE_MSG"), Description().c_str() );
}

void NotifyClientGroupChatInvite::SendAccept()
{
	// 수락 패킷 전송
	GLMSG::SNET_GCHAT_INVITE_CAC_FB NetMsg;
	NetMsg.bAccept = TRUE;
	NetMsg.dwKey = m_dwKey;
	NetMsg.dwChaWhom = m_dwChaNumWhom;
	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
}

void NotifyClientGroupChatInvite::SendRefused()
{
	// 거절 패킷 전송
	GLMSG::SNET_GCHAT_INVITE_CAC_FB NetMsg;
	NetMsg.bAccept = FALSE;
	NetMsg.dwKey = m_dwKey;
	NetMsg.dwChaWhom = m_dwChaNumWhom;
	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
}