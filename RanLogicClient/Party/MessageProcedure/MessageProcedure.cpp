#include "pch.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../RanLogic/RANPARAM.h"
#include "../../../RanLogic/MSG/GLContrlMsg.h"
#include "../../../RanLogic/s_NetGlobal.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../RanLogicClient/Tournament/GLTournamentClient.h"
#include "../../../Ranlogicclient/GLGaeaClient.h"


#include "../../Notify/NotifyClientMan.h"
#include "../../Notify/NotifyClientFactory.h"
#include "../../Land/GLLandManClient.h"
#include "../../GLGaeaClient.h"
#include "../../Char/GLCharacter.h"

#include "../GLPartyManagerClient.h"
#include "../GLPartyTenderManagerClient.h"

#include "../../Chat/GLChatMan.h"

const bool GLPartyManagerClient::MsgPartyLureTarget(NET_MSG_GENERIC* pPartyMessage)
{
	GLMSG::SNET_PARTY_LURE_TAR* pNetMsg = (GLMSG::SNET_PARTY_LURE_TAR*)pPartyMessage;

	GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
	if ( pLand == NULL )
		return false;

	std::tr1::shared_ptr<GLCharClient> pChar = pLand->GetChar ( pNetMsg->dwMasterID );
	if ( pChar == NULL )
		return false;

	// �������� ��ȯ���� �ʿ� ������.
	//if ( m_pInterface->UiIsVisibleGroup ( PARTY_MODAL_WINDOW ) )
	//{
		//	Note : ���� ��Ƽ��û�� ���� �亯�� ���� ���� ���
		//		   ���� ������ ������ ��� �����Ѵ�.
	//	GLMSG::SNET_PARTY_LURE_TAR_ANS NetMsg( EMPARTY_LURE_REFUSE, pNetMsg->PartyID, 0, pNetMsg->dwMasterID, 0);
	//	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
	//}

	// ��ʸ�Ʈ�� ��Ƽ�� �Ҽ��� ����.
	// TOURNAMENT_UNIQ_DISABLE
	bool bTOURNAMNET = m_pGaeaClient->GetTournamentClient()->GetIsTournamentView();

	//	Note : ��Ƽ �ڵ� �ź�.
	//
	if ( RANPARAM::bDIS_PARTY || bTOURNAMNET )
	{
		//	��Ƽ ���� ó��
		GLMSG::SNET_PARTY_LURE_TAR_ANS NetMsg( EMPARTY_LURE_REFUSE, pNetMsg->PartyID, 0, pNetMsg->dwMasterID, 0);
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

		std::string strText = sc::string::format(ID2GAMEINTEXT("PARTY_LURE_REFUSE_AUTO"), pChar->GetName()).c_str();
		m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, strText.c_str());
		return true;
	}

	//	Note : ��û�� ��Ƽ �ɼ�.
	//	

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Party_MsgInvite,
		"-s, -dw, -dw, -dw, -dw, -b",
		pChar->GetName(), pNetMsg->PartyID, 0, pNetMsg->dwMasterID, 0, pNetMsg->bExpedition );

	return true;
}


const bool GLPartyManagerClient::MsgPartyLureTargetWithCharName(NET_MSG_GENERIC* pPartyMessage)
{
	GLMSG::SNET_PARTY_LURE_TAR_WITH_CHARNAME* pNetMsg = 
		(GLMSG::SNET_PARTY_LURE_TAR_WITH_CHARNAME*) pPartyMessage;

	std::string strMasterName = stringformat( "%1%", pNetMsg->szMasterName );

	// �������� ��ȯ���� �ʿ� ������.
	//if ( m_pInterface->UiIsVisibleGroup ( PARTY_MODAL_WINDOW ) )
	//{
		//	Note : ���� ��Ƽ��û�� ���� �亯�� ���� ���� ���
		//		   ���� ������ ������ ��� �����Ѵ�.
	//	GLMSG::SNET_PARTY_LURE_TAR_ANS NetMsg( EMPARTY_LURE_REFUSE, pNetMsg->partyID, 0, pNetMsg->dwMasterID, 0, SPartyOption() );
	//	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
	//}

	// ��ʸ�Ʈ�� ��Ƽ�� �Ҽ��� ����.
	// TOURNAMENT_UNIQ_DISABLE
	bool bTOURNAMNET = m_pGaeaClient->GetTournamentClient()->GetIsTournamentView();

	//	Note : ��Ƽ �ڵ� �ź�.
	//
	if ( RANPARAM::bDIS_PARTY || bTOURNAMNET)
	{
		//	��Ƽ ���� ó��
		GLMSG::SNET_PARTY_LURE_TAR_ANS NetMsg( EMPARTY_LURE_REFUSE, pNetMsg->partyID, 0, pNetMsg->dwMasterID, 0, SPartyOption() );
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

		std::string strText = sc::string::format(ID2GAMEINTEXT("PARTY_LURE_REFUSE_AUTO"), strMasterName.c_str()).c_str();
		m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, strText.c_str());
		return true;
	}

	/// ��Ƽ ���� ��Ʈ��ȭ.

	const DWORD dwItemOption(pNetMsg->partyOption.GetItemOption());
	const DWORD dwMoneyOption(pNetMsg->partyOption.GetMoneyOption());
	const DWORD dwItemFilter(pNetMsg->partyOption.GetItemFilter());

	CString strItemOption, strItemFilter, strMoneyOption, strPartyInfo;
	switch ( dwItemOption )
	{
	case EMPARTY_ITEM_LEADER_SHARE:
	case EMPARTY_ITEM_DICE:
		{
			strItemOption.Format("%s", ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION", dwItemOption));
			strItemFilter.Format("%s", ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION_FILTER", dwItemFilter));
			strItemOption.Format("%s(%s)", 
				ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION", dwItemOption),
				ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION_FILTER", dwItemFilter));
		}
		break;
	default:
		{
			strItemOption.Format("%s", ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION", dwItemOption));
		}
	}

	strMoneyOption.Format("%s", ID2GAMEWORD("PARTY_MODAL_MONEY_OPTION", dwMoneyOption));
	
	strPartyInfo.Format( " [ %s : %s ] [ %s : %s ] ] "
		, ID2GAMEWORD("PARTY_MODAL_CONDITION",0)
		, strItemOption
		, ID2GAMEWORD("PARTY_MODAL_CONDITION",1)
		, strMoneyOption );


	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Party_MsgInvite,
		"-s, -dw, -dw, -dw, -dw, -b, -s",
		strMasterName.c_str(), pNetMsg->partyID, pNetMsg->dwHashKey,
		pNetMsg->dwMasterID, pNetMsg->dwTimeKey, pNetMsg->bExpedition, strPartyInfo );

	return true;
}

const bool GLPartyManagerClient::MsgPartyFNew(NET_MSG_GENERIC* pPartyMessage)
{
	GLMSG::SNET_PARTY_FNEW* pNetMsg = ( GLMSG::SNET_PARTY_FNEW * ) pPartyMessage;

	GLPartyClient* const pParty = GetParty(pNetMsg->nPartyIndex);
	if ( pParty == NULL )
		return false;	
		
	pParty->MsgPartyFNew(pPartyMessage);
	pParty->SetOption( pNetMsg->sOption );

	if ( pParty->isMember(m_pGaeaClient->GETMYGAEAID()) )
    {
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Party_New );
		m_PartyID = pNetMsg->PartyID;
    }

	return true;
}

const bool GLPartyManagerClient::MsgPartyFNewUpdate(NET_MSG_GENERIC* pPartyMessage)
{
	GLMSG::SNET_PARTY_FNEW_UPDATE* pNetMsg = ( GLMSG::SNET_PARTY_FNEW_UPDATE * ) pPartyMessage;

	GLPartyClient* const pParty = GetParty(pNetMsg->nPartyIndex);
	if ( pParty == NULL )
		return false;	

	pParty->MsgPartyFNewUpdate(pPartyMessage);
	pParty->SetOption( pNetMsg->sOption );
	//!! SF_TODO
	//m_pInterface->SetPartyOption( pNetMsg->sOption );
	if ( pParty->isMember(m_pGaeaClient->GETMYGAEAID()) )
	{
		//!! SF_TODO
		/*if ( GLUseFeatures::GetInstance().IsUsingPartyWindowRenewal() )
		{
			m_pInterface->UiShowGroupFocus( MINI_EXPEDITION_WINDOW );
		}
		else
		{
			//������Ƽ �ý���
			m_pInterface->UiShowGroupFocus( MINIPARTY_WINDOW );
			m_pInterface->UiHideGroup( MINIPARTY_OPEN );
		}*/

		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Party_New );

		m_PartyID = pNetMsg->PartyID;
	}
	return true;
}

