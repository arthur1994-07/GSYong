#include "pch.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../RanLogic/s_NetGlobal.h"
#include "../../RanLogic/MSG/GLContrlMsg.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogicClient/Tournament/GLTournamentClient.h"
#include "../../Ranlogicclient/GLGaeaClient.h"


#include "../Land/GLLandManClient.h"
#include "../GLGaeaClient.h"
#include "../Stage/DxGlobalStage.h"
#include "./GroupChatClient.h"

#include "../MfcExLib/RanFilter.h"

#include "../../RanUI/InnerInterface.h"

#include "../SigmaCore/String/StringFormat.h"

#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/GUInterface/UITextControl.h"
#include "../EngineLib/DxTools/DebugSet.h"

#include "../RanLogic/Item/GLItemMan.h"
#include "../RanLogic/Msg/ApexMsg.h"
#include "../RanLogic/GLogicDataMan.h"

#include "../RanLogicClient/Friend/GLFriendClient.h"
#include "../RanLogicClient/Stage/DxGlobalStage.h"
#include "../RanLogicClient/SNS/GLSNSClient.h"
#include "../RanLogicClient/GLGaeaClient.h"
#include "../RanLogicClient/Tutorial/GLBaseTutorial.h"
#include "../RanLogicClient/Land/GLLandManClient.h"
#include "../RanLogicClient/Pet/GLPetClient.h"
#include "../RanLogicClient/Club/GLClubClient.h"
#include "../RanLogicClient/Chat/GLChatMan.h"

#include "../RanLogic/RANPARAM.h"

#include "../../RanUI/OldUI/Util/ModalWindow.h"
#include "../../RanUI/OldUI/Club/ClubWindow.h"
#include "../../RanUI/OldUI/Confront/ConftModalWindow.h"
#include "../../RanUI/OldUI/NPC/DialogueWindow.h"
#include "../../RanUI/OldUI/Friend/FriendWindow.h"
#include "../../RanUI/OldUI/Item/InventoryWindow.h"
#include "../../RanUI/OldUI/Item/InventoryInterface.h"
#include "../../RanUI/OldUI/Item/MarketWindow.h"
#include "../../RanUI/OldUI/Party/PartyWindow.h"
#include "../../RanUI/OldUI/Party/PartyModalWindow.h"
#include "../../RanUI/OldUI/Groupchat/ChatGroupModalWindow.h"
#include "../../RanUI/OldUI/PrivateMarket/PrivateMarketSellWindow.h"
#include "../../RanUI/OldUI/PrivateMarket/PrivateMarketWindow.h"
#include "../../RanUI/OldUI/Char/PtoPWindow.h"
#include "../../RanUI/OldUI/Quest/QuestWindow.h"
#include "../../RanUI/OldUI/Quest/WindowRenewal/QuestWindowRenewal.h"
#include "../../RanUI/OldUI/Quest/WindowRenewal/Main/QuestMainRenewal.h"
#include "../../RanUI/OldUI/Message/WaitServerDialogue.h"
#include "../../RanUI/OldUI/Message/RebirthDialogue.h"
#include "../../RanUI/OldUI/Feature/StatsResetWindow.h"
#include "../../RanUI/OldUI/Post/PostBoxWindow.h"
#include "../../RanUI/OldUI/Post/PostBoxReadPage.h"
#include "../../RanUI/OldUI/Post/PostBoxSendPage.h"
#include "../../RanUI/OldUI/Post/PostBoxReceivePage.h"
#include "../../RanUI/OldUI/NPC/DialogueWindowRenewal.h"
#include "../../RanUI/OldUI/GroupChat/MaxChatWindow.h"
#include "../../RanUI/OldUI/GroupChat/ChatGroupWindow.h"

#include "../../RanLogicClient/Friend/GLFriendClient.h"
#include "../../RanLogicClient/Char/CharNameDbNumCache.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "../Notify/NotifyClientFactory.h"
#include "../Notify/NotifyClientMan.h"
// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


MAP_CHATCLIENT_MEMBER ChatRoomClient::GetMapMembers()
{
	return m_mapMembers;
}

BOOL ChatRoomClient::ADDMEMBER(  DWORD dwcharNum, CHAT_MEMBER sMember )
{
	//�濡 �ɹ��� �߰��Ѵ�
    MAP_CHATCLIENT_MEMBER_ITER iter = m_mapMembers.find(dwcharNum);
    if (iter != m_mapMembers.end())
        m_mapMembers.erase(iter);

	m_mapMembers.insert( MAP_CHATCLIENT_MEMBER_VALUE( dwcharNum, sMember ) );
	if ( dwcharNum == m_dwKey )
		m_strOwnerName = sMember.szChaName;
	return TRUE;
}

