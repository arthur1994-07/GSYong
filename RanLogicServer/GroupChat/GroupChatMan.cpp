#include "pch.h"
#include "./GroupChatMan.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/gassert.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/GLogicFile.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogicServer/AgentServer/GLAgentServer.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
//							Chatting Room
//////////////////////////////////////////////////////////////////////////

ChatRoom::ChatRoom(ChatManager *pChatMan, DWORD dwKey, DWORD dwMaxTO)
:m_pChatMan(pChatMan)
,m_dwKey(dwKey)
{
	GASSERT(m_pChatMan != NULL);
}

ChatRoom::ChatRoom()
:m_pChatMan(NULL)
,m_dwKey(INVALID_CHAT_KEY)
,m_dwMaxTO(0)
{
}

ChatRoom::~ChatRoom()
{
}

BOOL ChatRoom::Invite(DWORD dwChaNum)
{
	GASSERT(dwChaNum != INVALID_CHANUM);


	MAP_CHAT_MEMBER_ITER iterMember = m_mapMembers.find(dwChaNum);
	if (iterMember != m_mapMembers.end())
	{
		sc::writeLogDebug(stringformat("ChatRoom::Invite() - The Char(%1%) is already in the room(%2%).", dwChaNum, m_dwKey));
		return FALSE;
	}

	//이중 초대는 문제될 것이 없다.
/*
	SET_CHAT_INVITED_ITER iter = m_setInvited.find(dwChaNum);

	if (iter != m_setInvited.end())
	{
		sc::writeLogWarn(stringformat("ChatRoom::Invite() - The Char(%1%) is already invited to the room(%2%).", dwChaNum, m_dwKey));
		return TRUE;
	}
*/
	m_setInvited.insert(dwChaNum);

	return TRUE;
}

DWORD _KEY_ ChatRoom::Join(DWORD dwChaNum)
{
	GASSERT(dwChaNum != INVALID_CHANUM);

	if (dwChaNum != m_dwKey)
	{
		//is this character invited to this chat?
		SET_CHAT_INVITED_ITER iterInvited = m_setInvited.find(dwChaNum);

		if (iterInvited == m_setInvited.end())
		{
			sc::writeLogDebug(stringformat("ChatRoom::Join() - The Char(%1%) is not invited to the room(%2%).", dwChaNum, m_dwKey));

			return INVALID_CHAT_KEY;
		}
		
		m_setInvited.erase(iterInvited);
	}

	MAP_CHAT_MEMBER_ITER iter = m_mapMembers.find(dwChaNum);

	if (iter != m_mapMembers.end())
	{
		sc::writeLogDebug(stringformat("ChatRoom::Join() - The Char(%1%) is already in the room(%2%).", dwChaNum, m_dwKey));

		return m_dwKey;	//발생할 수 있는 상황이다
	}

	std::string strName = m_pChatMan->GetChaName(dwChaNum);

	m_mapMembers.insert(MAP_CHAT_MEMBER_VALUE(dwChaNum, CHAT_MEMBER(dwChaNum, strName)));

	m_pChatMan->UpdateJoinSet(dwChaNum, m_dwKey);
	
	GLMSG::SNET_GCHAT_MEMBER_SYNC_AC msgSync;	//채팅에 새롭게 참여(join)하는 캐릭터에게 방의 멤버 상태를 동기화
	msgSync.dwKey = m_dwKey;

	iter = m_mapMembers.begin();
	for (; iter != m_mapMembers.end();)
	{
		if (msgSync.AddData(iter->second) == TRUE)
		{
			++iter;
		}
		else
		{
			m_pChatMan->SendToClient(dwChaNum, &msgSync);

			msgSync.Reset();
			msgSync.dwKey = m_dwKey;
		}
	}

	if (msgSync.nSync > 0)	//마지막 동기화 메시지 전송
	{
		m_pChatMan->SendToClient(dwChaNum, &msgSync);
	}
	
	GLMSG::SNET_GCHAT_MEMBER_UPDATE_AC msg;
	msg.dwKey = m_dwKey;
	msg.bJoin = TRUE;
	msg.who = CHAT_MEMBER(dwChaNum, strName);

	SendToRoom(&msg);

	return m_dwKey;
}