const bool GLPartyManagerClient::MsgPartyAdd(NET_MSG_GENERIC* pPartyMessage)
{
	GLMSG::SNET_PARTY_ADD* pNetMsg = (GLMSG::SNET_PARTY_ADD*) pPartyMessage;
	
	GLPartyClient* const pParty = GetParty(pNetMsg->PartyID);
	if ( pParty == NULL )
		return false;

	if ( pParty->GetPartyID() != pNetMsg->PartyID )
		return false;

	if( TRUE == pParty->MsgPartyAdd(pPartyMessage) )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Party_UpdateSlot );

		return true;
	}

	return false;
}

const bool GLPartyManagerClient::MsgPartyDel(NET_MSG_GENERIC* pPartyMessage)
{
	GLMSG::SNET_PARTY_DEL* pNetMsg = (GLMSG::SNET_PARTY_DEL*) pPartyMessage;

	GLPartyClient* const pParty = GetParty(pNetMsg->PartyID);
	if ( pParty == NULL )
		return false;

	const DWORD delMemberGaeaID(pNetMsg->delMemberGaeaID);
	const DWORD dwMyGaeaID(m_pGaeaClient->GETMYGAEAID());
	if ( delMemberGaeaID == dwMyGaeaID )
	{
		Reset();

		SPartyOption sResetOption;
		sResetOption.Reset();

		pParty->SetOption( sResetOption );

		switch( pNetMsg->eType )
		{
		case PARTY_ACTION_BAN_TYPE :
			{				
				m_pGaeaClient->PrintMsgText(
					NS_UITEXTCOLOR::DIALOGUE_MOUSEOVER,
					ID2GAMEINTEXT("PARTY_BAN_SELF_RESULT") );	
			}
			break;
		case PARTY_ACTION_SECEDE_TYPE :
			{
				m_pGaeaClient->PrintMsgText(
					NS_UITEXTCOLOR::DIALOGUE_MOUSEOVER,
					ID2GAMEINTEXT("PARTY_QUIT_SELF_RESULT") );		
			}
			break;
		//!! SF_TODO
		//default : m_pInterface->SetPartyOption( sResetOption );

		}
		
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Party_Dissolve );
	}
	else
	{		
		GLPartySlotClient* pMember = pParty->GetMemberByGaeaID(delMemberGaeaID);

		bool bMaster = pParty->isMaster(dwMyGaeaID);

		if( pMember )
		{
			switch( pNetMsg->eType )
			{
				case PARTY_ACTION_BAN_TYPE :
					{
						if( bMaster )
						{
							m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DIALOGUE_MOUSEOVER,	sc::string::format(ID2GAMEINTEXT("PARTY_BAN_MASTER_RESULT"), pMember->m_szName));
						}
						else
						{
							m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DIALOGUE_MOUSEOVER,	sc::string::format(ID2GAMEINTEXT("PARTY_BAN_OTHER_RESULT"), pMember->m_szName));
						}
					}
					break;
				case PARTY_ACTION_SECEDE_TYPE :
					{
						//if( bMaster )
						//{
						//	m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DIALOGUE_MOUSEOVER,	sc::string::format(ID2GAMEINTEXT("PARTY_BAN_MASTER_RESULT"), pMember->m_szName));
						//}
						//else
						//{
							m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DIALOGUE_MOUSEOVER, sc::string::format(ID2GAMEINTEXT("PARTY_QUIT_OTHER_RESULT"), pMember->m_szName));
						//}
					}
					break;
			}
		}
	}

	if( TRUE == pParty->MsgPartyDel(pPartyMessage) )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Party_UpdateSlot );
		return true;
	}

	return false;
}

