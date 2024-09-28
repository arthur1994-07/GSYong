#include "pch.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogicClient/Tournament/GLTournamentClient.h"
#include "../../Ranlogicclient/Party/GLPartyManagerClient.h"
#include "../../Ranlogicclient/GLGaeaClient.h"

#include "../Stage/DxGlobalStage.h"
#include "../Land/GLLandManClient.h"
#include "../GLGaeaClient.h"
#include "./GLFriendClient.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "../Notify/NotifyClientFactory.h"
#include "../Notify/NotifyClientMan.h"

#include "../../SigmaCore/String/MinGuid.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


GLFriendClient::GLFriendClient(GLGaeaClient* pGaeaClient)
	: m_pGaeaClient(pGaeaClient)
    , m_bReqList(false)
{
}

GLFriendClient::~GLFriendClient(void)
{
}

void GLFriendClient::Clear ()
{
    friendlogic::Friend::Clear();
	m_bReqList = false;
}

bool GLFriendClient::IsFriend( const char* szName )
{
	return NULL == GetData( szName ) ? false : true;
}

bool GLFriendClient::IsOnline( const char* szName )
{
	std::tr1::shared_ptr<SFRIEND> sFriend = GetData( szName );
	if( !sFriend )
		return false;

	return ( sFriend->m_Online && sFriend->IsVALID() );
}

bool GLFriendClient::IsBlock( const char* szName )
{
	std::tr1::shared_ptr<SFRIEND> sFriend = GetData( szName );
	if( !sFriend )
		return false;

	return sFriend->IsBLOCK();
}

LuaTable GLFriendClient::GetFriendList()
{
	LuaTable tbFriendList( GLWidgetScript::GetInstance().GetLuaState() );

	int nFriendCount = 1;

	friendlogic::FRIENDMAP_CITER iter = m_mapFriend.begin();
	for( ; iter != m_mapFriend.end(); ++iter )
	{
		std::tr1::shared_ptr< SFRIEND > pFriend = iter->second;
		if( true == pFriend->IsBLOCK() )
			continue;

		LuaTable tbFriend( GLWidgetScript::GetInstance().GetLuaState() );
		tbFriend.set( 1, pFriend->m_ChaName.c_str() );
		tbFriend.set( 2, pFriend->m_Online );
		tbFriend.set( 3, pFriend->GetChaDbNum() );

		tbFriendList.set( nFriendCount++, tbFriend );
	}

	return tbFriendList;
}

LuaTable GLFriendClient::GetBlockList()
{
	LuaTable tbBlockList( GLWidgetScript::GetInstance().GetLuaState() );

	int nBlockCount = 1;
	for (friendlogic::FRIENDMAP_CITER iter=m_mapFriend.begin(); iter != m_mapFriend.end(); ++iter)
	{
		std::tr1::shared_ptr<SFRIEND> sFriend = iter->second;
		if ( sFriend->IsBLOCK() == false )
			continue;

		LuaTable tbBlock( GLWidgetScript::GetInstance().GetLuaState() );
		tbBlock.set( 1, sFriend->m_ChaName.c_str() );

		tbBlockList.set( nBlockCount, tbBlock );
		++nBlockCount;
	}

	return tbBlockList;
}

