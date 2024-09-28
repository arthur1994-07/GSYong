//mjeon.groupchat.client-side

#pragma once

#include "../../RanLogic/GroupChat/GroupChat.h"
#include <boost/regex.hpp>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include "../../RanLogic/Item/GLItemCustom.h"

#include "../Widget/GLWidgetScript.h"

class GLGaeaClient;

//
//mjeon.groupchat
//
class LinkMan;
typedef std::vector<SITEMCUSTOM>				VEC_ITEM_LINK;
typedef std::vector<SITEMCUSTOM>::iterator		VEC_ITEM_LINK_ITER;

#define UNKNOWN_ITEM_NAME	"Unknown Item"
#define WRONG_LINK_IDX		MAXDWORD


struct grep_predicate
{
public:
	grep_predicate(LinkMan *pLinkMan)
		:m_pLinkMan(pLinkMan)
	{
	}

	bool operator() (const boost::smatch &m);

private:
	LinkMan *m_pLinkMan;

	grep_predicate()
		:m_pLinkMan(NULL)
	{
	}
};

struct LINKPAIR
{
	DWORD		idx;
	std::string name;	//with bracket - "<itemname>"
};

//itemlink관리
class LinkMan
{
public:
	LinkMan();
	virtual ~LinkMan();

	std::string GetFreeTag();
	DWORD /*idx*/ Register(IN SITEMCUSTOM &item, OUT LINKPAIR& linkpair);
	BOOL IsTag(std::string tag);
	DWORD GetIndex(std::string& tag);
	BOOL GetItem(DWORD idx, OUT SITEMCUSTOM& item);
	BOOL GetItemName(DWORD idx, OUT std::string& itemName, BOOL bWithBracket);
	CString& GetTranslateBuffer() {return m_csTranslateBuffer;}
	std::string Translate(const std::string& IN original);

protected:
	VEC_ITEM_LINK	m_vecLinks;

	boost::regex* m_pRegexTag;

	CString m_csTranslateBuffer;
};


typedef std::map<DWORD /*ChaNum*/, CHAT_MEMBER>	MAP_CHATCLIENT_MEMBER;
typedef MAP_CHATCLIENT_MEMBER::iterator			MAP_CHATCLIENT_MEMBER_ITER;
typedef MAP_CHATCLIENT_MEMBER::value_type		MAP_CHATCLIENT_MEMBER_VALUE;

class ChatRoomClient
{
public:
	ChatRoomClient(DWORD dwKey)
		:m_dwKey(dwKey)
	{
	}

	virtual ~ChatRoomClient()
	{
	}

protected:
	DWORD					m_dwKey;	// == owner's ChaNum 방장ID
	CString					m_strOwnerName;

	std::list< SITEMCUSTOM >	m_listLinkItem;

public:
	MAP_CHATCLIENT_MEMBER	m_mapMembers;

public:
	void SETKEY( DWORD dwkey ) { m_dwKey = dwkey; }
	void SETOwnerName( CString strName ) { m_strOwnerName = strName; }
	DWORD GETKEY() { return m_dwKey; }
	CString GetOwnerName()
	{ 
		return m_strOwnerName; 
	}

	DWORD GetOwnerKey()
	{ 
		return m_dwKey; 
	}

	
	//CString GetMembername(int nNumber)
	//{ 
	//	return m_mapMembers[nNumber].szChaName; 
	//}
	BOOL ADDMEMBER ( DWORD dwcharNum, CHAT_MEMBER sMember);
	BOOL DELMEMBER ( DWORD dwcharNum );
	void GetMembername(std::vector<CString>& vecMemberName);
	void GetMember(std::vector<CHAT_MEMBER>& vecMember);
	MAP_CHATCLIENT_MEMBER GetMapMembers();

	bool AddLinkItem( const SITEMCUSTOM& sItem );
	void RemoveLinkItem( int nIndex );
	void ClearLinkItem();

	const std::list< SITEMCUSTOM >& GetLinkItemList() { return m_listLinkItem; }
};

//std::map<DWORD _KEY_, ChatRoomClient>	m_mapChatRoom; //형태로 관리
typedef std::map<DWORD, ChatRoomClient>					MAP_CHATROOMCLIENT;
typedef MAP_CHATROOMCLIENT::iterator					MAP_CHATROOMCLIENT_ITER;
typedef MAP_CHATROOMCLIENT::value_type					MAP_CHATROOMCLIENT_VALUE;

enum GROUPCHAT_INVITE_TYPE
{
	GROUPCHAT_INVITE_INTERACT = 0,
	GROUPCHAT_INVITE_FRIEND = 1,
	GROUPCHAT_INVITE_CLUBMEMBER = 2,
	NONE = 10
};

class GLGroupChat
{
private:
	MAP_CHATROOMCLIENT m_mapChatRoom;
	DWORD m_dwMaxGroupChatOpen;	//최대 참여 가능한 그룹채팅의 수
	DWORD m_dwMaxGroupChatTO;		//하나의 그룹채팅 방에 참여가능한 최대 인원

public:
	GLGroupChat(DWORD dwKey,GLGaeaClient* pGaeaClient)
		:m_pGaeaClient(pGaeaClient)
		,m_dwMaxGroupChatOpen(0)
		,m_dwMaxGroupChatTO(0)
		,m_dwKey(INVALID_CHAT_KEY)
	{
	}

