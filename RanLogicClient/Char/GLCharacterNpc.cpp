#include "../pch.h"

#include "../../Sigmacore/String/StringFormat.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"

#include "../../RanLogic/NpcTalk/NpcDialogue.h"
#include "../../RanLogic/NpcTalk/NpcDialogueCase.h"
#include "../../RanLogic/NpcTalk/NpcTalk.h"
#include "../../RanLogic/NpcTalk/NpcTalkCondition.h"
#include "../../RanLogic/NpcTalk/NpcTalkMan.h"

#include "../MiniGame/GLMiniGameManager.h"
#include "../Land/GLLandManClient.h"
#include "../GLGaeaClient.h"
#include "./GLCharacter.h"

#include "../NPCTalk/GLNpcTalkClient.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CNpcTalkControl::NPCMAP* GLCharacter::FindDefaultTalk( const std::string& strTalkFile )
{
	std::tr1::shared_ptr< NpcTalkMan > spTalkMan = GLogicData::GetInstance().GetNpcTalkMan();
	if( !spTalkMan )
		return NULL;

	std::tr1::shared_ptr< CNpcDialogueSet > spDlgSet = spTalkMan->Find( strTalkFile );
	if( !spDlgSet )
		return NULL;

	const DWORD nBeginDlg = 1;
	std::tr1::shared_ptr< CNpcDialogue > spDlg = spDlgSet->GetDialogue( nBeginDlg );
	if( !spDlg )
		return NULL;
	
	const DWORD nPartyMember = m_pGaeaClient->GetMyPartyClient()->GetNMember();

	CTime cCurSvrTime = m_pGaeaClient->GetCurrentTime();
	DWORD nDlgCase = spDlg->FindConditionSatisfied( this, nPartyMember, cCurSvrTime.GetHour() );
	if( nDlgCase >= UINT_MAX )
		return NULL;

	std::tr1::shared_ptr< CNpcDialogueCase > spDlgCase = spDlg->GetDlgCase( nDlgCase );
	if( !spDlgCase )
		return NULL;

	std::tr1::shared_ptr< CNpcTalkControl > spTalkControl = spDlgCase->GetTalkControl();
	if( !spTalkControl )
		return NULL;

	CNpcTalkControl::NPCMAP* pTalkMap = spTalkControl->GetTalkMap();
	if( !pTalkMap )
		return NULL;

	if( pTalkMap->empty() )
		return NULL;

	return pTalkMap;
}

