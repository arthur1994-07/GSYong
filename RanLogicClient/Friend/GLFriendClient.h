#pragma once

#include "../../SigmaCore/String/MinGuid.h"
#include "../../RanLogic/Friend/Friend.h"
#include "../Widget/GLWidgetScript.h"

class GLGaeaClient;

class GLFriendClient : public friendlogic::Friend
{
public:
    GLFriendClient(GLGaeaClient* pGaeaClient);
    virtual ~GLFriendClient();

protected:
    GLGaeaClient* m_pGaeaClient;
	bool m_bReqList;

public:
	virtual void Clear() override;
    virtual bool GroupDel(WORD GroupNum);

public :
	void ReqFriendList();
    void ReqFriendAdd(const char* szName);
	void ReqFriendLureAns(const char* szReqName, bool bOK);
	void SfReqFriendLureAns_throughAlarm(const char* szReqName, bool bOK, LuaTable tbMGuid);
    void ReqFriendLureAns(const char* szReqName, bool bOK, const sc::MGUID& Guid );
	void ReqFriendLureAnsForWidget(const char* szReqName, bool bOK);
    void ReqFriendDel(const char* szDelName ,int iDelFlag = 0 );
	void ReqFriendBlock(const char* szName, bool bBLOCK );

protected:
	void MsgReqFriendAddFb(NET_MSG_GENERIC* nmg);
	void MsgReqFriendAddLure(NET_MSG_GENERIC* nmg);
	void MsgReqFriendDelFb(NET_MSG_GENERIC* nmg);
	void MsgReqFriendBlockFb(NET_MSG_GENERIC* nmg);
	void MsgReqFriendInfo(NET_MSG_GENERIC* nmg);
	void MsgReqFriendState(NET_MSG_GENERIC* nmg);
	void MsgReqFriendRename(NET_MSG_GENERIC* nmg);
	//void MsgReqFriendPhoneNumber(NET_MSG_GENERIC* nmg);

public:
	bool IsFriend( const char* szName );
	bool IsOnline( const char* szName );
	bool IsBlock( const char* szName );
	int GetFriendDBNum(const char* szName);
	LuaTable GetFriendList();
	LuaTable GetBlockList();
	LuaTable GetGroupList();
	LuaTable GetFriendListInGroup( WORD wGroupNum );

	EMCHARINDEX GetGLCCToGLCI( EMCHARCLASS eGLCC );

	int GetMaxFriend();
	int GetMaxBlock();
	int GetMaxGroup();	
	int GetFriendCount();

	void ReqGroupAdd( const char* szName );
	void ReqGroupRename( WORD GroupNum, const char* NewName );
	void ReqGroupDelete( WORD GroupNum );
	void ReqFriendMoveGroup( const char* szName, WORD GroupNum );
	void ReqFriendInviteParty( const char* szName );
	void ReqMoveToFriend( const char* szName );

public:
	void MsgProcess(NET_MSG_GENERIC* nmg);
};