BOOL ChatRoomClient::DELMEMBER(  DWORD dwcharNum )
{
	//���� �ɹ��� �����Ѵ�
	if ( m_mapMembers.end() == m_mapMembers.find(dwcharNum) )
	return FALSE;

	m_mapMembers.erase(m_mapMembers.find(dwcharNum));
	MAP_CHATCLIENT_MEMBER_ITER it = m_mapMembers.find(dwcharNum);

	if (m_mapMembers.end() == it )
		return TRUE;
	else
		return FALSE;
}

void ChatRoomClient::GetMembername(std::vector<CString>& vecMemberName)
{
	// �ɹ����� �̸��� ���Ϳ� ��� �Ѱ��ش�
	vecMemberName.clear();

	MAP_CHATCLIENT_MEMBER_ITER iter = m_mapMembers.begin();

	for (; iter != m_mapMembers.end(); ++iter)
	{
		vecMemberName.push_back(CString(iter->second.szChaName));
	}

}


void ChatRoomClient::GetMember(std::vector<CHAT_MEMBER>& vecMember)
{
	//CHAT_MEMBER ����ü�� ���� �ɹ����� ������ ��� �Ѱ��ش�
	vecMember.clear();

	MAP_CHATCLIENT_MEMBER_ITER iter = m_mapMembers.begin();

	for (; iter != m_mapMembers.end(); ++iter)
	{
		vecMember.push_back(iter->second);
	}

}

bool ChatRoomClient::AddLinkItem( const SITEMCUSTOM& sItem )
{
	if( HOW_MANY_ITEM_LINK_PER_CHAT_MSG <= m_listLinkItem.size() )
		return false;

	m_listLinkItem.push_back( sItem );

	return true;
}

void ChatRoomClient::RemoveLinkItem( int nIndex )
{
	if( m_listLinkItem.size() <= (size_t) nIndex )
		return;

	std::list< SITEMCUSTOM >::iterator iter = m_listLinkItem.begin();
	for( int i = 0; iter != m_listLinkItem.end(); ++iter, ++i )
	{
		if( nIndex == i )
		{
			m_listLinkItem.erase( iter );
			break;
		}
	}
}

void ChatRoomClient::ClearLinkItem()
{
	m_listLinkItem.clear();
}

GLGroupChat::~GLGroupChat()
{
	ChatRoomClear();
	GLMSG::SNET_GCHAT_QUIT_CA msg;
	msg.dwKey = m_dwKey;
	m_pGaeaClient->NETSENDTOAGENT(&msg);
}

void GLGroupChat::GetRoomName( std::vector<CString>& vecRoomName, std::vector<DWORD>& vecRoomdwKey )
{
	//�� ���� ���� �̸��� DBNUM�� �޾ƿ´�
	BOOST_FOREACH( MAP_CHATROOMCLIENT_VALUE& it, m_mapChatRoom )
	{
		ChatRoomClient& cRoomname = it.second;
		CString strRoomName = cRoomname.GetOwnerName();
		if (strRoomName.GetLength())
			vecRoomName.push_back( strRoomName );

		DWORD cRoomdwKey = it.first;
		vecRoomdwKey.push_back( cRoomdwKey );
		if (!strRoomName.GetLength())
			vecRoomName.push_back( m_pGaeaClient->FindChaNameAtCache(cRoomdwKey).c_str());	
	}

}
CString GLGroupChat::GetMembername( DWORD dwRoomkey, DWORD dwMemberKey )
{
	//���� DB��ȣ�� ĳ������ DB��ȣ�� ������
	// �ɸ����� �̸��� �Ѱ��ش�
	MAP_CHATROOMCLIENT_ITER iter = m_mapChatRoom.find(dwRoomkey);

	if (iter == m_mapChatRoom.end())
	{
		GASSERT( iter == m_mapChatRoom.end() && "dwRoomkey Ű���� ���� �����ϴ�" );
		return FALSE;
	}
	ChatRoomClient& cRoom = iter->second;

	MAP_CHATCLIENT_MEMBER_ITER it = cRoom.m_mapMembers.find(dwMemberKey);
	if (it == cRoom.m_mapMembers.end())
	{
		GASSERT(it == cRoom.m_mapMembers.end() && "Ű���� �ɹ��� �����ϴ�");
		return FALSE;
	}
	CHAT_MEMBER& sMember = it->second;

	return sMember.szChaName;
}

void GLGroupChat::GetMembername( std::vector<CHAT_MEMBER>& member, DWORD dwkey )
{
	MAP_CHATROOMCLIENT_ITER iter = m_mapChatRoom.find(dwkey);

	if (iter == m_mapChatRoom.end())
	{
		GASSERT(iter == m_mapChatRoom.end() && "Ű���� ���� �����ϴ�");
		return;
	}

	ChatRoomClient& cRoom = iter->second;
	
	//cRoom.GetMembername(vecMemberName);
	cRoom.GetMember(member);
}