HRESULT GLCharacter::MsgNpcTalkBasic ( DWORD dwNpcGlobID, DWORD nAction, DWORD dwTalkID )
{
    // Need Add New Item Con.
    GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
    if ( !pLand )	
        return E_FAIL;

    std::tr1::shared_ptr<GLCrowClient> spCrow = pLand->GetCrow(dwNpcGlobID);
    if (!spCrow)	
        return E_FAIL;

    // NPC 유효성 체크
    if ( !spCrow->IsNpcActionable(GetPosition(), GETBODYRADIUS()))
    {
        return S_OK;
    }

    switch ( nAction )
    {
    case SNpcTalk::EM_STORAGE:
        {
			GLNpcTalkClient::GetInstance().SetNpcID( spCrow->m_sNativeID );
			GLNpcTalkClient::GetInstance().SetNpcGlobalID( dwNpcGlobID );
			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_Locker_Open,
				"-dw", dwNpcGlobID );
        }
        break;
    case SNpcTalk::EM_MARKET:
        {
			GLNpcTalkClient::GetInstance().SetNpcID( spCrow->m_sNativeID );
			GLNpcTalkClient::GetInstance().SetNpcGlobalID( dwNpcGlobID );
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_NpcTalk_OpenMarket );
        }
        break;
    case SNpcTalk::EM_CURE:
        {
            m_pGaeaClient->GetCharacter()->ReqCure ( dwNpcGlobID, dwTalkID );
        }
        break;
    case SNpcTalk::EM_STARTPOINT:
        {
            m_pGaeaClient->GetCharacter()->ReqReGenGate ( dwNpcGlobID );
        }
        break;

    case SNpcTalk::EM_CHAR_RESET:
        {
            m_pGaeaClient->GetCharacter()->ReqCharReset ( dwNpcGlobID );
        }
        break;

        // Need Add New Item Con.
    case SNpcTalk::EM_ITEM_TRADE:
        {			
        }
        break;

    case SNpcTalk::EM_BUSSTATION:
        {
            //	NOTE : 버스 정류장 인터페이스 로드.			
        }
        break;

    case SNpcTalk::EM_CLUB_NEW:
		{
			//	NOTE : 클럽 만들기 신청
			GLNpcTalkClient::GetInstance().SetNpcGlobalID( dwNpcGlobID );

			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_NpcTalk_CreateClub );
		}
		break;

    case SNpcTalk::EM_CLUB_UP:
        {

            m_pGaeaClient->ReqClubRank(dwNpcGlobID);
        }
        break;

    case SNpcTalk::EM_CD_CERTIFY:
        {
            m_pGaeaClient->ReqCDCertify(dwNpcGlobID);
        }
        break;

    case SNpcTalk::EM_COMMISSION:
        {
            // 수수료 지정
			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_SetCommission );
        }
        break;

    case SNpcTalk::EM_CLUB_STORAGE:
        {
			if( m_pGaeaClient->ReqGetClubStorage() )
			{
				GLNpcTalkClient::GetInstance().SetNpcID( spCrow->m_sNativeID );
				GLNpcTalkClient::GetInstance().SetNpcGlobalID( dwNpcGlobID );

				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_Club_OpenStorage );
			}
        }
        break;

    case SNpcTalk::EM_ITEM_REBUILD:	// ITEMREBUILD_MARK
        {
			GLNpcTalkClient::GetInstance().SetNpcID( spCrow->m_sNativeID );
			GLNpcTalkClient::GetInstance().SetNpcGlobalID( dwNpcGlobID );

			GLWidgetScript::GetInstance().LuaCallFunc< bool >( 
				NSWIDGET_SCRIPT::g_strFunc_RebuildCard_Open,
				"-b, -n",
				false,
				static_cast< int >( m_emTypeItemRemodelState ) );
        }
        break;

    case SNpcTalk::EM_ODDEVEN:
        {
			m_pGaeaClient->GetMiniGameManager()->RequestOpen(MiniGame::EMMINIGAME_ODDEVEN, spCrow->m_sNativeID, dwNpcGlobID);
        }
        break;
	//case SNpcTalk::EM_TEXASHOLDEM:
	//	{
	//		m_pGaeaClient->GetMiniGameManager()->RequestOpen(MiniGame::EMMINIGAME_TEXASHOLDEM, spCrow->m_sNativeID, dwNpcGlobID);
	//	}
	//	break;
    case SNpcTalk::EM_RECOVERY_EXP:
        {
            m_pGaeaClient->GetCharacter()->ReqGetReExpNpc( dwNpcGlobID );
        }
        break;
    case SNpcTalk::EM_RANDOM_PAGE:
        {
        }
        break;
    case SNpcTalk::EM_ITEMSEARCH_PAGE:
        {
			//!! SF_TODO
            //m_pInterface->SetItemSearchResultWindowClose();
            //m_pInterface->SetItemShopSearchWindowOpen();
        }
        break;

    case SNpcTalk::EM_ATTENDANCE_BOOK:
        {
			if ( GLUseFeatures::GetInstance().IsUsingActivity() )
			{
				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_StudentRecord_Open );
			}
        }
        break;
    case SNpcTalk::EM_ITEM_MIX:
        {
			//!! SF_TODO
            //m_pInterface->SetItemMixWindowOpen( spCrow->m_sNativeID, dwNpcGlobID );
        }
        break;

	case SNpcTalk::EM_HIRE_SUMMON:
		{
		}
		break;
    };

    return S_OK;
}

void GLCharacter::MsgReqDialogueWindowOpenFb(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNETPC_DIALOGUEWINDOW_OPEN_FB* pNetMsg = ( GLMSG::SNETPC_DIALOGUEWINDOW_OPEN_FB* ) pMsg;

	switch( pNetMsg->emFB )
	{
	case EMREQ_NPCTALK_FB_FAIL :
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DIALOGUEWINDOW_OPEN_FB_FAIL") );
		}
		break;

	case EMREQ_NPCTALK_FB_OK :
		{
			GLNpcTalkClient::GetInstance().SetTalkData( pNetMsg->sNPCID, m_NpcLastClick, pNetMsg->emCrow );
		}
		break;
	}
}

void GLCharacter::MsgReqNpcTalkBasicFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_NPCTALK_BASIC_FB* pNetMsg = ( GLMSG::SNETPC_REQ_NPCTALK_BASIC_FB* ) pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMREQ_NPCTALK_FB_FAIL:
        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_NPCTALK_FB_FAIL") );
        break;
    case EMREQ_NPCTALK_FB_OK:
        MsgNpcTalkBasic( pNetMsg->dwNpcGlobID, pNetMsg->nAction, pNetMsg->dwTalkID );
        break;
    case EMREQ_NPCTALK_FB_NPC:
        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_NPCTALK_FB_NPC") );
        break;
    case EMREQ_NPCTALK_FB_TYPE:
        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_NPCTALK_FB_TYPE") );
        break;
    case EMREQ_NPCTALK_FB_DISTANCE:
        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_NPCTALK_FB_DISTANCE") );
        break;
    }
}