LuaTable GLFriendClient::GetGroupList()
{
	/*
	LuaTable tbGroupList( GLWidgetScript::GetInstance().GetLuaState() );
	std::vector<FRIEND_GROUP> groupInfo;
	int nGroup = 1;
	GetGroup( groupInfo );
	tbGroupList.set( nGroup, ID2GAMEWORD("RNFRIEND_FRIENDTAB_SLOT_NORMAL", 0) );

	std::vector<FRIEND_GROUP>::iterator iter_group = groupInfo.begin();
	std::vector<FRIEND_GROUP>::iterator iter_group_end = groupInfo.end();

	++nGroup;
	for( ;iter_group != iter_group_end; ++iter_group )
	{
		tbGroupList.set( nGroup, iter_group->m_GroupName );
		++nGroup;
	}
	return tbGroupList;
	*/
	LuaTable tbGroupList( GLWidgetScript::GetInstance().GetLuaState() );

	std::vector<FRIEND_GROUP> groupInfo;
	int nGroup = 1;
	GetGroup( groupInfo );

	// '일반' 그룹 세팅;
	LuaTable tbNGroupInfo( GLWidgetScript::GetInstance().GetLuaState() );
	tbNGroupInfo.set( 1, 0 );
	tbNGroupInfo.set( 2, ID2GAMEWORD("RNFRIEND_FRIENDTAB_SLOT_NORMAL", 0) );
	tbGroupList.set(nGroup++, tbNGroupInfo);

	// 추가 그룹 세팅;
	std::vector<FRIEND_GROUP>::iterator iter_group = groupInfo.begin();
	std::vector<FRIEND_GROUP>::iterator iter_group_end = groupInfo.end();
	
	for( ;iter_group != iter_group_end; ++iter_group )
	{
		LuaTable tbGroupInfo(GLWidgetScript::GetInstance().GetLuaState());
		tbGroupInfo.set( 1, iter_group->m_GroupNum);
		tbGroupInfo.set( 2, iter_group->m_GroupName);

		tbGroupList.set(nGroup++, tbGroupInfo);
	}
	return tbGroupList;
}

LuaTable GLFriendClient::GetFriendListInGroup( WORD wGroupNum )
{
	LuaTable tbFriendGroupList( GLWidgetScript::GetInstance().GetLuaState() );

	friendlogic::FRIENDMAP_ITER iter_friend;
	friendlogic::FRIENDMAP_ITER iter_friend_end = m_mapFriend.end();

	int nFriend = 1;
	// 온라인
	for( iter_friend = m_mapFriend.begin();iter_friend != iter_friend_end; ++iter_friend )
	{
		LuaTable tbFriend( GLWidgetScript::GetInstance().GetLuaState() );
		const std::tr1::shared_ptr<SFRIEND> spFriend = (*iter_friend).second;
		WORD wNum = spFriend->GetGroup();

		if( wGroupNum != wNum )
		 continue;

		if( !spFriend->m_Online || !spFriend->IsVALID() || spFriend->IsBLOCK() )
		 continue;

		m_pGaeaClient->GetMapName( spFriend->m_ChaMap.getBaseMapID() );

		CString strCoord;
		strCoord.Format("%03d/%03d", spFriend->m_PosX, spFriend->m_PosY );

		CString strChannel;
		strChannel.Format(" [%d]", spFriend->m_Channel);

		strCoord = strCoord + strChannel;

		tbFriend.set( 1, GetGLCCToGLCI( (EMCHARCLASS)spFriend->m_ChaClass ) );
		tbFriend.set( 2, spFriend->m_ChaLevel );
		tbFriend.set( 3, spFriend->GetName() );
		tbFriend.set( 4, m_pGaeaClient->GetMapName( spFriend->m_ChaMap.getBaseMapID() ) );
		tbFriend.set( 5, strCoord.GetString() );
		tbFriend.set( 6, true );

		tbFriendGroupList.set( nFriend, tbFriend );
		++nFriend;
	}

	// 오프라인
	for( iter_friend = m_mapFriend.begin(); iter_friend != iter_friend_end; ++iter_friend )
	{
		LuaTable tbFriend( GLWidgetScript::GetInstance().GetLuaState() );
		const std::tr1::shared_ptr<SFRIEND> spFriend = (*iter_friend).second;
		WORD wNum = spFriend->GetGroup();

		if( wGroupNum != wNum )
			continue;

		if( spFriend->m_Online || spFriend->IsBLOCK() )
			continue;

		tbFriend.set( 1, GetGLCCToGLCI( (EMCHARCLASS)spFriend->m_ChaClass ) );
		tbFriend.set( 2, spFriend->m_ChaLevel );
		tbFriend.set( 3, spFriend->GetName() );
		tbFriend.set( 4, "" );
		tbFriend.set( 5, "" );
		tbFriend.set( 6, false );

		tbFriendGroupList.set( nFriend, tbFriend );
		++nFriend;
	}

	return tbFriendGroupList;
}