const bool GLPartyManagerClient::MsgPartyDissolve(NET_MSG_GENERIC* pPartyMessage)
{
	GLMSG::SNET_PARTY_DISSOLVE* pNetMsg = (GLMSG::SNET_PARTY_DISSOLVE*)pPartyMessage;

	GLPartyClient* const pParty = GetParty(pNetMsg->PartyID);
	if ( pParty == NULL )
		return false;

	Reset();

	SPartyOption sResetOption;
	sResetOption.Reset();

	pParty->SetOption( sResetOption );

	m_pGaeaClient->PrintMsgText(
		NS_UITEXTCOLOR::DIALOGUE_MOUSEOVER,
		ID2GAMEINTEXT("PARTY_DISSOLVE_RESULT") );	

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Party_Dissolve );

	return TRUE;
}

const bool GLPartyManagerClient::MsgPartyAuthority( NET_MSG_GENERIC* pPartyMessage )
{
	GLMSG::SNET_PARTY_AUTHORITY* pNetMsg = (GLMSG::SNET_PARTY_AUTHORITY*)pPartyMessage;	

	GLPartyClient* const pParty = GetParty(pNetMsg->PartyID);
	if ( pParty == NULL )
		return false;
	
	if ( pNetMsg->PartyID.isValidExpedition() == true )
		m_pExpedition->SetMasterGaeaID(pNetMsg->dwNewMasterGaeaID, pNetMsg->dwPartyIndex);

	if( TRUE == pParty->MsgPartyAuthority(pPartyMessage) )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Party_UpdateSlot );
		return true;
	}

	return false;
}

const bool GLPartyManagerClient::MsgPartyMoveMap(NET_MSG_GENERIC* pPartyMessage)
{
	GLMSG::SNET_PARTY_MBR_MOVEMAP* pNetMsg = (GLMSG::SNET_PARTY_MBR_MOVEMAP*) pPartyMessage;

	GLPartyClient* const pParty = GetParty(pNetMsg->PartyID);
	if ( pParty == NULL )
		return false;

	if( TRUE == pParty->MsgPartyMoveMap(pPartyMessage) )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Party_MembeMoveMap,
			"-dw, -dw, -dw",
			pParty->GetMemberIndex( pNetMsg->dwGaeaID ),
			pNetMsg->mapID.getBaseMapID().dwID,
			pNetMsg->mapID.getGaeaMapID().dwID );
		return true;
	}

	return false;
}

const bool GLPartyManagerClient::MsgPartyMbrDetail(NET_MSG_GENERIC* pPartyMessage)
{
	GLMSG::SNET_PARTY_MBR_DETAIL* pNetMsg = (GLMSG::SNET_PARTY_MBR_DETAIL*)pPartyMessage;

	GLPartyClient* const pParty = GetParty(pNetMsg->PartyID);
	if ( pParty == NULL )
		return false;	

	return pParty->MsgPartyMbrDetail(pPartyMessage);
}
const bool GLPartyManagerClient::MsgPartyMbrDetailParty(NET_MSG_GENERIC* pPartyMessage)
{
	GLMSG::SNET_PARTY_MBR_DETAIL_PARTY* pNetMsg = (GLMSG::SNET_PARTY_MBR_DETAIL_PARTY*)pPartyMessage;

	GLPartyClient* const pParty = GetParty(pNetMsg->PartyID);
	if ( pParty == NULL )
		return false;
	
	if ( pParty->MsgPartyMbrDetailParty(pPartyMessage) == FALSE && m_PartyID.isValidParty() )
		RequestRefreshParty(m_PartyID, EMPARTY_REFRESH_REQ_PARTY_INFORMATION);

	return TRUE;
}

const bool GLPartyManagerClient::MsgPartyMbrLevelUp(NET_MSG_GENERIC* pPartyMessage)
{
	GLMSG::SNET_PARTY_MBR_LEVEL_UP* pNetMsg = (GLMSG::SNET_PARTY_MBR_LEVEL_UP*)pPartyMessage;

	GLPartyClient* const pParty = GetParty(pNetMsg->dwPartyIndex);
	if ( pParty == NULL )
		return false;

	return pParty->MsgPartyMbrLevelUp(pPartyMessage);
}


const bool GLPartyManagerClient::MsgPartyLureFb ( NET_MSG_GENERIC* pPartyMessage )
{
	GLMSG::SNET_PARTY_LURE_FB* pNetMsg = 
		(GLMSG::SNET_PARTY_LURE_FB*)pPartyMessage;

	std::string strNAME;

	std::tr1::shared_ptr<GLCharClient> pChar = m_pGaeaClient->GetChar(pNetMsg->dwGaeaID);
	if ( pChar )
		strNAME = pChar->GetName();

	CString strText;
	switch ( pNetMsg->emAnswer )
	{
	case EMPARTY_LURE_CREATE_FAIL :
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LURE_CREATE_FAIL"));
		break;
	case EMPARTY_LURE_REFUSE:		
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::DISABLE,
			sc::string::format(ID2GAMEINTEXT("PARTY_LURE_REFUSE"), strNAME.c_str()));
		break;

	case EMPARTY_LURE_FULL:		
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::DISABLE,
			sc::string::format( ID2GAMEINTEXT("PARTY_LURE_FULL"), strNAME.c_str()));
		break;

	case EMPARTY_LURE_OTHER:		
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::DISABLE,
			sc::string::format( ID2GAMEINTEXT("PARTY_LURE_OTHER"), strNAME.c_str()));
		break;

	case EMPARTY_LURE_OTHERSCHOOL:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LURE_OTHERSCHOOL") );
		break;

	case EMPARTY_LURE_CLUBBATTLE:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LURE_CLUBBATTLE") );
		break;

	case EMPARTY_LURE_CTF:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LURE_CTF") );
		break;
	};

	return TRUE;
}