BOOL GLGroupChat::MsgChatGroupInviteCAC(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GCHAT_INVITE_CAC *msg = (GLMSG::SNET_GCHAT_INVITE_CAC *)nmg;

	m_dwKey = msg->dwKey;

	DWORD dwKey = msg->dwKey;				// �ʴ��� ���
	DWORD dwChaWhom = msg->dwChaNumWhom;	// �ʴ���� ĳ��

	// ��ʸ�Ʈ�� ��Ƽ�� �Ҽ��� ����.
	// TOURNAMENT_UNIQ_DISABLE
	bool bTOURNAMNET = m_pGaeaClient->GetTournamentClient()->GetIsTournamentView();

	//���� Ȥ�� ���� ����� ������ ����޽��� SNET_CHAT_INVITE_CAC_FB �� ������Ʈ��
	if ( RANPARAM::bGroupChatInvite || bTOURNAMNET) //�ڵ� ���� �޽���
	{
		GLMSG::SNET_GCHAT_INVITE_CAC_FB NetMsg; 
		NetMsg.bAccept = FALSE;
		NetMsg.dwKey = dwKey;
		NetMsg.dwChaWhom = dwChaWhom;
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

		return FALSE;
	}

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_GroupChat_InviteRecv,
		"-s", &msg->szChaNameOwner );

	return FALSE;	
}

BOOL GLGroupChat::MsgChatGroupInviteCAC_FB(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GCHAT_INVITE_CAC_FB *msg = (GLMSG::SNET_GCHAT_INVITE_CAC_FB *)nmg;

	DWORD dwKey = msg->dwKey;
	DWORD dwChaWhom = msg->dwChaWhom;
	BOOL bAccept = msg->bAccept;
	BOOL bAcceptDone = msg->bAcceptDone;

	if( FALSE == bAccept )
	{
		std::string strCharName = m_pGaeaClient->FindChaNameAtCache(dwChaWhom);
		std::string strTemp = sc::string::format( ID2GAMEWORD("INVITE_GROUP_CHAT_WINDOW_INVITE_MSG_REFUSAL") , strCharName.c_str());
		m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, strTemp );
	}

	//if ( m_dwKey == NULL && m_pGaeaClient->GetCharacter()->GetCharID() == dwKey )
	//	m_dwKey = dwKey;		

	return TRUE;
}

BOOL GLGroupChat::MsgChatGroupERRORCODE(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GCHAT_ERRORCODE *msg = (GLMSG::SNET_GCHAT_ERRORCODE *)nmg;
	
	GASSERT(msg->code);

	std::string strErrorMsg;
	//strErrorMsg = ID2GAMEWORD("INVITE_GROUP_CHAT_WINDOW_INVITE_MSG_REFUSAL");

	switch(msg->code)
	{
	case CHAT_CODE_OK:
		strErrorMsg.clear();
		break;

	case CHAT_CODE_ROOM_NOT_CONSTRUCT:
		strErrorMsg ="�׷�ä�� ���� ����";
		break;

	case CHAT_CODE_ROOM_NOT_EXIST:
		//strErrorMsg ="�ش� �׷�ä���� �������� �ʽ��ϴ�";
		strErrorMsg = ID2GAMEWORD("GROUP_CHAT_CODE_ROOM_NOT_EXIST");
		break;

	case CHAT_CODE_MAX_OPEN_CHAT:
		//strErrorMsg = "�� �̻� �׷�ä�ÿ� ������ �� �����ϴ�.";
		strErrorMsg = ID2GAMEWORD("GROUP_CHAT_CODE_MAX_OPEN_CHAT");
		break;

	case CHAT_CODE_MAX_MEMBERS:
		//strErrorMsg = "�ִ� ä�� �����ڸ� �ʰ� �Ͽ����ϴ�.";
		strErrorMsg = ID2GAMEWORD("GROUP_CHAT_CODE_MAX_MEMBERS");
		break;

	case CHAT_CODE_OWNER_LOW_LEVEL:
		//strErrorMsg ="�׷�ä�ÿ� ���������� �������� �����ϴ�";
		strErrorMsg = ID2GAMEWORD("GROUP_CHAT_CODE_OWNER_LOW_LEVEL");
		break;

	case CHAT_CODE_MEMBER_LOW_LEVEL:
		//strErrorMsg ="�׷�ä�ÿ� ���������� �������� �����ϴ�";
		strErrorMsg = ID2GAMEWORD("GROUP_CHAT_CODE_MEMBER_LOW_LEVEL");
		break;
	case CHAT_CODE_OTHER_PRISON:
		strErrorMsg = ID2GAMEINTEXT("CHAT_INVITE_PRISION_LIMIT");
		break;
	case CHAT_CODE_PRISON :
		strErrorMsg = ID2GAMEINTEXT("CHAT_GROUP_PRISION_LIMIT");
		break;
	case CHAT_CODE_RECEIVE_PRISON :
		strErrorMsg = ID2GAMEINTEXT("CHAT_RECEIVE_PRISION_LIMIT");
		break;
	}

	if( strErrorMsg.length() )
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::LIGHTSKYBLUE, strErrorMsg );

	return TRUE;
}