DWORD _KEY_ ChatRoom::Quit(DWORD dwChaNum)
{
	GASSERT(dwChaNum != INVALID_CHANUM);
	MAP_CHAT_MEMBER_ITER iter = m_mapMembers.find(dwChaNum);

	if (iter == m_mapMembers.end())
	{
		sc::writeLogDebug(stringformat("ChatRoom::Quit() - The Char(%1%) is not in the room(%2%).", dwChaNum, m_dwKey));

		return INVALID_CHAT_KEY;
	}
	
	GLMSG::SNET_GCHAT_MEMBER_UPDATE_AC msg;
	msg.dwKey = m_dwKey;
	msg.bJoin = FALSE;
	msg.who = CHAT_MEMBER(dwChaNum, m_pChatMan->GetChaName(dwChaNum));

	SendToRoom(&msg);	//나가는 장본인도 포함해서 알려준다

	m_mapMembers.erase(iter);

	return m_dwKey;
}

void ChatRoom::CheckOffline()
{
	MAP_CHAT_MEMBER_ITER iter = m_mapMembers.begin();
	GLCharAG *pChar = NULL;

	for (; iter != m_mapMembers.end();)
	{
		if (m_pChatMan->IsOffline(iter->first))
		{
			GLMSG::SNET_GCHAT_MEMBER_UPDATE_AC msg;
			msg.dwKey = m_dwKey;
			msg.bJoin = FALSE;
			msg.who = CHAT_MEMBER(iter->first, m_pChatMan->GetChaName(iter->first));

			SendToRoom(&msg);

			iter = m_mapMembers.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void ChatRoom::SendToRoom(NET_MSG_GENERIC *nmg)
{
	MAP_CHAT_MEMBER_ITER iter = m_mapMembers.begin();
	
	for (; iter != m_mapMembers.end(); ++iter)
	{
		m_pChatMan->SendToClient(iter->first, nmg);
	}
}

DWORD	ChatRoom::GetMemberCount()
{
	return m_mapMembers.size();
}

//////////////////////////////////////////////////////////////////////////
//							Chatting Manager
//////////////////////////////////////////////////////////////////////////

ChatManager::ChatManager(GLAgentServer *pGLAgentServer)
:m_pGLAgentServer(pGLAgentServer)
,m_dwMaxOpenChat(0)
,m_dwMaxTO(0)
,m_dwRequiredLevel(0)
,m_dwLogTime(0)
,m_dwHourLog(60*60*1000)
,m_dwLogPeople(0)
{
	GASSERT(pGLAgentServer != NULL);

	m_pScriptMan = new ChatScriptMan();
}

ChatManager::~ChatManager()
{
	SAFE_DELETE( m_pScriptMan );
}

BOOL ChatManager::Initialize()
{
	if (!GLUseFeatures::GetInstance().IsUsingGroupChat())
		return FALSE;
	//
	//Load Script
	//
	std::string strScriptPath(sc::getAppPath());
	strScriptPath.append("\\Data\\GLogicServer\\");
	strScriptPath.append("scripts\\");

	std::string strGroupChatScriptPath(strScriptPath);    
    strGroupChatScriptPath.append(GLogicData::GetInstance().GetGroupChatScriptName());

	m_pScriptMan->LoadScript(strGroupChatScriptPath.c_str());
	
	if (!m_pScriptMan->IsLoaded())
	{
		sc::writeLogError(std::string("ChatManager::Initialize() failure. Script MUST be loaded to run this feature."));

		return FALSE;
	}

	//
	//initialize Configurations
	//
	m_dwMaxOpenChat	= m_pScriptMan->GetConfigUINT("MaxOpenChat");
	m_dwMaxTO		= m_pScriptMan->GetConfigUINT("MaxTO");
	m_dwRequiredLevel	= m_pScriptMan->GetConfigUINT("RequiredLevel");

	return TRUE;
}

void ChatManager::FrameMove()
{
	if (!GLUseFeatures::GetInstance().IsUsingGroupChat())
		return;

	//
	//handle empty rooms & offline characters
	//
	MAP_CHAT_ROOM_ITER iter = m_mapRooms.begin();

	for (; iter != m_mapRooms.end();)
	{
		ChatRoom &room = iter->second;

		room.CheckOffline();	//offline 캐릭터 처리

		if (room.IsEmpty())
		{
			iter = m_mapRooms.erase(iter);
		}
		else
		{
			//m_dwLogPeople += room.GetMemberCount();
			++iter;
		}
	}
	
	//
	//update join state
	//
	MAP_JOINED_ROOM_ITER iterJoined = m_mapCharJoinState.begin();

	for (; iterJoined != m_mapCharJoinState.end();)
	{
		if (IsOffline(iterJoined->first))
		{
			iterJoined = m_mapCharJoinState.erase(iterJoined);
		}
		else
		{
			++iterJoined;
		}
	}

	if(m_dwLogTime == 0)
	{
		m_dwLogTime = GetTickCount();
	}

	if(m_dwHourLog <= GetTickCount() - m_dwLogTime)	
	{
		MAP_CHAT_ROOM_ITER iter = m_mapRooms.begin();

		for (; iter != m_mapRooms.end(); ++iter)
		{

			ChatRoom &room = iter->second;

			room.CheckOffline();	//offline 캐릭터 처리

			if (!room.IsEmpty())
			{
				m_dwLogPeople += room.GetMemberCount();
			}		
		}

		sc::writeLogInfo(sc::string::format("GroupChat: Room Count: %1%, Room People: %2%", m_mapRooms.size(), m_dwLogPeople));
		m_dwLogTime = GetTickCount();
		m_dwLogPeople = 0;
	}
	else if(GetTickCount() - m_dwLogTime < 0)
	{
		m_dwLogTime = GetTickCount();
	}
}

BOOL ChatManager::Invite(DWORD dwChaOwner, DWORD dwChaWho)
{
	if (!GLUseFeatures::GetInstance().IsUsingGroupChat())
		return FALSE;

	GASSERT(dwChaOwner != INVALID_CHANUM);
	GASSERT(dwChaWho != INVALID_CHANUM);
	if (dwChaOwner == INVALID_CHANUM || dwChaWho == INVALID_CHANUM)
		return FALSE;
	
	//1. Owner accepts it's room
	DWORD dwKey = Accept(dwChaOwner, dwChaOwner);	//owner character creates it's own room if it does not exist

	if (dwKey == INVALID_CHAT_KEY)
	{
		sc::writeLogError(stringformat("ChatManager::Invite() - Owner(%1%) cannot join the room.", dwChaOwner));

		GLMSG::SNET_GCHAT_ERRORCODE msg;
		msg.dwKey = dwChaOwner;
		msg.code  = CHAT_CODE_ROOM_NOT_CONSTRUCT;
		SendToClient(dwChaOwner, &msg);

		return FALSE;
	}

	MAP_CHAT_ROOM_ITER iter = m_mapRooms.find(dwChaOwner);	//find again

	if (iter == m_mapRooms.end())
	{
		sc::writeLogError(stringformat("ChatManager::Invite() - Owner(%1%) couldn't construct a room.", dwChaOwner));
		GLMSG::SNET_GCHAT_ERRORCODE msg;
		msg.dwKey = dwChaOwner;
		msg.code  = CHAT_CODE_ROOM_NOT_CONSTRUCT;
		SendToClient(dwChaOwner, &msg);

		return FALSE;
	}

	ChatRoom &room = iter->second;

	if (room.size() >= m_dwMaxTO)
	{
		GLMSG::SNET_GCHAT_ERRORCODE msg;
		msg.dwKey = dwChaOwner;
		msg.code  = CHAT_CODE_MAX_MEMBERS;
		SendToClient(dwChaOwner, &msg);

		return FALSE;
	}

	return room.Invite(dwChaWho);
}


BOOL ChatManager::ConstructRoom(DWORD dwChaNum)
{
	if (!GLUseFeatures::GetInstance().IsUsingGroupChat())
		return FALSE;

	GASSERT(dwChaNum != INVALID_CHANUM);
	if (dwChaNum == INVALID_CHANUM)
		return FALSE;

	MAP_CHAT_ROOM_ITER iter = m_mapRooms.find(dwChaNum);

	if (iter != m_mapRooms.end())
	{
		return FALSE;	//already exist
	}

	m_mapRooms.insert(MAP_CHAT_ROOM_VALUE(dwChaNum, ChatRoom(this, dwChaNum, m_dwMaxTO)));

	return TRUE;
}

DWORD _KEY_ ChatManager::Accept(DWORD dwChaNum, DWORD dwKey)
{
	if (!GLUseFeatures::GetInstance().IsUsingGroupChat())
		return INVALID_CHAT_KEY;

	GASSERT(dwChaNum != INVALID_CHANUM);
	GASSERT(dwKey != INVALID_CHANUM);
	if (dwChaNum == INVALID_CHANUM || dwKey == INVALID_CHAT_KEY)
		return INVALID_CHAT_KEY;

	if (dwChaNum != dwKey)
	{
		MAP_JOINED_ROOM_ITER iter = m_mapCharJoinState.find(dwChaNum);
		if (iter != m_mapCharJoinState.end())
		{
			SET_JOINED_ROOM &setJoinedRooms = iter->second;

			DWORD dwNumJoined = setJoinedRooms.size() - setJoinedRooms.count(dwChaNum);	//자기 소유의 방은 카운트하지 않는다.

			GASSERT(dwNumJoined <= m_dwMaxOpenChat);

			if (dwNumJoined >= m_dwMaxOpenChat)
			{
				GLMSG::SNET_GCHAT_ERRORCODE msg;
				msg.dwKey = dwKey;
				msg.code  = CHAT_CODE_MAX_OPEN_CHAT;
				SendToClient(dwChaNum, &msg);

				return INVALID_CHAT_KEY;
			}
		}
	}

	MAP_CHAT_ROOM_ITER iterRoom = m_mapRooms.find(dwKey);
	if (iterRoom == m_mapRooms.end())
	{
		if (dwChaNum == dwKey)
		{
			ConstructRoom(dwChaNum);

			iterRoom = m_mapRooms.find(dwKey);	//search again

			if (iterRoom == m_mapRooms.end())
			{
				sc::writeLogError(stringformat("ChatManager::Accept - couldn't construct a room(%1%)", dwKey));
				return INVALID_CHAT_KEY;
			}
		}
		else
		{
			GLMSG::SNET_GCHAT_ERRORCODE msg;
			msg.dwKey = dwKey;
			msg.code  = CHAT_CODE_ROOM_NOT_EXIST;
			SendToClient(dwChaNum, &msg);

			return INVALID_CHAT_KEY;
		}
	}

	ChatRoom &room = iterRoom->second;

	if (dwChaNum != dwKey && room.size() >= m_dwMaxTO)
	{
		GLMSG::SNET_GCHAT_ERRORCODE msg;
		msg.dwKey = dwKey;
		msg.code  = CHAT_CODE_MAX_MEMBERS;
		SendToClient(dwChaNum, &msg);

		return INVALID_CHAT_KEY;
	}

	return room.Join(dwChaNum);
}

DWORD _KEY_ ChatManager::Quit(DWORD dwChaNum, DWORD dwKey)
{
	if (!GLUseFeatures::GetInstance().IsUsingGroupChat())
		return INVALID_CHAT_KEY;

	GASSERT(dwChaNum != INVALID_CHANUM);
	GASSERT(dwKey != INVALID_CHANUM);
	if (dwChaNum == INVALID_CHANUM || dwKey == INVALID_CHAT_KEY)
		return INVALID_CHAT_KEY;

	MAP_JOINED_ROOM_ITER iter = m_mapCharJoinState.find(dwChaNum);
	if (iter != m_mapCharJoinState.end())
	{
		SET_JOINED_ROOM &setJoinedRooms = iter->second;

		setJoinedRooms.erase(dwKey);

		if (setJoinedRooms.empty())
		{
			m_mapCharJoinState.erase(iter);
		}
	}

	MAP_CHAT_ROOM_ITER iterRoom = m_mapRooms.find(dwKey);
	if (iterRoom == m_mapRooms.end())
	{
		//cannot quit. the room does not exist
		return INVALID_CHAT_KEY;
	}
	
	ChatRoom &room = iterRoom->second;

	return room.Quit(dwChaNum);
}

DWORD _KEY_ ChatManager::KickOut(DWORD dwChaNum, DWORD dwKey)
{
	return Quit(dwChaNum, dwKey);
}

BOOL ChatManager::IsOffline(DWORD dwChaNum)
{
	GLCharAG *pChar = m_pGLAgentServer->GetCharByDbNum(dwChaNum);

	return ( pChar == NULL );
}

/*

void ChatManager::QuitAllRooms(DWORD dwChaNum)
{
	GASSERT(dwChaNum != INVALID_CHANUM);
	if (dwChaNum == INVALID_CHANUM)
		return INVALID_CHAT_KEY;

	MAP_JOINED_ROOM_ITER iter = m_mapCharJoinState.find(dwChaNum);

	MAP_CHAT_ROOM_ITER iterRoom;

	if (iter != m_mapCharJoinState.end())
	{
		SET_JOINED_ROOM &setJoinedRooms = iter->second;

		SET_JOINED_ROOM_ITER iterJoined = setJoinedRooms.begin();

		for (; iterJoined != setJoinedRooms.end(); ++iterJoined)
		{
			DWORD dwKey = *iterJoined;

			iterRoom = m_mapRooms.find(dwKey);
			if (iterRoom == m_mapRooms.end())
			{
				continue;
			}
			
			ChatRoom &room = iterRoom->second;

			room.Quit(dwChaNum);

			//setJoinedRooms.erase(dwKey);	//m_mapCharJoinState상에서 통째로 삭제할 것이므로 불필요
		}

		//iter = m_mapCharJoinState.erase(iter);
	}
}

*/

void ChatManager::UpdateJoinSet(DWORD dwChaNum, DWORD dwKey)
{
	if (!GLUseFeatures::GetInstance().IsUsingGroupChat())
		return;

	MAP_JOINED_ROOM_ITER iter = m_mapCharJoinState.find(dwChaNum);

	if (iter != m_mapCharJoinState.end())
	{
		SET_JOINED_ROOM &setJoinedRooms = iter->second;

		setJoinedRooms.insert(dwKey);
	}
	else
	{
		SET_JOINED_ROOM setJoined;
		setJoined.insert(dwKey);

		m_mapCharJoinState.insert(MAP_JOINED_ROOM_VALUE(dwChaNum, setJoined));
	}
}

BOOL ChatManager::CheckLevel(DWORD dwLevel)
{
	return (dwLevel >= m_dwRequiredLevel);
}

std::string ChatManager::GetChaName(DWORD dwChaNum)
{
	return m_pGLAgentServer->GetChaNameByDbNum(dwChaNum);
}

void ChatManager::SyncConfig(DWORD dwClientID)
{
	GLMSG::SNET_GCHAT_CONFIG_AC msg;
	msg.dwMaxGroupChatOpen	= m_dwMaxOpenChat;
	msg.dwMaxGroupChatTO	= m_dwMaxTO;

	m_pGLAgentServer->SENDTOCLIENT(dwClientID, &msg);
}

void ChatManager::SendToRoom(DWORD dwChaNum /*who says*/, DWORD dwKey /*which room*/, NET_MSG_GENERIC *nmg)
{
	GASSERT(dwChaNum != INVALID_CHANUM);
	GASSERT(nmg != NULL);

	if (dwChaNum == INVALID_CHANUM)
		return;

	if (nmg == NULL)
		return;


	MAP_CHAT_ROOM_ITER iter = m_mapRooms.find(dwKey);

	if (iter == m_mapRooms.end())
	{
		sc::writeLogError(stringformat("ChatManager::SendToRoom() - The room(%1%) does not exist", dwKey));

		return;
	}

	ChatRoom &room = iter->second;

	if (!room.IsMember(dwChaNum))
	{
		sc::writeLogDebug(stringformat("ChatManager::SendToRoom() - The char(%1%) is not a member of the room(%2%)", dwChaNum, dwKey));

		return;
	}

	room.SendToRoom(nmg);
}

void ChatManager::SendToClient(DWORD dwChaNum, NET_MSG_GENERIC *nmg)
{
	GASSERT(dwChaNum != INVALID_CHANUM);
	if (dwChaNum == INVALID_CHANUM)
		return;

	m_pGLAgentServer->SENDTOCLIENT(nmg, dwChaNum);
}