const bool GLPartyManagerClient::MsgPartyLureFbWithCharName( NET_MSG_GENERIC* pPartyMessage )
{
	GLMSG::SNET_PARTY_LURE_FB_WITH_CHARNAME* pNetMsg = 
		(GLMSG::SNET_PARTY_LURE_FB_WITH_CHARNAME*)pPartyMessage;

	std::string strNAME = stringformat( "%1%", pNetMsg->szCharName );

	switch ( pNetMsg->emAnswer )
	{
	case EMPARTY_LURE_REFUSE:		
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::DISABLE,
			sc::string::format(ID2GAMEINTEXT("PARTY_LURE_REFUSE"), strNAME.c_str()));
		break;

	case EMPARTY_LURE_FULL:		
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::DISABLE,
			sc::string::format( ID2GAMEINTEXT("PARTY_LURE_FULL"), strNAME.c_str()));
		break;

	case EMPARTY_LURE_OTHER:		
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::DISABLE,
			sc::string::format( ID2GAMEINTEXT("PARTY_LURE_OTHER"), strNAME.c_str()));
		break;

	case EMPARTY_LURE_OTHERSCHOOL:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LURE_OTHERSCHOOL") );
		break;

	case EMPARTY_LURE_CLUBBATTLE:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LURE_CLUBBATTLE") );
		break;

	case EMPARTY_LURE_CTF:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LURE_CTF") );
		break;
	};

	return TRUE;
}

const bool GLPartyManagerClient::MsgPartyLinkJoinFB( NET_MSG_GENERIC* pPartyMessage )
{
	GLMSG::SNET_PARTY_LINK_JOIN_FB* pNetMsg = (GLMSG::SNET_PARTY_LINK_JOIN_FB*)pPartyMessage;

	switch ( pNetMsg->emFB )
	{
	case EMPARTY_LINK_JOIN_FB_OK:	//	�̷� �޼����� �ȿ´�.
		break;
	case EMPARTY_LINK_JOIN_FB_FAIL:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_JOIN_FB_FAIL") );
		break;
	case EMPARTY_LINK_JOIN_FB_FULL:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_JOIN_FB_FULL") );
		break;
	case EMPARTY_LINK_JOIN_FB_NOCHAR:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_JOIN_FB_NOCHAR") );
		break;
	case EMPARTY_LINK_JOIN_FB_REJOIN:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_JOIN_FB_REJOIN") );
		break;
	case EMPARTY_LINK_JOIN_FB_NOMASTER:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_JOIN_FB_NOMASTER") );
		break;
	case EMPARTY_LINK_JOIN_FB_OTHERSCHOOL:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_JOIN_FB_OTHERSCHOOL") );
		break;
	case EMPARTY_LINK_JOIN_FB_CLUBBATTLE:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_JOIN_FB_CLUBBATTLE") );
		break;
	case EMPARTY_LINK_JOIN_FB_CHANNEL:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_JOIN_FB_CHANNEL") );
		break;
	}

	return TRUE;
}

const bool GLPartyManagerClient::MsgPartyLinkJoinANS( NET_MSG_GENERIC* pPartyMessage )
{
	GLMSG::SNET_PARTY_LINK_JOIN_ANS *pNetMsg = ( GLMSG::SNET_PARTY_LINK_JOIN_ANS * ) pPartyMessage;

	//GLWidgetScript::GetInstance().LuaCallFunc< void >(
	//	NSWIDGET_SCRIPT::g_strFunc_PartyRecruit_RequestRecv,
	//	"-dw, -n, -s, -dw, -dw, -dw",
	//	pNetMsg->dwGaeaID, (int) pNetMsg->emClass, pNetMsg->szName,
	//	pNetMsg->dwHashKey, pNetMsg->dwTimeKey, m_pGaeaClient->GetCharacter()->GetGaeaID() );

	////GLChatMan::GetInstance().MsgChatLinkMessage( pNetMsg );

	GLMSG::NET_CHAT_LINK_FB	NetChatFB;

	//	��ũ ������ �����Ѵ�.
	{
		CString strChatMsg, strTemp;

		// ��ũ ������ ã�´�.
		SLINK_TYPE sLinkType1, sLinkType2;
		sLinkType1.wMType = PARTY_LINK;
		sLinkType1.wSType = PARTY_LINK_ACCEPT;

		sLinkType2.wMType = PARTY_LINK;
		sLinkType2.wSType = PARTY_LINK_REFUSE;

		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			strChatMsg += CGameTextMan::GetInstance().GetCommentText("CHARCLASS", CharClassToIndex(pNetMsg->emClass)).GetString();
		else
			strChatMsg += COMMENT::CHARCLASS[ CharClassToIndex( pNetMsg->emClass )].c_str();		

		//strTemp.Format( "%s%d%d%s", strLINK_LEFT_SIMBOL, sLinkType1.wMType, sLinkType1.wSType, strLINK_RIGHT_SIMBOL );			
		//strChatMsg += strTemp;

		//strTemp.Format( "%s%d%d%s", strLINK_LEFT_SIMBOL, sLinkType2.wMType, sLinkType2.wSType, strLINK_RIGHT_SIMBOL );	
		//strChatMsg += strTemp;

		int nStart = sc::string::ansiToUnicode(strChatMsg.GetString()).length();
		strTemp.Format( "%s%s%s", strLINK_LEFT_SIMBOL, ID2GAMEWORD( "CHAT_LINK_PARTY_RECRUIT", 2 ), strLINK_RIGHT_SIMBOL );
		strChatMsg += strTemp.GetString();
		int nEnd = nStart + sc::string::ansiToUnicode(strTemp.GetString()).length();

		NetChatFB.ADD( &pNetMsg->dwGaeaID, sizeof(DWORD) * 3);
		NetChatFB.sLinkDataRecv[0].sLinkType = sLinkType1;
		NetChatFB.sLinkDataRecv[0].sLinkPos.nStart = nStart;
		NetChatFB.sLinkDataRecv[0].sLinkPos.nEnd = nEnd;
		NetChatFB.sLinkDataRecv[0].dwSize = sizeof(DWORD) * 3;	

		nStart = nEnd;
		strTemp.Format( "%s%s%s", strLINK_LEFT_SIMBOL, ID2GAMEWORD( "CHAT_LINK_PARTY_RECRUIT", 3 ), strLINK_RIGHT_SIMBOL );
		strChatMsg += strTemp.GetString();
		nEnd = nStart + sc::string::ansiToUnicode(strTemp.GetString()).length();

		if( this->m_pGaeaClient->GetServiceProvider() == SP_GS )
			strChatMsg = sc::string::ansiToUnicode(strChatMsg.GetString()).c_str();

		NetChatFB.ADD( &pNetMsg->dwGaeaID, sizeof(DWORD));
		NetChatFB.sLinkDataRecv[1].sLinkType = sLinkType2;
		NetChatFB.sLinkDataRecv[1].sLinkPos.nStart = nStart;
		NetChatFB.sLinkDataRecv[1].sLinkPos.nEnd = nEnd;
		NetChatFB.sLinkDataRecv[1].dwSize = sizeof(DWORD);

		NetChatFB.emType = CHAT_TYPE_PRIVATE_PARTY_RECRUIT;
		NetChatFB.SetName(pNetMsg->szName);
		NetChatFB.SetChatMsg(strChatMsg.GetString());

		//m_pInterface->DisplayChatLinkMessage(&NetChatFB);
		GLChatMan::GetInstance().MsgChatLinkMessage( &NetChatFB );
	}

	return TRUE;
}

