#include "../pch.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"

#include "./GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


void GLCharacter::MsgChatLoudSpeakerFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_CHAT_LOUDSPEAKER_FB *pNetMsg = (GLMSG::SNETPC_CHAT_LOUDSPEAKER_FB *) pMsg;
    switch ( pNetMsg->emFB )
    {
    case EMCHAT_LOUDSPEAKER_FAIL:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHAT_LOUDSPEAKER_FAIL") );
        break;

    case EMCHAT_LOUDSPEAKER_OK:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCHAT_LOUDSPEAKER_OK") );
        break;

    case EMCHAT_LOUDSPEAKER_NOITEM:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHAT_LOUDSPEAKER_NOITEM") );
        break;

    case EMCHAT_LOUDSPEAKER_BLOCK:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHAT_LOUDSPEAKER_BLOCK") );
        break;
    };
}

void GLCharacter::MsgChatAreaFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_CHAT_AREA_FB *pNetMsg = (GLMSG::SNETPC_CHAT_AREA_FB *) pMsg;
    switch ( pNetMsg->emFB )
    {
    case EMCHAT_AREA_FAIL:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHAT_AREA_FAIL") );
        break;
    case EMCHAT_AREA_OK:
        {
            if ( GLCONST_CHAR::dwCHAT_AREA_CHARGE > 0 ) 
            {
                std::string strText = sc::string::format( ID2GAMEINTEXT("EMCHAT_AREA_OK"), GLCONST_CHAR::dwCHAT_AREA_CHARGE ).c_str();
                m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, strText );
            }
        }
        break;
    case EMCHAT_AREA_NOMONEY:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHAT_AREA_NOMONEY") );
        break;
    case EMCHAT_AREA_BLOCK:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHAT_AREA_BLOCK") );
		break;
	case EMCHAT_AREA_UNAPPLY:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHAT_AREA_UNAPPLY") );
        break;
    }
}


void GLCharacter::MsgChatPartyRecruitFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_CHAT_PARTY_RECRUIT_FB *pNetMsg = (GLMSG::SNETPC_CHAT_PARTY_RECRUIT_FB *) pMsg;
    switch ( pNetMsg->emFB )
    {
    case EMCHAT_PARTY_RECRUIT_FAIL:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHAT_PARTY_RECRUIT_FAIL") );
        break;
    case EMCHAT_PARTY_RECRUIT_OK:
        {
            m_fPartyRecruitTime = 0.0f;
            if ( GLCONST_CHAR::dwCHAT_PARTY_RECRUIT_CHARGE > 0 ) 
            {
                std::string strText = sc::string::format( ID2GAMEINTEXT("EMCHAT_PARTY_RECRUIT_OK"), GLCONST_CHAR::dwCHAT_PARTY_RECRUIT_CHARGE ).c_str();
                m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, strText );
            }
        }
        break;
    case EMCHAT_PARTY_RECRUIT_NOMONEY:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHAT_PARTY_RECRUIT_NOMONEY") );
        break;
    case EMCHAT_PARTY_RECRUIT_BLOCK:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHAT_PARTY_RECRUIT_BLOCK") );
        break;
    case EMCHAT_PARTY_RECRUIT_TIME:
        {
            int nTime = (int)GLCONST_CHAR::fCHAT_PARTY_RECRUIT_TIME;
            std::string strText = sc::string::format( ID2GAMEINTEXT("EMCHAT_PARTY_RECRUIT_TIME"), nTime / 60 ).c_str();
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, strText );
            m_fPartyRecruitTime = nTime - pNetMsg->fPartyRecruitTime;
        }
        break;
	case EMCHAT_PARTY_RECRUIT_UNAPPLY:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHAT_PARTY_RECRUIT_UNAPPLY") );
		break;
    }
}

void GLCharacter::MsgChatFactionFb(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNETPC_CHAT_FACTION_FB* pNetMsg =
		static_cast< GLMSG::SNETPC_CHAT_FACTION_FB* >( pMsg );
	if ( NULL == pNetMsg )
		return;

	switch ( pNetMsg->emFB )
	{
	case EMCHAT_FACTION_FAIL:
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::DISABLE,
			ID2GAMEINTEXT( "EMCHAT_FACTION_FAIL" ) );
		break;
	case EMCHAT_FACTION_OK:
		break;
	case EMCHAT_FACTION_UNAPPLY:
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::DISABLE,
			ID2GAMEINTEXT( "EMCHAT_FACTION_UNAPPLY" ) );
		break;
	case EMCHAT_FACTION_BLOCK:
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::DISABLE,
			ID2GAMEINTEXT( "EMCHAT_FACTION_BLOCK" ) );
		break;
	}
}