BOOL GLGroupChat::MsgChatGroupQUIT_CA(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GCHAT_QUIT_CA *msg = (GLMSG::SNET_GCHAT_QUIT_CA *)nmg;
	
	DWORD dwKey = msg->dwKey;

	DELCHATROOMCLIENT( dwKey, m_pGaeaClient->GetCharacter()->CharDbNum() );
	DELCHATROOM( dwKey, m_pGaeaClient->GetCharacter()->CharDbNum() );

	return TRUE;
}

BOOL GLGroupChat::MsgChatGroupMEMBER_UPDATE_AC(NET_MSG_GENERIC* nmg)
{	
	GLMSG::SNET_GCHAT_MEMBER_UPDATE_AC *msg = (GLMSG::SNET_GCHAT_MEMBER_UPDATE_AC *)nmg;

	DWORD dwKey = msg->dwKey;
	bool bJoin = TRUE == msg->bJoin ? true : false;
	CHAT_MEMBER who = msg->who;

	int nMaxRoomIndex = GetMaxChatRoomIndex( dwKey );
	if( nMaxRoomIndex < 0 )
	{
		GASSERT( "���� �����ϴ�.");
		return FALSE;
	}

	std::string strName( who.szChaName );

	// �ڱ��ڽ����� �����°�
	bool bSelf = false;

	if( true == bJoin )
	{
		if( TRUE == ADDCHATROOMCLIENT( dwKey, who, std::string() ) )
		{
			// ������ ��� ���� �޽��� ���
			if( m_pGaeaClient->GetCharacter()->GetCharID() == who.dwChaNum )
				bSelf = true;
		}
	}
	else
	{
		if( TRUE == DELCHATROOMCLIENT( dwKey, who.dwChaNum ) )
			bSelf = false;

		if ( who.dwChaNum == m_pGaeaClient->GetCharacter()->CharDbNum() )
			DELCHATROOM( dwKey, who.dwChaNum );
	}

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_GroupChat_UpdateMember,
		"-dw, -s, -b, -b", dwKey, strName.c_str(), bJoin, bSelf );

	return TRUE;
}

BOOL GLGroupChat::MsgChatMemberSyncAC(NET_MSG_GENERIC* nmg)
{
	//��ũ �޽���
	//�濡 ���� ����鿡�� ������ �濡 �ִ� �ɹ�����
	// ������ ���� ���ش�
	GLMSG::SNET_GCHAT_MEMBER_SYNC_AC *msg = (GLMSG::SNET_GCHAT_MEMBER_SYNC_AC *)nmg;

	DWORD dwKey = msg->dwKey;
	std::string strOwnerName = "";

	for (INT i = 0; i < msg->nSync; ++i )
	{
		if ( msg->dataSync[i].dwChaNum == dwKey )
			strOwnerName = msg->dataSync[i].szChaName;
	}
	
	for( unsigned int i = 0; i < msg->nSync; ++i )
		ADDCHATROOMCLIENT( dwKey, msg->dataSync[ i ], strOwnerName );

	return TRUE;
}

BOOL GLGroupChat::MsgChatGroupKickCA(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GCHAT_KICK_CA *msg = (GLMSG::SNET_GCHAT_KICK_CA *)nmg;

	return TRUE;
}