const bool GLPartyManagerClient::MsgPartyLinkLureFB( NET_MSG_GENERIC* pPartyMessage )
{

	GLMSG::SNET_PARTY_LINK_LURE_FB *pNetMsg = ( GLMSG::SNET_PARTY_LINK_LURE_FB * ) pPartyMessage;

	switch ( pNetMsg->emFB )
	{
	case EMPARTY_LINK_LURE_FB_OK:	//	�̷� �޼����� �ȿ´�.
		break;
	case EMPARTY_LINK_LURE_FB_FAIL:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_LURE_FAIL") );
		break;
	case EMPARTY_LINK_LURE_FB_FULL:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_LURE_FB_FULL") );
		break;
	case EMPARTY_LINK_LURE_FB_NOCHAR:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_LURE_FB_NOCHAR") );
		break;
	case EMPARTY_LINK_LURE_FB_REJOIN:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_LURE_FB_REJOIN") );
		break;
	case EMPARTY_LINK_LURE_FB_NOMASTER:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_LURE_FB_NOMASTER") );
		break;
	case EMPARTY_LINK_LURE_FB_OTHERSCHOOL:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_LURE_FB_OTHERSCHOOL") );
		break;
	case EMPARTY_LINK_LURE_FB_CLUBBATTLE:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_LURE_FB_CLUBBATTLE") );
		break;
	case EMPARTY_LINK_LURE_FB_CHANNEL:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_LURE_FB_CHANNEL") );
		break;
	}

	return TRUE;
}

const bool GLPartyManagerClient::MsgPartyLinkRefuseFB( NET_MSG_GENERIC* pPartyMessage )
{

	GLMSG::SNET_PARTY_LINK_REFUSE_FB *pNetMsg = ( GLMSG::SNET_PARTY_LINK_REFUSE_FB * ) pPartyMessage;

	GLMSG::NET_CHAT_LINK_FB	NetChatFB;

	CString strChatMsg = ID2GAMEWORD( "CHAT_LINK_PARTY_RECRUIT",4 );

	NetChatFB.SetData(CHAT_TYPE_PRIVATE_PARTY_RECRUIT, pNetMsg->szName, strChatMsg.GetString());

	GLChatMan::GetInstance().MsgChatLinkMessage( &NetChatFB );

	return TRUE;
}

const bool GLPartyManagerClient::MsgPartyMbrUpdate(NET_MSG_GENERIC* pPartyMessage)
{
	GLMSG::SNET_PARTY_MBR_UPDATE *pNetMsg = (GLMSG::SNET_PARTY_MBR_UPDATE *) pPartyMessage;

	GLPartyClient* const pParty = GetParty(pNetMsg->PartyID);
	if ( pParty == NULL )
		return false;	

	if( TRUE == pParty->MsgPartyMbrUpdate(pPartyMessage) )
	{
		const GLPartySlotClient* const pMember = pParty->GetMemberByGaeaID( pNetMsg->dwGaeaID );

		int nPosX = 0, nPosY = 0;
		m_pGaeaClient->ConvertWorldToMap( pMember->m_sMapID.getBaseMapID(), pMember->m_vPosition, nPosX, nPosY );

		LuaTable tbPos( GLWidgetScript::GetInstance().GetLuaState() );
		tbPos.set( "x", pMember->m_vPosition.x );
		tbPos.set( "y", pMember->m_vPosition.y );
		tbPos.set( "z", pMember->m_vPosition.z );

		GLWidgetCallFuncValue args[ 7 ];
		args[ 0 ].SetDWORD( pParty->GetMemberIndex( pNetMsg->dwGaeaID ) );
		args[ 1 ].SetInt( nPosX );
		args[ 2 ].SetInt( nPosY );
		args[ 3 ].SetDWORD( pNetMsg->sHP.dwMax );
		args[ 4 ].SetDWORD( pNetMsg->sHP.dwNow );
		args[ 5 ].SetInt( pNetMsg->wLevel );
		args[ 6 ].SetTable( &tbPos );

		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_Party_UpdateMember, args, 7 );

		return true;
	}
	
	return false;
}

