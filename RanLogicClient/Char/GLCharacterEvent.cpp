#include "../pch.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"

#include "../../RanLogic/Event/GLIPEvent.h"

#include "./GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLCharacter::MsgEventFactBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_EVENTFACT_BRD *pNetMsg = (GLMSG::SNETPC_EVENTFACT_BRD *)pMsg;

    std::string strText;
    switch ( pNetMsg->emType )
    {
    case EMGM_EVENT_SPEED:
        strText = sc::string::format( ID2GAMEINTEXT("EMEVENTFACT_SPEED"), pNetMsg->wValue ).c_str();
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, strText );
        break;

    case EMGM_EVENT_ASPEED:
        strText = sc::string::format( ID2GAMEINTEXT("EMEVENTFACT_ASPEED"), pNetMsg->wValue ).c_str();
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, strText );
        break;

    case EMGM_EVENT_ATTACK:
        strText = sc::string::format( ID2GAMEINTEXT("EMEVENTFACT_ATTACK"), pNetMsg->wValue ).c_str();
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, strText );
        break;
    };

    m_sEVENTFACT.SetEVENT( pNetMsg->emType, pNetMsg->wValue );
}


void GLCharacter::MsgEventFactEndBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_EVENTFACT_END_BRD *pNetMsg = (GLMSG::SNETPC_EVENTFACT_END_BRD *)pMsg;

    switch( pNetMsg->emType )
    {
    case EMGM_EVENT_SPEED:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMEVENTFACT_SPEED_END") );
        break;

    case EMGM_EVENT_ASPEED:	
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMEVENTFACT_ASPEED_END") );
        break;

    case EMGM_EVENT_ATTACK:	
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMEVENTFACT_ATTACK_END") );
        break;
    }

    m_sEVENTFACT.ResetEVENT( pNetMsg->emType );
}


void GLCharacter::MsgEventFactInfo(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_EVENTFACT_INFO *pNetMsg = (GLMSG::SNETPC_EVENTFACT_INFO*)pMsg;

    m_sEVENTFACT = pNetMsg->sEVENTFACT;
}

void GLCharacter::MsgIPEventBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_IPEVENT_BRD* pNetMsg = (GLMSG::SNETPC_IPEVENT_BRD*)pMsg;

    ran_event::GLIPEvent::Instance()->SetValues( pNetMsg->m_AttackValue, pNetMsg->m_AttackSpeed, pNetMsg->m_MoveSpeed, pNetMsg->m_ItemDrop
        , pNetMsg->m_MoneyDrop, pNetMsg->m_Exp, pNetMsg->m_HPRecovery, pNetMsg->m_MPRecovery, pNetMsg->m_SPRecovery, pNetMsg->m_AllRecovery, pNetMsg->m_CriticalRate );

    std::string strText;

    if ( pNetMsg->bEnable )
    {
        strText = ID2GAMEWORD( "IP_EVENT", 0 );
        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::PALEGREEN, strText );

        strText = sc::string::format( ID2GAMEWORD( "IP_EVENT", 1 ), ran_event::GLIPEvent::Instance()->GetAttackValue() ).c_str();
        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::PALEGREEN, strText );

        strText = sc::string::format( ID2GAMEWORD( "IP_EVENT", 2 ), ran_event::GLIPEvent::Instance()->GetAttackSpeed() ).c_str();
        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::PALEGREEN, strText );

        strText = sc::string::format( ID2GAMEWORD( "IP_EVENT", 3 ), ran_event::GLIPEvent::Instance()->GetMoveSpeed() ).c_str();
        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::PALEGREEN, strText );

        strText = sc::string::format( ID2GAMEWORD( "IP_EVENT", 4 ), ran_event::GLIPEvent::Instance()->GetItempDrop() ).c_str();
        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::PALEGREEN, strText );

        strText = sc::string::format( ID2GAMEWORD( "IP_EVENT", 5 ), ran_event::GLIPEvent::Instance()->GetMoneyDrop() ).c_str();
        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::PALEGREEN, strText );

        strText = sc::string::format( ID2GAMEWORD( "IP_EVENT", 6 ), ran_event::GLIPEvent::Instance()->GetExp() ).c_str();
        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::PALEGREEN, strText );

        strText = sc::string::format( ID2GAMEWORD( "IP_EVENT", 7 ), ran_event::GLIPEvent::Instance()->GetHPRecovery() ).c_str();
        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::PALEGREEN, strText );

        strText = sc::string::format( ID2GAMEWORD( "IP_EVENT", 8 ), ran_event::GLIPEvent::Instance()->GetMPRecovery() ).c_str();
        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::PALEGREEN, strText );

        strText = sc::string::format( ID2GAMEWORD( "IP_EVENT", 9 ), ran_event::GLIPEvent::Instance()->GetSPRecovery() ).c_str();
        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::PALEGREEN, strText );

        strText = sc::string::format( ID2GAMEWORD( "IP_EVENT", 10 ), ran_event::GLIPEvent::Instance()->GetAllRecovery() ).c_str();
        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::PALEGREEN, strText );

        strText = sc::string::format( ID2GAMEWORD( "IP_EVENT", 11 ), ran_event::GLIPEvent::Instance()->GetCriticalRate() ).c_str();
        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::PALEGREEN, strText );
    }
    else
    {
        if ( m_sEVENTFACT.bIPEvent )
        {
            strText = ID2GAMEWORD( "IP_EVENT", 12 );
            m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::PALEGREEN, strText );
        }
    }


    m_sEVENTFACT.bIPEvent = pNetMsg->bEnable;
}