BOOL GLGroupChat::MsgChatGroupMESSAGECAC(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GCHAT_MESSAGE_CAC *msg = (GLMSG::SNET_GCHAT_MESSAGE_CAC *)nmg;

	DWORD dwkey = msg->dwKey;
	DWORD dwChaSay = msg->dwChaSay;
	int nLink = msg->nLink;

	//����: UI�� ä�� �޼����� �Է��ϴ� �κ� UI������ �̵� �Ұ�
	
	//���� �˻�
	//���� ģ�� ���踦 �޾ƿ´�
	std::tr1::shared_ptr<SFRIEND> spFriend = m_pGaeaClient->GetFriendClient()->GetData( dwChaSay );
	if( dwChaSay != m_pGaeaClient->GetCharacter()->CharDbNum() && !(spFriend == NULL) )
	{
		if( spFriend->IsBLOCK() )
			return FALSE;
	}

	int nMaxRoomIndex = GetMaxChatRoomIndex( dwkey );

	if ( nMaxRoomIndex < 0 )
	{
		GASSERT("���� �����ϴ�");
		return FALSE;
	}

	CString strName = GetMembername( dwkey, dwChaSay );
	std::string strChat = msg->szChat;

	if( nLink <= 0 )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_GroupChat_RecvMsg,
			"-dw, -s, -s",
			dwkey, strName.GetBuffer(), strChat.c_str() );
	}
	else
	{
		VEC_LINK_TEXT_DATA_EX vecTextLinkData;
		for( int i = 0; i < nLink; ++i )
		{
			if( HOW_MANY_ITEM_LINK_PER_CHAT_MSG <= i )
				break;

			SITEM* pItem = GLogicData::GetInstance().GetItem( msg->itemLink[ i ].GetNativeID() );
			if( NULL == pItem )
				continue;

			SLINK_TEXT_DATA_EX sTextLinkData;
			sTextLinkData.sLinkDataBasic.sLinkType.wMType = ITEM_LINK;
			sTextLinkData.sLinkDataBasic.sLinkType.wSType = ITEM_LINK_NORMAL;
			sTextLinkData.sLinkDataBasic.sLinkPos.nStart = 0;
			sTextLinkData.sLinkDataBasic.sLinkPos.nEnd = 0;
			sTextLinkData.sItemcustom = msg->itemLink[ i ];
			sTextLinkData.dwTextColor = ITEM_INFOR_COLOR::dwItemRank[ pItem->sBasicOp.emLevel ];

			vecTextLinkData.push_back( sTextLinkData );
		}

		VEC_LINK_TEXT_DATA vecLinkData;
		GLChatMan::GetInstance().ConvertLinkData( vecTextLinkData, vecLinkData );

		LuaTable tbLinkTextDataList( GLWidgetScript::GetInstance().GetLuaState() );
		int nTableIndex = 1;

		VEC_LINK_TEXT_DATA_ITER iter = vecLinkData.begin();
		for( ; iter != vecLinkData.end(); ++iter )
		{
			LuaTable tbLinkTextData( GLWidgetScript::GetInstance().GetLuaState() );
			tbLinkTextData.set( "nLinkTypeIndex", iter->nLinkTypeIndex );
			tbLinkTextData.set( "nStart", iter->sLinkPos.nStart );
			tbLinkTextData.set( "nEnd", iter->sLinkPos.nEnd );
			tbLinkTextData.set( "dwColor", iter->dwTextColor );
			tbLinkTextData.set( "strName", msg->itemLink[ nTableIndex - 1 ].GETNAME().c_str() );

			tbLinkTextDataList.set( nTableIndex++, tbLinkTextData );
		}

		GLWidgetCallFuncValue args[ 4 ];
		args[ 0 ].SetInt( dwkey );
		args[ 1 ].SetString( strName.GetBuffer() );
		args[ 2 ].SetString( strChat.c_str() );
		args[ 3 ].SetTable( &tbLinkTextDataList );

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_GroupChat_RecvMsg, args, 4 );
	}

	return TRUE;
}

BOOL GLGroupChat::MsgChatGroupConfigAC(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GCHAT_CONFIG_AC *msg = (GLMSG::SNET_GCHAT_CONFIG_AC *)nmg;

	m_dwMaxGroupChatOpen = msg->dwMaxGroupChatOpen +1;
	m_dwMaxGroupChatTO = msg->dwMaxGroupChatTO;

	return TRUE;
}



HRESULT GLGroupChat::MsgProcess(NET_MSG_GENERIC* nmg)
{
	switch ( nmg->nType )
	{
	case NET_MSG_GCHAT_INVITE_CAC:
		MsgChatGroupInviteCAC ( nmg );
		break;

	case NET_MSG_GCHAT_INVITE_CAC_FB:
		MsgChatGroupInviteCAC_FB ( nmg );
		break;

	case NET_MSG_GCHAT_ERRORCODE:
		MsgChatGroupERRORCODE ( nmg );
		break;

	case NET_MSG_GCHAT_QUIT_CA:
		MsgChatGroupQUIT_CA ( nmg );
		break;

	case NET_MSG_GCHAT_MEMBER_UPDATE_AC:
		MsgChatGroupMEMBER_UPDATE_AC( nmg );
		break;

	case NET_MSG_GCHAT_MEMBER_SYNC_AC:
		MsgChatMemberSyncAC( nmg );
		break;

	case NET_MSG_GCHAT_KICK_CA:
		MsgChatGroupKickCA ( nmg );
		break;

	case NET_MSG_GCHAT_MESSAGE:
		MsgChatGroupMESSAGECAC ( nmg );
		break;

	case NET_MSG_GCHAT_CONFIG_AC:
		MsgChatGroupConfigAC ( nmg );
		break;
	};

	return S_OK;
}