const bool GLPartyManagerClient::MsgPartyMbrItemPickUp(NET_MSG_GENERIC* pPartyMessage)
{
	GLMSG::SNETPC_PARTY_MBR_PICKUP_BRD *pNetMsg = (GLMSG::SNETPC_PARTY_MBR_PICKUP_BRD *) pPartyMessage;

	GLPartyClient* const pParty = GetParty(pNetMsg->PartyID);
	if ( pParty == NULL )
		return false;	

	return pParty->MsgPartyMbrItemPickUp(pPartyMessage);	
}

const bool GLPartyManagerClient::MsgPartyMbrItemPickUpLink(NET_MSG_GENERIC* pPartyMessage)
{
	GLMSG::SNETPC_PARTY_MBR_PICKUP_LINK_BRD *pNetMsg = (GLMSG::SNETPC_PARTY_MBR_PICKUP_LINK_BRD *) pPartyMessage;
	


	GLPartyClient* const pParty = GetParty(pNetMsg->PartyID);
	if ( pParty == NULL )
		return false;	

	return pParty->MsgPartyMbrItemPickUpLink(pPartyMessage);
}

const bool GLPartyManagerClient::MsgPartyMbrRename(NET_MSG_GENERIC* pPartyMessage)
{
	GLMSG::SNET_PARTY_MEMBER_RENAME* pNetMsg = ( GLMSG::SNET_PARTY_MEMBER_RENAME*) pPartyMessage;

	GLPartyClient* const pParty = GetParty(pNetMsg->PartyID);
	if ( pParty == NULL )
		return false;

	if( TRUE == pParty->MsgPartyMbrRename(pPartyMessage) )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Party_MemberRename,
			"-n, -s", pParty->GetMemberIndex( pNetMsg->dwGaeaID ), pNetMsg->szName );

		return true;
	}

	return false;
}