EMCHARINDEX GLFriendClient::GetGLCCToGLCI( EMCHARCLASS eGLCC )
{
	unsigned int eGLCI = 0;
	switch( eGLCC )
	{
	case GLCC_ACTOR_W:
		eGLCI++;
	case GLCC_ACTOR_M:
		eGLCI++;
	case GLCC_ETC_W:
		eGLCI++;
	case GLCC_ETC_M:
		eGLCI++;
	case GLCC_TRICKER_W:
		eGLCI++;
	case GLCC_TRICKER_M:
		eGLCI++;
	case GLCC_ASSASSIN_W:
		eGLCI++;
	case GLCC_ASSASSIN_M:
		eGLCI++;
	case GLCC_SCIENTIST_W:
		eGLCI++;
	case GLCC_SCIENTIST_M:
		eGLCI++;
	case GLCC_SPIRIT_M:
		eGLCI++;
	case GLCC_ARCHER_M:
		eGLCI++;
	case GLCC_ARMS_W:
		eGLCI++;
	case GLCC_FIGHTER_W:
		eGLCI++;
	case GLCC_EXTREME_W:
		eGLCI++;
	case GLCC_EXTREME_M:
		eGLCI++;
	case GLCC_SPIRIT_W:
		eGLCI++;
	case GLCC_ARCHER_W:
		eGLCI++;
	case GLCC_ARMS_M:
		eGLCI++;
	case GLCC_FIGHTER_M:
		break;
	default:
		return GLCI_FIGHTER_M;
	}

	return (EMCHARINDEX)eGLCI;
}

int GLFriendClient::GetMaxFriend()
{
	return GLCONST_CHAR::nMAX_FRIEND_LIST;
}

int GLFriendClient::GetMaxBlock()
{
	return GLCONST_CHAR::nMAX_FRIEND_BLOCK;
}

int GLFriendClient::GetMaxGroup()
{
	return GLCONST_CHAR::nMAX_FRIEND_GROUP;
}

int GLFriendClient::GetFriendCount()
{
	int nFriend = 0;
	for (friendlogic::FRIENDMAP_CITER iter=m_mapFriend.begin(); iter != m_mapFriend.end(); ++iter)
	{
		std::tr1::shared_ptr<SFRIEND> sFriend = iter->second;
		if ( sFriend->IsBLOCK() )
			continue;

		++nFriend;
	}

	return nFriend;
}

void GLFriendClient::ReqGroupAdd(const char* szName)
{
	std::vector<FRIEND_GROUP> vecGroup;
	m_pGaeaClient->FriendGroup( vecGroup );

	if( (unsigned int)GLCONST_CHAR::nMAX_FRIEND_GROUP <= vecGroup.size() + 1 )
		return;

	const std::string szGroupName = szName;
	if( szGroupName.empty() == FALSE )
		m_pGaeaClient->FriendGroupNewCA( szGroupName );
}

void GLFriendClient::ReqGroupRename( WORD GroupNum, const char* NewName )
{
	m_pGaeaClient->FriendRenameGroup( GroupNum, NewName );
}

void GLFriendClient::ReqGroupDelete( WORD GroupNum )
{
	m_pGaeaClient->FriendGroupDelReq( GroupNum );
}

void GLFriendClient::ReqFriendMoveGroup( const char* szName, WORD GroupNum )
{
	std::tr1::shared_ptr<SFRIEND> sFriend = GetData( szName );
	if( !sFriend )
		return;

	m_pGaeaClient->FriendGroupSetFriend( sFriend->m_ChaDbNum, GroupNum );	
}

void GLFriendClient::ReqFriendInviteParty( const char* szName )
{
	std::tr1::shared_ptr<SFRIEND> sFriend = GetData( szName );
	if( !sFriend )
		return;

	GLPartyManagerClient* pPartyManager = m_pGaeaClient->GetPartyManager();
	if( !pPartyManager )
		return;

	pPartyManager->LureByDbNum( sFriend->m_ChaDbNum );
}