BOOL GLGroupChat::ADDCHATROOMCLIENT( DWORD dwKey, CHAT_MEMBER sMember, std::string strOwnerName  )
{
	//������ �ð��� �ȴٸ� ���� �ʿ��ҵ�, ��ũ��Ʈ���� ������ ������ �������� �о �׷�ä�� ������ �������׼� �޾ƿͼ�
	//�� ������ŭ UI�Ҵ��� �����ϰ� �����, ���� �˻��ϰ� �����ϴ� ������ ������(��Ŷ ����ü���� ����)
	//if ( m_mapChatRoom.size() > m_dwMaxGroupChatTO ) 
	//	return  FALSE;

	MAP_CHATROOMCLIENT_ITER it = m_mapChatRoom.find(dwKey);
	if( m_mapChatRoom.end() != it )
	{		
		ChatRoomClient &cRoom = it->second;
		cRoom.ADDMEMBER( sMember.dwChaNum, sMember );
		m_mapChatRoom.insert( MAP_CHATROOMCLIENT_VALUE( dwKey, cRoom ) );
		return TRUE;
	}
	else
	{
		ChatRoomClient cRoom( dwKey );
		cRoom.SETKEY( dwKey ); // ���� Ű ���
		
		int nIndex = GetFreeRoomIndex();

		if (nIndex < 0)
		{
			GASSERT(nIndex < 0 && " ����� �ִ���� �ʰ� �Ͽ����ϴ�");
			return FALSE;
		}

		if ( dwKey == m_pGaeaClient->GetCharacter()->CharDbNum() )
			cRoom.SETOwnerName( m_pGaeaClient->GetCharacter()->GetName() ); // ���� �̸� ���
		
		if ( dwKey == sMember.dwChaNum )
			cRoom.SETOwnerName( sMember.szChaName ); // ���� �̸� ���

		cRoom.ADDMEMBER( sMember.dwChaNum, sMember );
		m_mapChatRoom.insert( MAP_CHATROOMCLIENT_VALUE( dwKey, cRoom ) );

 		GLWidgetScript::GetInstance().LuaCallFunc< void >(
 			NSWIDGET_SCRIPT::g_strFunc_GroupChat_CreateRoom,
 			"-n, -dw, -s", nIndex, dwKey, strOwnerName.c_str() );
	}

	return FALSE;
}


BOOL GLGroupChat::DELCHATROOMCLIENT ( DWORD dwOwnerKey, DWORD dwMembenum  )
{
	//��ȭ�� �����ڸ� ��������
	MAP_CHATROOMCLIENT_ITER it = m_mapChatRoom.find(dwOwnerKey);
	if( m_mapChatRoom.end() != it )
	{		
		ChatRoomClient &cRoom = it->second;
		cRoom.DELMEMBER( dwMembenum );
		
		return TRUE;
	}

	return FALSE;
}

BOOL GLGroupChat::DELCHATROOM(DWORD dwOwnerKey, DWORD dwMembenum)
{
	//���� �����ϰ� �ش� UI�� ��ȭ���� ����
	if (m_pGaeaClient->GetCharacter()->CharDbNum() == dwMembenum)
	{
		if (m_mapChatRoom.end() != m_mapChatRoom.find(dwOwnerKey))
		{
			for (DWORD i = 0; i < m_dwMaxGroupChatOpen; i++)
			{
				if( dwOwnerKey == GLWidgetScript::GetInstance().LuaCallFunc< DWORD >(
					NSWIDGET_SCRIPT::g_strFunc_GroupChat_GetKey, "-n", (int) i ) )
				{
					GLWidgetScript::GetInstance().LuaCallFunc< void >(
						NSWIDGET_SCRIPT::g_strFunc_GroupChat_DeleteRoom, "-dw", dwOwnerKey );
				}
			}

			m_mapChatRoom.erase(dwOwnerKey);
			
			return TRUE;
		}
	}

	return FALSE;
}

int GLGroupChat::GetFreeRoomIndex()
{
	// ��� ������ â�� �ε����� �Ѱ��ش�
	for( DWORD i = 0; i < m_dwMaxGroupChatOpen; ++i )
	{
		if( true == GLWidgetScript::GetInstance().LuaCallFunc< bool >(
			NSWIDGET_SCRIPT::g_strFunc_GroupChat_IsRoomFree, "-n", (int) i ) )
			return i;
	}

	return -1;
}

int GLGroupChat::GetMaxChatRoomIndex( DWORD dwkey )
{
	// ��ȭâ�� ��Ű�� �ָ� �׹��� �ε����� �Ѱ��ش�
	for( DWORD i = 0; i < m_dwMaxGroupChatOpen; ++i )
	{
		DWORD dwRoomKey = GLWidgetScript::GetInstance().LuaCallFunc< DWORD >(
			NSWIDGET_SCRIPT::g_strFunc_GroupChat_GetKey, "-n", (int) i );

		if( dwkey == dwRoomKey )
			return i;
	}

	return -1;
}

void GLGroupChat::ChatRoomClear()
{
	m_mapChatRoom.clear();
}

//
//mjeon.groupchat
// LinkMan: item link�� ���Ե� ä�� �޽����� link������ ���� ���ȴ�.
//
LinkMan::LinkMan()
{
	m_pRegexTag = new boost::regex;

	// �ʿ��ϴٸ� tag�� ������ �� �� �����ϰ� �ؼ� �Ϲ� �������� �����Է��� ������ �� �ִ�.
	// ���� 1�������� UI�ܿ��� �ش� �Է��� �����ؾ� �Ѵ�.
	std::string strExpression("<item[0-9]+>");	//  <item10>

	try
	{
		m_pRegexTag->assign(strExpression);	//��ҹ��� ����
	}
	catch (boost::regex_error& e)
	{
		std::string ErrMsg = stringformat("Regular expression(%1%) is wrong", e.what());
		sc::writeLogError(ErrMsg);
	}
}