const bool GLPartyManagerClient::MsgPartyChangeOption(NET_MSG_GENERIC* pPartyMessage)
{
	GLMSG::SNET_PARTY_CHANGE_OPTION* pNetMsg = (GLMSG::SNET_PARTY_CHANGE_OPTION*)pPartyMessage;

	const GLPartyID& PartyID = pNetMsg->PartyID;	

	if ( PartyID.isValidExpedition() == true )
	{
		if ( m_pExpedition->isMaster(pNetMsg->dwMasterGaeaID) == false )
			return false;

		for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
		{
			const GLPartyID& ExpeditionPartyID = m_pExpedition->GetPartyID(_i - 1);
			GLPartyClient* const pParty = GetParty(_i - 1);
			if ( pParty->GetPartyID() != ExpeditionPartyID )
				return false;			

			pParty->SetOption(pNetMsg->PartyOption);
		}		
	}
	else if ( PartyID.isValidParty() == true )
	{
		GLPartyClient* const pParty = GetParty(0);	
		if ( pParty->GetPartyID() != PartyID )
			return false;

		// ������ �й� �ɼ��� �ִٸ�, ���� ������� �������� ���¸� �����ؾ��Ѵ�;
		GLPartyTenderManagerClient* pTenderManager = m_pGaeaClient->GetPartyTenderManager();
		if (pTenderManager)
			pTenderManager->Reset();

		pParty->SetOption(pNetMsg->PartyOption);
	}
	else
		return false;

	const DWORD dwItemOption(DWORD(pNetMsg->PartyOption.GetItemOption()));
	const DWORD dwMoneyOption(pNetMsg->PartyOption.GetMoneyOption());
	const DWORD dwItemFilter(pNetMsg->PartyOption.GetItemFilter());

	//��Ƽ ���� ����
	RANPARAM::dwPARTY_GET_ITEM = pNetMsg->PartyOption.GetItemOption();
	RANPARAM::dwPARTY_GET_MONEY = pNetMsg->PartyOption.GetMoneyOption();
	RANPARAM::dwPARTY_GET_ITEM_Filter = pNetMsg->PartyOption.GetItemFilter();


	CString strItemOption, strItemFilter, strMoneyOption;
	switch ( dwItemOption )
	{
	case EMPARTY_ITEM_LEADER_SHARE:
	case EMPARTY_ITEM_DICE:
		{
			strItemOption.Format("%s", ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION", dwItemOption));
			strItemFilter.Format("%s", ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION_FILTER", dwItemFilter));
			strItemOption.Format("%s(%s)", 
				ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION", dwItemOption),
				ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION_FILTER", dwItemFilter));
		}
		break;
	default:
		{
			strItemOption.Format("%s", ID2GAMEWORD("PARTY_MODAL_ITEM_OPTION", dwItemOption));
		}
	}

	strMoneyOption.Format("%s", ID2GAMEWORD("PARTY_MODAL_MONEY_OPTION", dwMoneyOption));

	// ���� �޼��� ���;
	m_pGaeaClient->PrintMsgText(
		NS_UITEXTCOLOR::DIALOGUE_MOUSEOVER,
		sc::string::format(ID2GAMEINTEXT("PARTY_OPTION_CHANGE"), strItemOption.GetString(), strMoneyOption.GetString()));	

	// ��Ƽ����â ���� ���� ����;
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Party_ChangeOption,
		"-n, -n, -n", 
		(int) dwItemOption,
		(int) dwMoneyOption,
		(int) pNetMsg->PartyOption.GetItemFilter() );

	return true;
}

const bool GLPartyManagerClient::MsgExpeditionFNew(NET_MSG_GENERIC* pPartyMessage)
{
	GLMSG::SNET_EXPEDITION_NEW_FB* pNetMsg = (GLMSG::SNET_EXPEDITION_NEW_FB*)pPartyMessage;	

	if ( m_PartyID.isValidExpedition() == true )
		return false;

	const DWORD dwMyPartyIndex = pNetMsg->dwMyPartyIndex;
	if ( dwMyPartyIndex > (MAX_PARTYGROUP - 1) )
		return false;

	m_PartyID = pNetMsg->PartyIDs[dwMyPartyIndex];
	for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
		m_ptrPartyClient[_i - 1]->SetPartyID(pNetMsg->PartyIDs[_i - 1]);

	m_pExpedition->SetPartyID(pNetMsg->PartyIDs);
	m_pExpedition->SetMasterGaeaID(pNetMsg->dwMasterGaeaID, MASTER_PARTY);

    //�̴Ͽ�����â ����
    //m_pInterface->UiShowGroupFocus( MINI_EXPEDITION_WINDOW );
	return true;
}

const bool GLPartyManagerClient::MsgExpeditionCheckReadyRequest(NET_MSG_GENERIC* pPartyMessage)
{	
	if ( m_PartyID.isValidExpedition() == false )
		return false;

	GLMSG::SNET_EXPEDITION_CHECK_READY_REQUEST_CA* pNetMsg = (GLMSG::SNET_EXPEDITION_CHECK_READY_REQUEST_CA*)pPartyMessage;

	if ( pNetMsg->dwMasterGaeaID != m_pExpedition->GetMasterGaeaID() )
		return false;	

	//m_pInterface->ToggleWindowExpeditionCheckReady(true);

	return true;
}

const bool GLPartyManagerClient::MsgExpeditionCheckReadyAnswer(NET_MSG_GENERIC* pPartyMessage)
{
	if ( m_PartyID.isValidExpedition() == false )
		return false;

	if ( isMasterAuthority() == false )
		return false;

	GLMSG::SNET_EXPEDITION_CHECK_READY_ANSWER_CA* pNetMsg = 
		(GLMSG::SNET_EXPEDITION_CHECK_READY_ANSWER_CA*)pPartyMessage;

	if ( m_pExpedition->GetMasterGaeaID() != pNetMsg->dwMasterGaeaID )
		return false;

	if ( m_pExpedition->GetPartyID(pNetMsg->dwPartySlot) != pNetMsg->PartyID )
		return false;

	GLPartyClient* const pParty = m_ptrPartyClient[pNetMsg->dwPartySlot].get();

	const DWORD dwSlotIndex = pParty->GetMemberIndex(pNetMsg->dwGaeaID);
	GLPartySlotClient* const pMember = pParty->GetMember(dwSlotIndex);
	if ( pMember == NULL )
		return false;

	if ( pNetMsg->bAnswer )
		pMember->SetMemberState(EMPARTY_MEMBER_STATE_READY);
	else
		pMember->ResetMemberState(EMPARTY_MEMBER_STATE_READY);

	return true;
}
const bool GLPartyManagerClient::MsgExpeditionSwapSlot(NET_MSG_GENERIC* pPartyMessage)
{	
	GLMSG::SNET_EXPEDITION_SWAP_SLOT* pNetMsg = (GLMSG::SNET_EXPEDITION_SWAP_SLOT*)pPartyMessage;		

	const GLSwapSlotInformation& SwapSlotFrom = pNetMsg->SwapSlotFrom;
	const GLSwapSlotInformation& SwapSlotTo = pNetMsg->SwapSlotTo;

	const GLPartyID& PartyID = pNetMsg->PartyID;

	const DWORD dwFromGaeaID = SwapSlotFrom.dwGaeaID;
	const DWORD dwToGaeaID = SwapSlotTo.dwGaeaID;
	if ( dwFromGaeaID == dwToGaeaID )
		return true;

	if ( dwFromGaeaID == GAEAID_NULL && dwToGaeaID == GAEAID_NULL )
		return false; // �� �� GAEAID_NULL�̸� �ȵ�;

	if ( (SwapSlotFrom.nPartyIndex >= MAX_PARTYGROUP) || (SwapSlotTo.nPartyIndex >= MAX_PARTYGROUP) )
		return false;

	// field server�� ��� GaeaID�� NULL�� �ƴ϶� �ϴ��� GLChar�� ���� ���� ���� �� ����;

	DWORD dwMasterGaeaID = GAEAID_NULL;
	if ( PartyID.isValidExpedition() == true )
	{
		const GLPartyID& PartyIDMaster = m_pExpedition->GetPartyID(MASTER_PARTY);
		const GLPartyID& PartyIDFrom = m_pExpedition->GetPartyID(SwapSlotFrom.nPartyIndex);
		const GLPartyID& PartyIDTo = m_pExpedition->GetPartyID(SwapSlotTo.nPartyIndex);
		GLPartyClient* const pPartyMaster = GetParty(PartyIDMaster);
		GLPartyClient* const pPartyFrom = GetParty(PartyIDFrom);
		GLPartyClient* const pPartyTo = GetParty(PartyIDTo);
		if ( (pPartyFrom == NULL) || (pPartyTo == NULL) || (pPartyMaster == NULL) )
			return false;

		GLPartyClient::PARTYMAP_ITER iterFrom = find_if(pPartyFrom->m_mapParty.begin(), pPartyFrom->m_mapParty.end(), GLPartyClient::FindMemberKey(dwFromGaeaID));
		GLPartyClient::PARTYMAP_ITER iterTo = find_if(pPartyTo->m_mapParty.begin(), pPartyTo->m_mapParty.end(), GLPartyClient::FindMemberKey(dwToGaeaID));

		if ( dwFromGaeaID != GAEAID_NULL && iterFrom == pPartyFrom->m_mapParty.end() )
			return false;
		if ( dwToGaeaID != GAEAID_NULL && iterTo == pPartyTo->m_mapParty.end() )
			return false;

		if ( dwToGaeaID == GAEAID_NULL )
		{ // toGaeaID�� NULL�̸� fromGaeaID�� toParty�� �̵�;
			if ( pPartyTo->m_mapParty.size() < MAXPARTY )
			{
				const GLPartyClient::PARTYPAIRCLIENT copyFrom = *iterFrom;				
				pPartyFrom->m_mapParty.erase(iterFrom);
				pPartyTo->m_mapParty.push_back(copyFrom);
			}
			else
				return false;
		}
		else if ( dwFromGaeaID == GAEAID_NULL )
		{ // fromGaeaID�� NULL�̸� toGaeaID�� fromParty�� �̵�;
			if ( pPartyFrom->m_mapParty.size() < MAXPARTY )
			{
				const GLPartyClient::PARTYPAIRCLIENT copyTo = *iterTo;
				pPartyTo->m_mapParty.erase(iterTo);
				pPartyFrom->m_mapParty.push_back(copyTo);				
			}
			else
				return false;			
		}
		else
		{ // �� �� NULL�� �ƴϸ� �ڸ� ��ü;
			const GLPartyClient::PARTYPAIRCLIENT copyFrom = *iterFrom;
			*iterFrom = *iterTo;
			*iterTo = copyFrom;
		}

		const DWORD dwMyGaeaID = m_pGaeaClient->GETMYGAEAID();
		if ( SwapSlotFrom.dwGaeaID == dwMyGaeaID )
		{
			m_PartyID = pPartyTo->GetPartyID();
			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_QuickMenu_SetQBoxLock,
				"-b", pPartyTo->m_bQBoxEnable );
		}
		else if ( SwapSlotTo.dwGaeaID == dwMyGaeaID )
		{
			m_PartyID = pPartyFrom->GetPartyID();
			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_QuickMenu_SetQBoxLock,
				"-b", pPartyTo->m_bQBoxEnable );
		}

		_refreshExpeditionMaster();
	}
	else if ( PartyID.isValidParty() == true )
	{
		GLPartyClient* const pParty = GetParty(PartyID);
		if ( pParty == NULL )
			return false;

		GLPartyClient::PARTYMAP_ITER iterEnd = pParty->m_mapParty.end();
		GLPartyClient::PARTYMAP_ITER iterFrom = find_if(pParty->m_mapParty.begin(), iterEnd, GLPartyClient::FindMemberKey(dwFromGaeaID));
		GLPartyClient::PARTYMAP_ITER iterTo = find_if(pParty->m_mapParty.begin(), iterEnd, GLPartyClient::FindMemberKey(dwToGaeaID));

		if ( dwFromGaeaID != GAEAID_NULL && iterFrom == iterEnd )
			return false;
		if ( dwToGaeaID != GAEAID_NULL && iterTo == iterEnd )
			return false;

		if ( dwToGaeaID == GAEAID_NULL )
		{ // toGaeaID�� NULL�̸� fromGaeaID�� toParty�� �̵�;
			const GLPartyClient::PARTYPAIRCLIENT copyFrom = *iterFrom;
			pParty->m_mapParty.erase(iterFrom);
			pParty->m_mapParty.push_back(copyFrom);
		}
		else if ( dwFromGaeaID == GAEAID_NULL )
		{ // fromGaeaID�� NULL�̸� toGaeaID�� fromParty�� �̵�;
			const GLPartyClient::PARTYPAIRCLIENT copyTo = *iterTo;
			pParty->m_mapParty.erase(iterTo);
			pParty->m_mapParty.push_back(copyTo);
		}
		else
		{ // �� �� NULL�� �ƴϸ� �ڸ� ��ü;
			const GLPartyClient::PARTYPAIRCLIENT copyFrom = *iterFrom;
			*iterFrom = *iterTo;
			*iterTo = copyFrom;
		}
	}		

	return true;
}