void GLCharacter::MsgNpcRecallFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_INVEN_NPC_RECALL_FB* pNetMsg = ( GLMSG::SNET_INVEN_NPC_RECALL_FB* ) pMsg;

    switch( pNetMsg->emFB )
    {
    case EMREQ_NPC_RECALL_FB_FAIL:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_NPC_RECALL_FB_FAIL") );
        break;
    case EMREQ_NPC_RECALL_FB_OK:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_NPC_RECALL_FB_OK") );
        break;
    case EMREQ_NPC_RECALL_FB_NOITEM:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_NPC_RECALL_FB_NOITEM") );
        break;
    case EMREQ_NPC_RECALL_FB_NPC:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_NPC_RECALL_FB_NPC") );
        break;
    case EMREQ_NPC_RECALL_FB_POS:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_NPC_RECALL_FB_POS") );
        break;
    }
}

void GLCharacter::MsgNpcItemTradeFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_NPC_ITEM_TRADE_FB *pNetNsg = (GLMSG::SNETPC_REQ_NPC_ITEM_TRADE_FB *)pMsg;
    switch ( pNetNsg->emFB )
    {
    case EMNPC_ITEM_TRADE_SUCCEED:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMNPC_ITEM_TRADE_SUCCEED") );
        break;
    case EMNPC_ITEM_TRADE_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMNPC_ITEM_TRADE_FAIL") );
        break;
    case EMNPC_ITEM_TRADE_ITEM_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMNPC_ITEM_TRADE_ITEM_FAIL") );
        break;
    case EMNPC_ITEM_TRADE_INSERT_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMNPC_ITEM_TRADE_INSERT_FAIL") );
        break;
    };
}


void GLCharacter::MsgRecoveryNpcFb(NET_MSG_GENERIC* pMsg)
{    
    // 경험치회복_정의_Npc
    if (m_pGaeaClient->CanRecoveryExpNpc())
    {
        GLMSG::SNETPC_REQ_RECOVERY_NPC_FB* pPacket = (GLMSG::SNETPC_REQ_RECOVERY_NPC_FB*) pMsg;

        switch ( pPacket->emFB )
        {
        case EMREQ_RECOVERY_NPC_FB_FAIL:
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RECOVERY_NPC_FB_FAIL") );
            break;
        case EMREQ_RECOVERY_NPC_FB_OK:
            {
                std::string strText = sc::string::format( ID2GAMEINTEXT("EMREQ_RECOVERY_NPC_FB_OK"), pPacket->nReExp ).c_str();
                m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, strText );
            }
            break;
        case EMREQ_RECOVERY_NPC_FB_NOMONEY:
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RECOVERY_NPC_FB_NOMONEY") );
            break;
        case EMREQ_RECOVERY_NPC_FB_NOREEXP:
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RECOVERY_NPC_FB_NOREEXP") );
            break;
        case EMREQ_RECOVERY_NPC_FB_NOTUSE: // different enum types are compared
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RECOVERY_NPC_FB_NOTUSE") );
            break;
        };

        if ( pPacket->emFB != EMREQ_RECOVERY_NPC_FB_OK ) // different enum types are compared
        {
            ReqResurrect(EM_RESURRECTION_NOMAL);

			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_EnterGameStage );
        }
    }
//#endif
}

void GLCharacter::MsgGetExpRecoveryNpcFb(NET_MSG_GENERIC* pMsg)
{    
    // 경험치회복_정의_Npc
    if (m_pGaeaClient->CanRecoveryExpNpc())
    {
        GLMSG::SNETPC_REQ_GETEXP_RECOVERY_NPC_FB* pPacket = (GLMSG::SNETPC_REQ_GETEXP_RECOVERY_NPC_FB*) pMsg;

        // 회복할 경험치가 없습니다.
        if ( pPacket->nReExp <= 0 )	
        {
            m_pGaeaClient->PrintMsgText( 
                NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "EMREQ_RECOVERY_NPC_FB_NOREEXP" ) );

            return;
        }

        // 소지 금액이 모자르다면 부활시킨다.
        if ( pPacket->nDecMoney > GetInvenMoney() )	
        {
            m_pGaeaClient->PrintMsgText( 
                NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "EMREQ_RECOVERY_NPC_FB_NOMONEY" ) );				

            return;				
        }

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_RecoveryNpcExp,
			"-dw, -l, -l", pPacket->dwNpcGlobID, pPacket->nReExp, pPacket->nDecMoney );	
    }
//#endif
}