LinkMan::~LinkMan()
{
	delete m_pRegexTag;
}

//network msg ���ۿ� ����� ������ tag����
// "<itemN>" ���� N�� ���ٸ� �ǹ̸� ���� �ʴ´�.
std::string LinkMan::GetFreeTag()
{
	return std::string("<item0>");
	//return (std::string("<item") + boost::lexical_cast<std::string>(m_vecLinks.size()) + std::string(">"));
}

BOOL LinkMan::IsTag(std::string tag)
{
	return boost::regex_match(tag, *m_pRegexTag);
}

DWORD /*idx*/ LinkMan::Register(IN SITEMCUSTOM &item, OUT LINKPAIR& linkpair)
{	
	m_vecLinks.push_back(item);

	linkpair.idx = m_vecLinks.size() - 1;
	linkpair.name = GLogicData::GetInstance().GetItemName(item.Id());
	
	return linkpair.idx;
}

BOOL LinkMan::GetItem(DWORD idx, OUT SITEMCUSTOM& item)
{
	if (m_vecLinks.size() <= idx)
	{
		sc::writeLogError(stringformat("LinkMan::GetItem - %1% is not valid index", idx));
		return FALSE;
	}

	item = m_vecLinks[idx];

	return TRUE;
}

BOOL LinkMan::GetItemName(DWORD idx, OUT std::string& itemName, BOOL bWithBracket)
{
	if (m_vecLinks.size() <= idx)
	{
		sc::writeLogError(stringformat("LinkMan::GetItemName - %1% is not valid index", idx));
		return FALSE;
	}

	itemName = GLogicData::GetInstance().GetItemName(m_vecLinks[idx].Id());

	if (bWithBracket)
	{
		itemName = "<" + itemName + ">";
	}

	return TRUE;
}

DWORD LinkMan::GetIndex(std::string& tag)
{
	//<itemN> : N is the index
	//  ex) <item5>

	if (IsTag(tag) == FALSE)
		return WRONG_LINK_IDX;

	size_t from	= tag.find("<item");
	size_t to	= tag.find(">");
	std::string strIDX = tag.substr(from, to - from);

	DWORD idx = boost::lexical_cast<DWORD>(strIDX);

	return idx;
}

//
//<tag>�� ������ ������ ���� <itemname>�� ������ �������� ����
//  ex) "<item0> ��ϴ�" -> "<Ȱ> ��ϴ�"
std::string LinkMan::Translate(const std::string& IN source)
{
	m_csTranslateBuffer.SetString(source.c_str());

	boost::regex_grep(grep_predicate(this) /*a copy of pred is used inside, not a reference*/, source, *m_pRegexTag);

	return m_csTranslateBuffer.GetString();
}

bool grep_predicate::operator() (const boost::smatch &m)
{
	std::string tag = m.str(0);

	DWORD idx = m_pLinkMan->GetIndex(tag);

	if (idx == WRONG_LINK_IDX)
		return true;

	SITEMCUSTOM item;
	BOOL bRet = m_pLinkMan->GetItem(idx, item);

	std::string tagNew;
	std::string itemName;

	if (bRet)
	{
		itemName = GLogicData::GetInstance().GetItemName(item.Id());
	}
	else
	{
		itemName = UNKNOWN_ITEM_NAME;
	}

	tagNew = "<" + itemName + ">";

	CString& csBuffer = m_pLinkMan->GetTranslateBuffer();
	csBuffer.Replace(tag.c_str(), tagNew.c_str());
	
	return true;
}

bool GLGroupChat::ReqGroupChat( const char* szName, int nType )
{
	DWORD dwDBNum = -1;
	switch(nType)
	{
	case GROUPCHAT_INVITE_INTERACT :
		{
			dwDBNum = m_pGaeaClient->FindChaDbNumAtCache( szName );
		}
		break;

	case GROUPCHAT_INVITE_FRIEND :
		{
			dwDBNum = m_pGaeaClient->GetFriendClient()->GetFriendDBNum( szName );
		}
		break;

	case GROUPCHAT_INVITE_CLUBMEMBER :
		{
			dwDBNum = m_pGaeaClient->GetMyClub()->GetMemberDBNum( szName );
		}
		break;
	}

	if( 0 >= dwDBNum )
		return false;

	GLMSG::SNET_GCHAT_INVITE_CAC sNetMsg;
	sNetMsg.dwChaNumWhom = dwDBNum;
	m_pGaeaClient->NETSENDTOAGENT( &sNetMsg );

	return true;
}

void GLGroupChat::ReqGroupChatFB( bool bAccept )
{
	GLMSG::SNET_GCHAT_INVITE_CAC_FB NetMsg;
	NetMsg.bAccept = true == bAccept ? TRUE : FALSE;
	NetMsg.dwKey = m_dwKey;
	NetMsg.dwChaWhom = m_pGaeaClient->GetCharacter()->GetCharID();
	m_pGaeaClient->NETSENDTOAGENT( &NetMsg );
}