const bool GLPartyManagerClient::MsgPartyError(NET_MSG_GENERIC* pPartyMessage)
{
	GLMSG::SNET_PARTY_ERROR_TO_CLIENT* pNetMsg = (GLMSG::SNET_PARTY_ERROR_TO_CLIENT*)pPartyMessage;
	switch ( pNetMsg->emPartyErrorCode )
	{
	case EMPARTY_ERROR_NOPARTY:
	case EMPARTY_ERROR_NOEXPEDITION:
	case EMPARTY_ERROR_NOMEMBER:
		Reset();
		break;
	case EMPARTY_ERROR_LURE_REJOIN:
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_JOIN_FB_REJOIN") );
		break;
	case EMPARTY_ERROR_LINK_REJOIN:
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, sc::string::format(ID2GAMEINTEXT("PARTY_LURE_OTHER"), pNetMsg->szCharName) );
		break;
	case EMPARTY_ERROR_OTHERSCHOOL:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_JOIN_FB_OTHERSCHOOL") );
		break;
	case EMPARTY_ERROR_NOCHAR:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_JOIN_FB_NOCHAR") );
		break;
	case EMPARTY_ERROR_NOMASTER:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_JOIN_FB_NOMASTER") );
		break;
	case EMPARTY_ERROR_LINK_FULLPARTY:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_JOIN_FB_FULL") );
		break;
	case EMPARTY_ERROR_LURE_FULLPARTY:		
		m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, sc::string::format( ID2GAMEINTEXT("PARTY_LURE_FULL"), pNetMsg->szCharName));
		break;
	case EMPARTY_ERROR_LINK_CLUBBATTLE:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_JOIN_FB_CLUBBATTLE") );
		break;
	case EMPARTY_ERROR_LURE_CLUBBATTLE:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_JOIN_FB_CLUBBATTLE") );
		break;
	case EMPARTY_ERROR_LINK_CTF:
	case EMPARTY_ERROR_LURE_CTF:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LURE_CTF") );
		break;
	case EMPARTY_ERROR_PRISON :
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("PARTY_INVITE_PRISION_LIMIT") );
		break;
	}

	return true;
}

const bool GLPartyManagerClient::MsgExpeditionError( NET_MSG_GENERIC* pPartyMessage )
{
	GLMSG::SNET_EXPEDITION_ERROR* pNetMsg = (GLMSG::SNET_EXPEDITION_ERROR*) pPartyMessage;

	switch( pNetMsg->emError )
	{
	case EMEXPEDITION_ERROR_NONE :
		{
		}
		break;

	case EMEXPEDITION_ERROR_NEW_ALREADY :
		{
		}
		break;

	case EMEXPEDITION_ERROR_NEW_PARTYMAP :
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT( "EMEXPEDITION_ERROR_NEW_PARTYMAP" ) );
		}
		break;
	case EMEXPEDITION_ERROR_INVALID_LAND:
		{
		}
		break;
	}

	return true;
}