void GLFriendClient::ReqMoveToFriend( const char* szName )
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if( NULL == pCharacter )
		return;

	std::tr1::shared_ptr<SFRIEND> sFriend = GetData( szName );
	if( !sFriend )
		return;

	if( sFriend != NULL && sFriend->m_Online  )
	{
		// 친구연결카드가 있는지 먼저 검사한다.
		SINVENITEM* pItem = pCharacter->m_cInventory.FindItem( ITEM_2FRIEND );
		if( NULL == pItem )
		{
			m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT( "EM2FRIEND_FB_NO_ITEM" ) );
			return;
		}

		// 친구에게 이동 관련 메시지UI 필요;
		pCharacter->Req2Friend( szName );
	}
	else
	{
		// 친구가 없거나 친구가 오프라인 상태인 경우
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::DISABLE,
			sc::string::format(ID2GAMEINTEXT("EM2FRIEND_FB_FAIL"), szName));
	}
}

void GLFriendClient::ReqFriendList()
{
	if (m_bReqList)
        return;

	m_bReqList = true;

	// 메시지 전송
	GLMSG::SNETPC_REQ_FRIENDLIST NetMsg;
	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
}

void GLFriendClient::ReqFriendAdd(const char* szName)
{
    if (!szName)
        return;
	
	const char* charName = m_pGaeaClient->GetCharacter()->GetName();
	if( strcmp( charName, szName ) == 0 )
	{
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::DISABLE,
			sc::string::format(ID2GAMEINTEXT("FRIEND_ADD_FAIL"), szName));

		return ;
	}

    if (IsExist(std::string(szName)))
    {
		//	Note : 이미 추가되어 있습니다.
		m_pGaeaClient->PrintMsgText(
            NS_UITEXTCOLOR::DISABLE,
            sc::string::format(ID2GAMEINTEXT("FRIEND_ADD_ALREADY"), szName));
	}
    else if( GLCONST_CHAR::nMAX_FRIEND_LIST <= m_pGaeaClient->GetFriendClient()->GetFriendListCount() )
    {
        //! 목록 제한 숫자를 넘기는 경우
        m_pGaeaClient->PrintMsgText(
            NS_UITEXTCOLOR::DISABLE,
            sc::string::format(ID2GAMEINTEXT("FRIEND_ADD_MAX"), GLCONST_CHAR::nMAX_FRIEND_LIST));
        return;
    }
    else
    {
	    //	Note : 메시지 전송.
	    GLMSG::SNETPC_REQ_FRIENDADD NetMsg(szName);
	    m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    }
}

void GLFriendClient::ReqFriendLureAns(const char* szReqName, bool bOK)
{
    ReqFriendLureAns( szReqName, bOK, sc::MGUID() );
}

void GLFriendClient::SfReqFriendLureAns_throughAlarm(const char* szReqName, bool bOK, LuaTable tbMGuid)
{
	sc::MGUID mGuid(tbMGuid.get<ULONG>(1)
		, tbMGuid.get<USHORT>(2)
		, tbMGuid.get<USHORT>(3)
		, tbMGuid.get<BYTE>(4)
		, tbMGuid.get<BYTE>(5)
		, tbMGuid.get<BYTE>(6)
		, tbMGuid.get<BYTE>(7)
		, tbMGuid.get<BYTE>(8)
		, tbMGuid.get<BYTE>(9)
		, tbMGuid.get<BYTE>(10)
		, tbMGuid.get<BYTE>(11));
	ReqFriendLureAns(szReqName, bOK, mGuid);
	m_pGaeaClient->GetNotifyClientMan()->Del(mGuid);
}

void GLFriendClient::ReqFriendLureAns(const char* szReqName, bool bOK, const sc::MGUID& Guid )
{
    if( GLCONST_CHAR::nMAX_FRIEND_LIST <= m_pGaeaClient->GetFriendClient()->GetFriendListCount() )
    {
        //! 목록 제한 숫자를 넘기는 경우
        m_pGaeaClient->PrintMsgText(
            NS_UITEXTCOLOR::DISABLE,
            sc::string::format(ID2GAMEINTEXT("FRIEND_ADD_MAX"), GLCONST_CHAR::nMAX_FRIEND_LIST));
        return;
    }

    //	Note : 메시지 전송.
    GLMSG::SNETPC_REQ_FRIENDADD_ANS NetMsg(bOK, szReqName);
    NetMsg.Guid = Guid;
    m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
}

