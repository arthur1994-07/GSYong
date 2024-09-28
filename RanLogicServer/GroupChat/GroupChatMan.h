//mjeon.groupchat

#pragma once

#include "../../SigmaCore/File/StringFile.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/gassert.h"
#include "../../enginelib/G-Logic/TypeDefine.h"
#include "../../RanLogic/s_NetGlobal.h"
#include "../../RanLogic/GroupChat/GroupChat.h"

#include "./GroupChatScriptMan.h"


class GLAgentServer;
class ChatRoom;
class ChatManager;

typedef std::map<DWORD _KEY_, ChatRoom>				MAP_CHAT_ROOM;
typedef std::map<DWORD _KEY_, ChatRoom>::iterator	MAP_CHAT_ROOM_ITER;
typedef std::map<DWORD _KEY_, ChatRoom>::value_type	MAP_CHAT_ROOM_VALUE;

//<ChaNum>
typedef std::set<DWORD>								SET_CHAT_INVITED;
typedef std::set<DWORD>::iterator					SET_CHAT_INVITED_ITER;

//<ChaNum, CHAT_MEMBER>
typedef std::map<DWORD, CHAT_MEMBER>				MAP_CHAT_MEMBER;
typedef std::map<DWORD, CHAT_MEMBER>::iterator		MAP_CHAT_MEMBER_ITER;
typedef std::map<DWORD, CHAT_MEMBER>::value_type	MAP_CHAT_MEMBER_VALUE;

//<Key>
typedef std::set<DWORD _KEY_>						SET_JOINED_ROOM;		//참여한 방의 목록
typedef std::set<DWORD _KEY_>::iterator				SET_JOINED_ROOM_ITER;

//<ChaNum, std::set<Key>>
typedef std::map<DWORD, SET_JOINED_ROOM>			MAP_JOINED_ROOM;		//캐릭터별 join 현황
typedef std::map<DWORD, SET_JOINED_ROOM>::iterator	MAP_JOINED_ROOM_ITER;
typedef std::map<DWORD, SET_JOINED_ROOM>::value_type	MAP_JOINED_ROOM_VALUE;


class ChatRoom
{
public:
	ChatRoom();
	ChatRoom(ChatManager *pChatMan, DWORD dwKey, DWORD dwMaxTO);
	virtual ~ChatRoom();

	BOOL	IsFull() {return (m_dwMaxTO == size());}
	BOOL	IsEmpty() {return (size() == 0);}
	DWORD	size() {return (DWORD)m_mapMembers.size();}
	DWORD	TO() {return m_dwMaxTO - size();}

	BOOL	IsMember(DWORD dwChaNum)
	{
		GASSERT(dwChaNum != INVALID_CHANUM);

		return (m_mapMembers.count(dwChaNum) > 0);
	}

	BOOL Invite(DWORD dwChaNum);

	DWORD _KEY_ Join(DWORD dwChaNum);
	DWORD _KEY_ Quit(DWORD dwChaNum);
	DWORD	GetMemberCount();
	void CheckOffline();
	
	void SendToRoom(NET_MSG_GENERIC *nmg);
	

protected:
	DWORD				m_dwKey;		//Key = owner's ChaNum
	DWORD				m_dwMaxTO;		//최대 참여가능 인원

	MAP_CHAT_MEMBER		m_mapMembers;

	SET_CHAT_INVITED	m_setInvited;	//list of invited characters

	ChatManager			*m_pChatMan;
};


class ChatManager
{
public:
	ChatManager(GLAgentServer *pGLAgentServer);
	virtual ~ChatManager();

	BOOL Initialize();
	void FrameMove();

	BOOL Invite(DWORD dwChaOwner, DWORD dwChaWho);
	DWORD _KEY_ Accept(DWORD dwChaNum, DWORD dwKey);	//accepted by dwChaNum to join the chat(dwKey)	
	DWORD _KEY_ Quit(DWORD dwChaNum, DWORD dwKey);
	DWORD _KEY_ KickOut(DWORD dwChaNum, DWORD dwKey);

	BOOL IsOffline(DWORD dwChaNum);

	//void QuitAllRooms(DWORD dwChaNum);
	void UpdateJoinSet(DWORD dwChaNum, DWORD dwKey);

	BOOL CheckLevel(DWORD dwChaNum);

	std::string GetChaName(DWORD dwChaNum);

	void SyncConfig(DWORD dwClientID);	//send configurations to a client


	//Network methods
public:
	void SendToRoom(DWORD dwChaNum /*who says*/, DWORD dwKey /*which room*/, NET_MSG_GENERIC *nmg);
	void SendToClient(DWORD dwChaNum, NET_MSG_GENERIC *nmg);
	
protected:
	BOOL ConstructRoom(DWORD dwChaNum);

protected:
	DWORD			m_dwMaxOpenChat;
	DWORD			m_dwMaxTO;
	DWORD			m_dwRequiredLevel;
	DWORD			m_dwLogTime;
	const DWORD		m_dwHourLog;
	DWORD			m_dwLogPeople;

	MAP_CHAT_ROOM	m_mapRooms;

	MAP_JOINED_ROOM	m_mapCharJoinState;		//캐릭터별 참여 현황


protected:	
	ChatScriptMan	*m_pScriptMan;
	GLAgentServer	*m_pGLAgentServer;

	friend class ChatRoom;
};