//void GLCharacter::MsgCheckAddRebuyItem(NET_MSG_GENERIC* pMsg)
//{
//    GLMSG::SNET_ADD_REBUY_ITEM_FC* pPacket = (GLMSG::SNET_ADD_REBUY_ITEM_FC*)pMsg;
//
//    if ( !RANPARAM::bCheckRebuySuppliesRebuyPage )
//    {
//        SITEM* pItem = GLogicData::GetInstance().GetItem( pPacket->sItemCustom.Id() );
//        if ( !pItem )
//            return;
//
//        if ( pItem->BasicType() == ITEM_CURE
//            || pItem->BasicType() == ITEM_ARROW
//            || pItem->BasicType() == ITEM_CHARM
//            || pItem->BasicType() == ITEM_BULLET )
//            return;
//    }
//
//    GLMSG::SNET_ADD_REBUY_ITEM_CA NetAddRebuyItem( pPacket->sItemCustom, pPacket->Price );
//    m_pGaeaClient->NETSENDTOAGENT( &NetAddRebuyItem );
//}

void GLCharacter::MsgAddRebuyItem(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_ADD_REBUY_ITEM_AC* pPacket = (GLMSG::SNET_ADD_REBUY_ITEM_AC*)pMsg;

    int nRebuyListSize = static_cast<int>( m_vecRebuy.size() );

    if ( nRebuyListSize >= GLCONST_CHAR::wMaxRebuyList )
    {
        m_vecRebuy.erase( m_vecRebuy.begin() );
    }

    RebuyItem::SREBUY_ITEM sRebuyItem( pPacket->sItemCustom, pPacket->Price );
    m_vecRebuy.push_back( sRebuyItem );

    GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_NpcTalk_UpdateRebuy );
}

void GLCharacter::MsgBuyRebuyItem(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_BUY_REBUY_ITEM_FC* pPacket = (GLMSG::SNET_BUY_REBUY_ITEM_FC*)pMsg;

    if ( m_vecRebuy.empty() )
    {
        m_pGaeaClient->PrintMsgText( 
            NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT( "BUY_REBUYITEM_FAILED" ) );

        return;
    }

    SITEMCUSTOM& sItemCustom = pPacket->sItemCustom;
    LONGLONG& llPrice = pPacket->Price;

    if ( pPacket->emFB == EMBUYREBUYITEM_SUCCESS
        || pPacket->emFB == EMBUYREBUYITEM_FAILED
        || pPacket->emFB == EMBUYREBUYITEM_OVER_DURATION )
    {
        RebuyItem::REBUY_VEC_ITER iter = m_vecRebuy.begin();

        for ( ; iter != m_vecRebuy.end(); iter++ )
        {
            RebuyItem::SREBUY_ITEM& sRebuyItem = (*iter);

            if ( sRebuyItem.ItemCustom() == sItemCustom &&
                sRebuyItem.Price() == llPrice )
            {
                m_vecRebuy.erase( iter );
				break;
            }
        }

        if ( pPacket->emFB == EMBUYREBUYITEM_SUCCESS )
        {
            m_pGaeaClient->PrintMsgText( 
                NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT( "BUY_REBUYITEM_SUCCESS" ) );
        }
        else if ( pPacket->emFB == EMBUYREBUYITEM_FAILED )
        {
            m_pGaeaClient->PrintMsgText( 
                NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT( "BUY_REBUYITEM_FAILED" ) );
        }
        else if ( pPacket->emFB == EMBUYREBUYITEM_OVER_DURATION )
        {
            m_pGaeaClient->PrintMsgText( 
                NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT( "BUY_REBUYITEM_OVER_DURATION" ) );
        }
    }
    else if ( pPacket->emFB == EMBUYREBUYITEM_NOT_ENOUGH_INVEN )
    {
        m_pGaeaClient->PrintMsgText( 
            NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT( "BUY_REBUYITEM_NOT_ENOUGH_INVEN" ) );
    }
    else if ( pPacket->emFB == EMBUYREBUYITEM_NOT_ENOUGH_MONEY )
    {
        m_pGaeaClient->PrintMsgText( 
            NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT( "BUY_REBUYITEM_NOT_ENOUGH_MONEY" ) );
    }
    else if ( pPacket->emFB == EMBUYREBUYITEM_CAN_NOT_BUY_ITEM )
    {
        m_pGaeaClient->PrintMsgText( 
            NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT( "BUY_REBUYITEM_CAN_NOT_BUY_ITEM" ) );
    }

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_NpcTalk_UpdateRebuy );
}

void GLCharacter::MsgCheckRebuySupplies(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_CHECK_REBUY_SUPPLIES* pPacket = (GLMSG::SNET_CHECK_REBUY_SUPPLIES*)pMsg;

    RANPARAM::bCheckRebuySuppliesRebuyPage = pPacket->m_bCheck;

	//!! SF_TODO
    //m_pInterface->DialogueWindowCheckRebuySupplies( RANPARAM::bCheckRebuySuppliesRebuyPage );
}