void GLFriendClient::ReqFriendLureAnsForWidget(const char* szReqName, bool bOK)
{
	ReqFriendLureAns( szReqName, bOK );
}

void GLFriendClient::ReqFriendDel ( const char* szDelName, int iDelFlag )
{
    friendlogic::FRIENDMAP_ITER iter = m_mapFriend.find ( szDelName );
	if ( iter==m_mapFriend.end() )
	{		
		m_pGaeaClient->PrintMsgText(
            NS_UITEXTCOLOR::DISABLE,
            sc::string::format(ID2GAMEINTEXT("FRIEND_DEL_FAIL"), szDelName));
		return;
	}

	//	Note : 메시지 전송.
	GLMSG::SNETPC_REQ_FRIENDDEL NetMsg;

	memcpy_s( &NetMsg.iDelFlag , sizeof(int), &iDelFlag, sizeof(int));
	StringCchCopy ( NetMsg.szDEL_CHAR, CHR_ID_LENGTH+1, szDelName );

	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
}

void GLFriendClient::ReqFriendBlock ( const char* szName, bool bBLOCK )
{
    if( GLCONST_CHAR::nMAX_FRIEND_BLOCK <= m_pGaeaClient->GetFriendClient()->GetBlockListCount() )
    {
        m_pGaeaClient->PrintMsgText(
            NS_UITEXTCOLOR::DISABLE,
            sc::string::format(ID2GAMEINTEXT("FRIEND_ADD_MAX"), GLCONST_CHAR::nMAX_FRIEND_BLOCK ));
        return;
    }

    //! 삭제를 의미한다.
    if( bBLOCK == FALSE )
    {
		if ( GetState(szName) == EMFRIEND_BLOCK )
			ReqFriendDel(szName, EMFRIENDDEL_FLAG_PARTY_BLOCK_OFF);		
        return;
    }

	//	Note : 메시지 전송.
	GLMSG::SNETPC_REQ_FRIENDBLOCK NetMsg;
	StringCchCopy ( NetMsg.szCHAR, CHR_ID_LENGTH+1, szName );
	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
}

void GLFriendClient::MsgReqFriendAddFb ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_FRIENDADD_FB *pNetMsg = (GLMSG::SNETPC_REQ_FRIENDADD_FB *) nmg;

	switch (pNetMsg->emFB)
	{
	case EMFRIENDADD_FAIL:		
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::DISABLE,
			sc::string::format(ID2GAMEINTEXT("FRIEND_ADD_FAIL"), pNetMsg->szADD_CHAR));
		break;

	case EMFRIENDADD_OK:		
		{
			m_pGaeaClient->PrintMsgText(
				NS_UITEXTCOLOR::ENABLE,
				sc::string::format(ID2GAMEINTEXT("FRIEND_ADD_OK"), pNetMsg->szADD_CHAR));

			friendlogic::FRIENDMAP_ITER iter = m_mapFriend.find ( pNetMsg->szADD_CHAR );
			if ( iter != m_mapFriend.end() )
			{
				std::tr1::shared_ptr<SFRIEND> pFridend = iter->second;

				GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
				if ( pCharacter->GetPartyID().isValidParty() && m_pGaeaClient->GetMyPartyClient()->isMember(pNetMsg->szADD_CHAR) )
					GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Party_UpdateSlot );

				GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Community_AddFriend, "-n", pFridend->GetGroup() );
			}
		}
		break;

