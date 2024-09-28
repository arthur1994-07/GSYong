#include "pch.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../RanLogic/Msg/FriendMsg.h"

#include "./Char/GLCharacter.h"
#include "./Club/GLClubClient.h"
#include "./Friend/GLFriendClient.h"
#include "./GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

bool GLGaeaClient::FriendAddGroup(const FRIEND_GROUP& Info)
{
    return m_pFriendClient->GroupAdd(Info);
}

bool GLGaeaClient::FriendDelGroup(WORD GroupNum)
{
    return m_pFriendClient->GroupDel(GroupNum);
}

bool GLGaeaClient::FriendIsExistGroup(WORD GroupNum) const
{
    return m_pFriendClient->GroupIsExist(GroupNum);
}

bool GLGaeaClient::FriendIsExistGroup(const std::string& GroupName) const
{
    return m_pFriendClient->GroupIsExist(GroupName);
}

WORD GLGaeaClient::FriendGetFreeGroupNum() const
{
    return m_pFriendClient->GroupGetFreeNum();
}

std::string GLGaeaClient::FriendGetGroupName(WORD GroupNum) const
{
    return m_pFriendClient->GroupGetName(GroupNum);
}

void GLGaeaClient::FriendGroup(std::vector<FRIEND_GROUP>& GroupInfo) const
{
    m_pFriendClient->GetGroup(GroupInfo);
}

bool GLGaeaClient::FriendGroupNewCA(const std::string& GroupName)
{
    if (FriendIsExistGroup(GroupName))
        return false; // 이미 존재함
    
    // 서버로 요청
    GLMSG::NET_FRIEND_GROUP_NEW_CA MsgCA(GroupName);
    NETSENDTOAGENT(&MsgCA);
    
    return true;
}

void GLGaeaClient::MsgFriendGroupNewAC(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_FRIEND_GROUP_NEW_AC* pPacket = (GLMSG::NET_FRIEND_GROUP_NEW_AC*) pMsg;
    FRIEND_GROUP Data(pPacket->m_GroupNum, pPacket->m_GroupName);
    m_pFriendClient->GroupAdd(Data);
    
    // UI Refresh
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Community_AddGroup, "-n", pPacket->m_GroupNum );
}

bool GLGaeaClient::FriendGroupDelReq(WORD GroupNum)
{
    if (GroupNum == 0)
        return false; // 0 번은 기본값이라 지울수 없음

    if (!FriendIsExistGroup(GroupNum))
        return false; // 존재하지 않음

    GLMSG::NET_FRIEND_GROUP_DEL_CA MsgCA(GroupNum);
    NETSENDTOAGENT(&MsgCA);

    return true;
}

void GLGaeaClient::MsgFriendGroupDelAC(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_FRIEND_GROUP_DEL_AC* pPacket = (GLMSG::NET_FRIEND_GROUP_DEL_AC*) pMsg;
    m_pFriendClient->GroupDel(pPacket->m_GroupNum);
    
    // UI Refresh
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Community_RemoveGroup, "-n", pPacket->m_GroupNum );
}

bool GLGaeaClient::FriendGroupSetFriend(DWORD FriendDbNum, WORD Group)
{
    if (!m_pFriendClient->GetData(FriendDbNum))
        return false; // 존재하지 않음

    if (Group != 0 && !m_pFriendClient->GroupIsExist(Group))
        return false; // 존재하지 않는 그룹

    GLMSG::NET_FRIEND_SET_GROUP_CA MsgCA(FriendDbNum, Group);
    NETSENDTOAGENT(&MsgCA);

    return true;
}

void GLGaeaClient::MsgFriendSetGroupAC(NET_MSG_GENERIC* pMsg)
{
	WORD wBeforeGroupNum(-1);
    GLMSG::NET_FRIEND_SET_GROUP_AC* pPacket = (GLMSG::NET_FRIEND_SET_GROUP_AC*) pMsg;
    m_pFriendClient->GroupSetFriend(pPacket->m_FriendDbNum, pPacket->m_GroupNum, wBeforeGroupNum);
    
    // UI Refresh
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Community_FriendSetGroup, "-n, -n", wBeforeGroupNum, pPacket->m_GroupNum );
}