void GLGroupChat::ReqQuit( DWORD dwKey )
{
	MAP_CHATROOMCLIENT_ITER iter = m_mapChatRoom.find( dwKey );
	if( iter == m_mapChatRoom.end() )
		return;

	GLMSG::SNET_GCHAT_QUIT_CA msg;
	msg.dwKey = dwKey;

	m_pGaeaClient->NETSENDTOAGENT( &msg );
}

LuaTable GLGroupChat::GetMemberForWidget( DWORD dwKey )
{
	LuaTable tbMember( GLWidgetScript::GetInstance().GetLuaState() );

	MAP_CHATROOMCLIENT_ITER iter = m_mapChatRoom.find( dwKey );
	if( iter == m_mapChatRoom.end() )
		return tbMember;

	std::vector< CString > vecName;

	ChatRoomClient& sRoom = iter->second;
	sRoom.GetMembername( vecName );

	for( size_t i = 0; i < vecName.size(); ++i )
		tbMember.set( i + 1, vecName[ i ].GetBuffer() );

	return tbMember;
}

void GLGroupChat::SendChat( DWORD dwKey, const char* szMsg )
{	
	MAP_CHATROOMCLIENT_ITER iter = m_mapChatRoom.find( dwKey );
	if( iter == m_mapChatRoom.end() )
		return;

	CString strTemp;
	strTemp = szMsg;
	strTemp.TrimLeft ( _T(" ") ); // ���� ���� ����

	// ������ ���͸�
	if( TRUE == CRanFilter::GetInstance().ChatFilter( strTemp ) )
		strTemp = CRanFilter::GetInstance().GetProverb();

	GLMSG::SNET_GCHAT_MESSAGE_CAC msg;
	wsprintf( msg.szChat, "%s", strTemp );
	msg.dwChaSay = m_pGaeaClient->GetCharacter()->CharDbNum();
	msg.dwKey = dwKey;

	const std::list< SITEMCUSTOM >& listLinkItem = iter->second.GetLinkItemList();

	std::list< SITEMCUSTOM >::const_iterator iterLink = listLinkItem.begin();
	for( int i = 0; iterLink != listLinkItem.end(); ++iterLink, ++i )
	{
		if( HOW_MANY_ITEM_LINK_PER_CHAT_MSG > i )
			msg.AddLink( const_cast< SITEMCUSTOM& >( (*iterLink) ) );
	}
	
	m_pGaeaClient->NETSENDTOAGENT( &msg );

	// ��ũ ������ Ŭ����
	iter->second.ClearLinkItem();
}

bool GLGroupChat::AddLinkItem( DWORD dwKey, const SITEMCUSTOM& sItem )
{
	MAP_CHATROOMCLIENT_ITER iter = m_mapChatRoom.find( dwKey );
	if( iter == m_mapChatRoom.end() )
		return false;

	return iter->second.AddLinkItem( sItem );
}

void GLGroupChat::RemoveLinkItem( DWORD dwKey, int nIndex )
{
	MAP_CHATROOMCLIENT_ITER iter = m_mapChatRoom.find( dwKey );
	if( iter == m_mapChatRoom.end() )
		return;

	iter->second.RemoveLinkItem( nIndex );
}

LuaTable GLGroupChat::GetLinkItemInfo( DWORD dwKey )
{
	LuaTable tbLinkItem( GLWidgetScript::GetInstance().GetLuaState() );

	MAP_CHATROOMCLIENT_ITER iter = m_mapChatRoom.find( dwKey );
	if( iter == m_mapChatRoom.end() )
		return tbLinkItem;

	const std::list< SITEMCUSTOM >& listLinkItem = iter->second.GetLinkItemList();

	int nTableIndex = 1;
	
	std::list< SITEMCUSTOM >::const_iterator iterLink = listLinkItem.begin();
	for( ; iterLink != listLinkItem.end(); ++iterLink )
	{
		const SITEMCUSTOM& sItem = (*iterLink);

		SITEM* pItem = GLogicData::GetInstance().GetItem( sItem.GetNativeID() );
		if( NULL == pItem )
			continue;

		DWORD dwColor = ITEM_INFOR_COLOR::dwItemRank[ pItem->sBasicOp.emLevel ];
		
		LuaTable tbItem( GLWidgetScript::GetInstance().GetLuaState() );
		tbItem.set( 1, sItem.GETNAME().c_str() );
		tbItem.set( 2, dwColor );
		
		tbLinkItem.set( nTableIndex++, tbItem );
	}

	return tbLinkItem;
}

void GLGroupChat::ReqKick( const char* szName )
{
	GLMSG::SNET_GCHAT_KICK_CA msg;
	msg.dwChaNum = m_pGaeaClient->FindChaDbNumAtCache( szName );
	m_pGaeaClient->NETSENDTOAGENT( &msg );
}