// 2012.10 - 담당(이익세)자의 기획 의도에 따라 거절시 메시지를 보여주지 않는다. cwBack
// 	case EMFRIENDADD_REFUSE:		
// 		m_pGaeaClient->PrintMsgText(
//             NS_UITEXTCOLOR::DISABLE,
//             sc::string::format(ID2GAMEINTEXT("FRIEND_ADD_REFUSE"), pNetMsg->szADD_CHAR));
// 		break;

	case EMFRIENDADD_ABSENCE:		
		m_pGaeaClient->PrintMsgText(
            NS_UITEXTCOLOR::DISABLE,
            sc::string::format(ID2GAMEINTEXT("EMFRIENDADD_ABSENCE"), pNetMsg->szADD_CHAR));
		break;

    case EMFRIENDADD_MAX:
        m_pGaeaClient->PrintMsgText(
            NS_UITEXTCOLOR::DISABLE,
            sc::string::format(ID2GAMEINTEXT("FRIEND_ADD_EMFRIENDADD_MAX"), pNetMsg->szADD_CHAR));
        break;

    default:
        m_pGaeaClient->PrintMsgText(
            NS_UITEXTCOLOR::DISABLE,
            std::string("Friend add unknown error"));
        break;
	};

	// UI Refresh
	// GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Community_FriendTab_Update );
}

void GLFriendClient::MsgReqFriendAddLure ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_FRIENDADD_LURE* pNetMsg = (GLMSG::SNETPC_REQ_FRIENDADD_LURE *) nmg;

	// 토너먼트중 파티를 할수가 없다.
	// TOURNAMENT_UNIQ_DISABLE
	bool bTOURNAMNET = m_pGaeaClient->GetTournamentClient()->GetIsTournamentView();

	// 자동거절인지 확인.
	if (RANPARAM::bDIS_FRIEND || bTOURNAMNET)
	{
		//	Note : 메시지 전송.
        ReqFriendLureAns( pNetMsg->szREQ_CHAR, false );
        return;
	}

    // 친구 수락 메시지
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Community_ReqFriend,
		"-s, -b",
		pNetMsg->szREQ_CHAR,
		GLUseFeatures::GetInstance().IsUsingNotifyWindow() );
}

void GLFriendClient::MsgReqFriendDelFb ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_FRIENDDEL_FB *pNetMsg = (GLMSG::SNETPC_REQ_FRIENDDEL_FB *) nmg;

	switch ( pNetMsg->emFB )
	{
	case EMFRIENDDEL_FAIL:		
		m_pGaeaClient->PrintMsgText(
            NS_UITEXTCOLOR::DISABLE,
            sc::string::format(ID2GAMEINTEXT("FRIEND_DEL_FAIL"), pNetMsg->szDEL_CHAR));
		break;

	case EMFRIENDDEL_OK:		

		std::tr1::shared_ptr<SFRIEND> spFirend = GetData(pNetMsg->szDEL_CHAR);
		const WORD wGroupNum(spFirend ? spFirend->GetGroup() : 0);
		
		//	Note : 친구 리스트에서 삭제됨.
		Del ( pNetMsg->szDEL_CHAR );

		// 파티창의 친구등록 버튼 갱신을 위함;
		GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
		if ( pCharacter->GetPartyID().isValidParty() && m_pGaeaClient->GetMyPartyClient()->isMember(pNetMsg->szDEL_CHAR) )
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Party_UpdateSlot );
		}

		switch( pNetMsg->iDelFlag )
		{
		case EMFRIENDDEL_FLAG_NORMAL:
			m_pGaeaClient->PrintMsgText(
				NS_UITEXTCOLOR::ENABLE,
				sc::string::format( ID2GAMEINTEXT("FRIEND_DEL_OK"), pNetMsg->szDEL_CHAR));

			//	Note : 인터페이스에 변경 알림.
			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_RnCommunity_BlockTab_Update );
			break ;

		case EMFRIENDDEL_FLAG_PARTY_BLOCK_OFF:
			m_pGaeaClient->PrintMsgText(
				NS_UITEXTCOLOR::ENABLE,
				sc::string::format( ID2GAMEINTEXT("EMFRIENDBLOCK_OFF"), pNetMsg->szDEL_CHAR));

			//	Note : 인터페이스에 변경 알림.
			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_Community_RemoveFriend,
				"-n", wGroupNum );
			break ;
		}
		break;
	};
}