bool GLGaeaClient::FriendRenameGroup(WORD GroupNum, const std::string& NewName)
{
    if (GroupNum == 0)
        return false;
    
    std::string OldName = m_pFriendClient->GroupGetName(GroupNum);
    if (OldName == NewName)
        return false;

	std::vector< FRIEND_GROUP > vecGroup;
	FriendGroup( vecGroup );

	std::vector< FRIEND_GROUP >::iterator iter = vecGroup.begin();
	for( ; iter != vecGroup.end(); ++iter )
	{
		const FRIEND_GROUP& sGroup = (*iter);

		if( GroupNum == sGroup.m_GroupNum )
			continue;

		if( 0 == strcmp( NewName.c_str(), sGroup.m_GroupName ) )
			return false;
	}

    GLMSG::NET_FRIEND_GROUP_NAME_CHANGE_CA MsgCA(GroupNum, NewName);
    NETSENDTOAGENT(&MsgCA);

    return true;
}

void GLGaeaClient::MsgFriendGroupNameChangeAC(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_FRIEND_GROUP_NAME_CHANGE_AC* pPacket = (GLMSG::NET_FRIEND_GROUP_NAME_CHANGE_AC*) pMsg;
    m_pFriendClient->GroupRename(pPacket->m_GroupNum, pPacket->m_GroupName);
    
    // UI Refresh
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_RnCommunity_FriendTab_Update);
}

void GLGaeaClient::MsgFriendGroupReqAC(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_FRIEND_GROUP_REQ_AC* pPacket = (GLMSG::NET_FRIEND_GROUP_REQ_AC*) pMsg;    
    m_pFriendClient->GroupAdd(pPacket->m_Group);
    
    // UI Refresh
	// GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Community_FriendStateUpdate );
}

void GLGaeaClient::MsgOtherCharLevelChangeAC(NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    GLMSG::NET_OTHER_CHAR_LEVEL_CHANGE_AC RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;
    m_pFriendClient->SetFriendLevel(RecvData.m_CharDbNum, RecvData.m_ChaLevel);
    m_pMyClub->SetMemberState(RecvData.m_CharDbNum, RecvData.m_ChaLevel);

	std::tr1::shared_ptr<SFRIEND> spFriend = m_pFriendClient->GetData(RecvData.m_CharDbNum);
	// UI Refresh
	if ( spFriend )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Community_FriendStateUpdate, "-n", spFriend->GetGroup() );
	}

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Club_UpdateMember );
}

void GLGaeaClient::MsgFriendMapChangeAC(NET_MSG_GENERIC* pMsg)
{
    // NET_MSG_FRIEND_MAP_CHANGE_AC
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    GLMSG::NET_MAP_CHANGE_FRIEND_CLUB_AC RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;
    m_pFriendClient->SetFriendMap(RecvData.m_ChaDbNum, RecvData.m_MapID);

    m_pMyClub->SetMemberState(RecvData.m_ChaDbNum, RecvData.m_MapID);

	std::tr1::shared_ptr<SFRIEND> spFriend = m_pFriendClient->GetData(RecvData.m_ChaDbNum);
	// UI Refresh
	if ( spFriend )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Community_FriendStateUpdate, "-n", spFriend->GetGroup() );
	}

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Club_UpdateMember );
}

void GLGaeaClient::MsgCharOnlineAC(NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    GLMSG::NET_CHAR_STATE_ONLINE_AC RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

    // 친구 상태 업데이트
    m_pFriendClient->SetFriendState(
        RecvData.m_ChaDbNum,
        RecvData.m_Channel,
        RecvData.m_CurrentMap,
        int(RecvData.m_fPositionX),
        int(RecvData.m_fPositionZ));

    // 클럽 상태 업데이트
    m_pMyClub->SetMemberState(RecvData.m_ChaDbNum, true, RecvData.m_Channel, RecvData.m_CurrentMap);
    m_pMyClub->SetMemberPos(RecvData.m_ChaDbNum, D3DXVECTOR3(RecvData.m_fPositionX, RecvData.m_fPositionY, RecvData.m_fPositionZ));

	std::tr1::shared_ptr<SFRIEND> spFriend = m_pFriendClient->GetData(RecvData.m_ChaDbNum);

	// Online message
    std::string ChaName;
    bool bDisplay = false;
    if (spFriend)
    {
        EMFRIEND_FLAGS Flag = spFriend->GetFlag();
        if (Flag == EMFRIEND_VALID)
        {
			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Community_FriendStateUpdate, "-n", spFriend->GetGroup() );
            ChaName = spFriend->GetName();
            bDisplay = true;
        }
    }
    
    const GLCLUBMEMBER* pClubMember = m_pMyClub->GetMember(RecvData.m_ChaDbNum);
    if (pClubMember)
    {
        ChaName = pClubMember->GetNameStr();
        bDisplay = true;

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_Club_UpdateMember );
    }

    if (RecvData.m_ChaDbNum == m_pCharacter->CharDbNum())
        bDisplay = false;

    if (bDisplay)
    {
        std::string Msg(
            sc::string::format(
                ID2GAMEINTEXT("FRIEND_CLUB_CHAR_ONLINE"), ChaName));
        PrintMsgText(NS_UITEXTCOLOR::GOLD, Msg);
    }
}