	 virtual ~GLGroupChat();

	LinkMan			m_LinkMan;
	DWORD			m_dwKey;	// == owner's ChaNum
	
	GLGaeaClient* m_pGaeaClient;

public:
	BOOL ADDCHATROOMCLIENT ( DWORD dwKey, CHAT_MEMBER sMember, std::string strOwnerName );
	BOOL DELCHATROOMCLIENT ( DWORD dwOwnerKey, DWORD dwMembenum );
	BOOL DELCHATROOM ( DWORD dwOwnerKey, DWORD dwMembenum );
	MAP_CHATROOMCLIENT GetMapChatRoom() {return m_mapChatRoom;};
	
	void GetRoomName( std::vector<CString>& vecRoomName, std::vector<DWORD>& vecRoomdwKey );
	void GetMembername( std::vector<CHAT_MEMBER>& member, DWORD dwkey );
	CString GetMembername( DWORD dwRoomkey, DWORD dwMemberKey );
	int GetFreeRoomIndex();
	int GetMaxChatRoomIndex( DWORD dwkey );

	DWORD GetMaxGroupChatOpen() { return m_dwMaxGroupChatOpen; };
	DWORD GetMaxGroupChatTO() { return m_dwMaxGroupChatTO; };
	void ChatRoomClear();

protected:
	BOOL MsgChatGroupInviteCAC(NET_MSG_GENERIC* nmg);
	BOOL MsgChatGroupInviteCAC_FB(NET_MSG_GENERIC* nmg);
	BOOL MsgChatGroupERRORCODE(NET_MSG_GENERIC* nmg);
	BOOL MsgChatMemberSyncAC(NET_MSG_GENERIC* nmg);
	BOOL MsgChatGroupMEMBER_UPDATE_AC(NET_MSG_GENERIC* nmg);
	BOOL MsgChatGroupQUIT_CA(NET_MSG_GENERIC* nmg);
	BOOL MsgChatGroupMESSAGECAC(NET_MSG_GENERIC* nmg);
	BOOL MsgChatGroupKickCA(NET_MSG_GENERIC* nmg);
	BOOL MsgChatGroupConfigAC(NET_MSG_GENERIC* nmg);

public:
	// 그룹 채팅 초대
	bool ReqGroupChat( const char* szName, int nType );
	// 그룹 채팅 초대 답장 
	void ReqGroupChatFB( bool bAccept );
	// 그룹 채팅 나가기 요청
	void ReqQuit( DWORD dwKey );
	
	// 채팅방 멤버 가져오기
	LuaTable GetMemberForWidget( DWORD dwKey );
	// 채팅 내용 전송
	void SendChat( DWORD dwKey, const char* szMsg );
	// 링크 아이템 추가
	bool AddLinkItem( DWORD dwKey, const SITEMCUSTOM& sItem );
	// 링크 아이템 제거
	void RemoveLinkItem( DWORD dwKey, int nIndex );
	// 링크 아이템 정보 가져오기
	LuaTable GetLinkItemInfo( DWORD dwKey );
	// 강퇴
	void ReqKick( const char* szName );

public:
	HRESULT MsgProcess(NET_MSG_GENERIC* nmg);
};


////
////mjeon.groupchat
////
//typedef std::vector<SITEMCUSTOM>				VEC_ITEM_LINK;
//typedef std::vector<SITEMCUSTOM>::iterator		VEC_ITEM_LINK_ITER;
//
//#define UNKNOWN_ITEM_NAME	"Unknown Item"
//#define WRONG_LINK_IDX		MAXDWORD
//
//class LinkMan;
//
//struct grep_predicate
//{
//public:
//	grep_predicate(LinkMan *pLinkMan)
//		:m_pLinkMan(pLinkMan)
//	{
//	}
//
//	bool operator() (const boost::smatch &m);
//
//private:
//	LinkMan *m_pLinkMan;
//
//	grep_predicate()
//		:m_pLinkMan(NULL)
//	{
//	}
//};
//
//struct LINKPAIR
//{
//	DWORD		idx;
//	std::string name;	//with bracket - "<itemname>"
//};
//
////itemlink관리
//class LinkMan
//{
//public:
//	LinkMan();
//	virtual ~LinkMan();
//
//	std::string GetFreeTag();
//	DWORD /*idx*/ Register(IN SITEMCUSTOM &item, OUT LINKPAIR& linkpair);
//	BOOL IsTag(std::string tag);
//	DWORD GetIndex(std::string& tag);
//	BOOL GetItem(DWORD idx, OUT SITEMCUSTOM& item);
//	BOOL GetItemName(DWORD idx, OUT std::string& itemName, BOOL bWithBracket);
//	CString& GetTranslateBuffer() {return m_csTranslateBuffer;}
//	std::string Translate(const std::string& IN original);
//
//protected:
//	VEC_ITEM_LINK	m_vecLinks;
//
//	boost::regex* m_pRegexTag;
//
//	CString m_csTranslateBuffer;
//};