void GLFriendClient::MsgReqFriendBlockFb ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_FRIENDBLOCK_FB *pNetMsg = (GLMSG::SNETPC_REQ_FRIENDBLOCK_FB *) nmg;

	switch (pNetMsg->emFB)
	{
	case EMFRIENDBLOCK_FAIL:		
		m_pGaeaClient->PrintMsgText(
            NS_UITEXTCOLOR::DISABLE,
            sc::string::format(ID2GAMEINTEXT("EMFRIENDBLOCK_FAIL"), pNetMsg->szCHAR));
		break;

	case EMFRIENDBLOCK_ON:		
		m_pGaeaClient->PrintMsgText(
            NS_UITEXTCOLOR::ENABLE,
            sc::string::format(ID2GAMEINTEXT("EMFRIENDBLOCK_ON"), pNetMsg->szCHAR));
		break;

// 	case EMFRIENDBLOCK_OFF:		
// 		m_pGaeaClient->PrintMsgText(
//             NS_UITEXTCOLOR::ENABLE,
//             sc::string::format(ID2GAMEINTEXT("EMFRIENDBLOCK_OFF"), pNetMsg->szCHAR));
// 		break;
	};

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_RnCommunity_BlockTab_Update );
}

void GLFriendClient::MsgReqFriendInfo(NET_MSG_GENERIC* nmg)
{
    std::vector<SFRIEND> vecFriend;
    MSGPACK_AUTO_UNPACK<SFRIEND> autoUnpack;
    if( autoUnpack.GetVecData( nmg, vecFriend ) )
    {
        BOOST_FOREACH( const SFRIEND& sFriend, vecFriend )
        {
            Add(sFriend);
            m_pGaeaClient->AddCharacterCache(sFriend.GetChaDbNum(), sFriend.GetName());
        }

        // 인터페이스에 변경 알림
		// GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Community_FriendTab_Update );
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Community_FriendStateUpdate, "-n", -1 );
    }
    else
    {
        sc::writeLogError("GLFriendClient::MsgReqFriendInfo");
    }
}

void GLFriendClient::MsgReqFriendState(NET_MSG_GENERIC* nmg)
{
    // NET_MSG_FRIENDSTATE
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
    GLMSG::SNETPC_REQ_FRIENDSTATE RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

	friendlogic::FRIENDMAP_ITER iter = m_mapFriend.find(RecvData.m_FriendName);
	if ( iter!=m_mapFriend.end() )
	{
		std::tr1::shared_ptr<SFRIEND> sFRIEND = iter->second;
		
		// 차단 이후에는 삭제 말고는 받을 메세지가 없으므로 이렇게 수정 
		if( sFRIEND->m_Flag != -1 )
			sFRIEND->m_Flag    = RecvData.m_FriendFlag;
		
		sFRIEND->m_Online  = RecvData.m_Online;
		sFRIEND->m_Channel = RecvData.m_Channel;

		if (sFRIEND->m_Online)
		{			
			m_pGaeaClient->PrintMsgText(
                NS_UITEXTCOLOR::LIGHTSKYBLUE, 
                sc::string::format(ID2GAMEINTEXT( "FRIEND_ONLINE" ), RecvData.m_FriendName.c_str()));

			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Party_UpdateSlot );
		}

		//	Note : 인터페이스에 상태 변경 알림.
		//GLWidgetScript::GetInstance().LuaCallFunc< void >(
		//	NSWIDGET_SCRIPT::g_strFunc_Community_FriendTab_Update );
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_Community_FriendStateUpdate, "-n", sFRIEND->GetGroup());
	}
}