void GLGaeaClient::MsgCharOfflineAC(NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    GLMSG::NET_CHAR_STATE_OFFLINE_AC RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

    // 친구 상태 업데이트
    m_pFriendClient->SetFriendOffline(RecvData.m_ChaDbNum);

    // 클럽 상태 업데이트
    if( true == m_pMyClub->SetMemberOffline(RecvData.m_ChaDbNum) )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_Club_UpdateMember );
	}

	std::tr1::shared_ptr<SFRIEND> spFriend = m_pFriendClient->GetData(RecvData.m_ChaDbNum);
	// UI Refresh
	if ( spFriend )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_Community_FriendStateUpdate, 
			"-n", spFriend->GetGroup() );
	}
}

void GLGaeaClient::MsgCharPositionAC(NET_MSG_GENERIC* pMsg)
{    
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    GLMSG::NET_CHAR_POSITION_AC RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

    // 친구 상태 업데이트
    m_pFriendClient->SetFriendMapPosition(RecvData.m_ChaDbNum, RecvData.m_PosX, RecvData.m_PosY);

    // 클럽 상태 업데이트    
    const GLCLUBMEMBER* pMember = m_pMyClub->GetMember(RecvData.m_ChaDbNum);
    if (pMember)
    {
        D3DXVECTOR3 ChaPos(0.0f, 0.0f, 0.0f);
        ConvertMapToWorld(pMember->m_CurrentMap, RecvData.m_PosX, RecvData.m_PosY, ChaPos);
        m_pMyClub->SetMemberPos(RecvData.m_ChaDbNum, ChaPos);

		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Club_UpdateMember );
    }

	std::tr1::shared_ptr<SFRIEND> spFriend = m_pFriendClient->GetData(RecvData.m_ChaDbNum);
	// UI Refresh
	if ( spFriend )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_Community_FriendStateUpdate, 
			"-n", spFriend->GetGroup() );
	}
}

bool GLGaeaClient::GetFriendState(DWORD MainChaDbNum, DWORD SubChaDbNum, EMFRIEND_FLAGS& State)
{
    if (m_pCharacter->CharDbNum() == MainChaDbNum)
    {        
        std::tr1::shared_ptr<SFRIEND> spFriend = m_pFriendClient->GetData(SubChaDbNum);
        if (spFriend)
        {
            // 나의 친구
            State = spFriend->GetFlag();
        }
        else
        {
            // 친구가 아님
            State = EMFRIEND_OFF;
        }
        return true;
    }

    // 서버에 요청
    GLMSG::NET_FRIEND_STATE_REQ_CA Data(MainChaDbNum, SubChaDbNum);
    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, Data);
    NETSENDTOAGENT(NET_MSG_FRIEND_STATE_REQ_CA, SendBuffer);
    return false;
}

void GLGaeaClient::MsgFriendStateReqAC(NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    GLMSG::NET_FRIEND_STATE_REQ_AC Data;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), Data))
        return;
    Data.m_MainChar;
    Data.m_SubChar;
    Data.m_State; // EMFRIEND_FLAGS
}

void GLGaeaClient::MsgOtherCharPartyChangeAC(NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    GLMSG::NET_OTHER_CHAR_PARTY_CHANGE_AC RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;
    m_pFriendClient->SetFriendParty(RecvData.m_ChaDbNum, RecvData.m_PartyNum);

	std::tr1::shared_ptr<SFRIEND> spFriend = m_pFriendClient->GetData(RecvData.m_ChaDbNum);
	// UI Refresh
	if ( spFriend )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Community_FriendStateUpdate, "-n", spFriend->GetGroup() );
	}
}