void GLFriendClient::MsgReqFriendRename ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_FRIENDRENAME_CLT *pNetMsg = (GLMSG::SNETPC_REQ_FRIENDRENAME_CLT *) nmg;

	friendlogic::FRIENDMAP_ITER iter = m_mapFriend.find(pNetMsg->szOldName);	
	if ( iter != m_mapFriend.end() )
	{
		std::tr1::shared_ptr<SFRIEND> sFRIEND = iter->second;
		sFRIEND->m_ChaName = pNetMsg->szNewName;

        friendlogic::Friend::Del(pNetMsg->szOldName);
        Add(sFRIEND);

		//	Note : 인터페이스에 상태 변경 알림.
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_Community_FriendStateUpdate, "-n", sFRIEND->GetGroup() );
	}
}
/*
void GLFriendClient::MsgReqFriendPhoneNumber ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_FRIENDPHONENUMBER_CLT *pNetMsg = (GLMSG::SNETPC_REQ_FRIENDPHONENUMBER_CLT *) nmg;

	friendlogic::FRIENDMAP_ITER iter = m_mapFriend.find ( pNetMsg->szName );
	GASSERT ( iter != m_mapFriend.end() );
	if ( iter != m_mapFriend.end() )
	{
		std::tr1::shared_ptr<SFRIEND> sFRIEND = iter->second;

		//StringCchCopy ( sFRIEND.szPhoneNumber, SMS_RECEIVER, pNetMsg->szNewPhoneNumber );

		//	Note : 인터페이스에 상태 변경 알림.
		m_pInterface->REFRESH_FRIEND_STATE();
		m_pInterface->FRIEND_LIST(sFRIEND->m_ChaName.c_str(), sFRIEND->m_Online);
	}
}
*/
void GLFriendClient::MsgProcess ( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	case NET_MSG_REQ_FRIENDADD_FB:
        MsgReqFriendAddFb(nmg);
        break;
	case NET_MSG_REQ_FRIENDADD_LURE:
        MsgReqFriendAddLure(nmg);
        break;
	case NET_MSG_REQ_FRIENDDEL_FB:
        MsgReqFriendDelFb(nmg);
        break;
	case NET_MSG_REQ_FRIENDBLOCK_FB:
        MsgReqFriendBlockFb(nmg);
        break;
	case NET_MSG_FRIENDINFO:
        MsgReqFriendInfo(nmg);
        break;
	case NET_MSG_FRIENDSTATE:
        MsgReqFriendState(nmg);
        break;
	case NET_MSG_GCTRL_FRIEND_RENAME_CLT:
        MsgReqFriendRename(nmg);
        break;
	//case NET_MSG_GCTRL_FRIEND_PHONENUMBER_CLT:
    //  MsgReqFriendPhoneNumber(nmg);
        break;
	};
}

bool GLFriendClient::GroupDel(WORD GroupNum)
{
    if (!friendlogic::Friend::GroupDel(GroupNum))
        return false;

    for (friendlogic::FRIENDMAP_ITER iter=m_mapFriend.begin(); iter!=m_mapFriend.end(); ++iter)
    {
        std::tr1::shared_ptr<SFRIEND> spFriend = iter->second;
        if (spFriend && spFriend->GetGroup() == GroupNum)
            spFriend->SetGroup(0);
    }

    return true;
}

int GLFriendClient::GetFriendDBNum(const char* szName)
{
	friendlogic::FRIENDMAP_ITER _begin = m_mapFriend.begin();
	friendlogic::FRIENDMAP_ITER _end = m_mapFriend.end();

	for(;_begin != _end; ++_begin)
	{
		std::tr1::shared_ptr<SFRIEND> _Friend = _begin->second;
		if(!strcmp(_Friend->GetName().c_str(), szName))
			return _Friend->GetChaDbNum();
	}

	return -1;
}

//LuaTable GLFriendClient::GetFriendList()
//{
//	friendlogic::FRIENDMAP_ITER _begin = m_mapFriend.begin();
//	friendlogic::FRIENDMAP_ITER _end = m_mapFriend.end();
//
//	std::vector<CString> _vecCStr;	
//	LuaTable tbStrList( GLWidgetScript::GetInstance().GetLuaState() );
//
//	for(;_begin != _end; ++_begin)
//	{
//		std::tr1::shared_ptr<SFRIEND> _Friend = _begin->second;
//		if(!_Friend->IsOFF())
//		{		
//			CString _Name(_Friend->GetName().c_str());
//			_vecCStr.push_back(_Name);
//		}
//	}
//
//	unsigned int _Size = _vecCStr.size();
//	for(unsigned int _index(0); _index < _Size; ++_index)
//	{
//		tbStrList.set(_index + 1, _vecCStr[_index].GetBuffer());
//	}
//
//	return tbStrList;
//}