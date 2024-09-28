#ifndef _GLOGIC_SERVER_DB_ACTION_GAME_FRIEND_H_
#define _GLOGIC_SERVER_DB_ACTION_GAME_FRIEND_H_

#pragma once

#include "DbAction.h"

namespace db
{

class FriendDelete : public DbAction
{
public:
    FriendDelete(int nChaP, int nChaS);
    virtual ~FriendDelete() {};
    virtual int Execute(NetServer* pServer) override;

protected:
    int m_nChaP;
    int m_nChaS;
};

class FriendAdd : public DbAction
{
public:
    FriendAdd(int nChaP, int nChaS);
    virtual ~FriendAdd() {};
    virtual int Execute(NetServer* pServer) override;

protected:
    int m_nChaP;
    int m_nChaS;
};

class FriendFlagUpdate : public DbAction
{
public:
    FriendFlagUpdate(int nChaP, int nChaS, int nFlag);
    virtual ~FriendFlagUpdate() {};
    virtual int Execute(NetServer* pServer) override;

protected:
    int m_nChaP;
    int m_nChaS;
    int m_nFlag;
};

class ChaFriendGroupInsert : public DbAction
{
public:
    ChaFriendGroupInsert(int ChaDbNum, WORD GroupNum, const std::string& GroupName);
    virtual ~ChaFriendGroupInsert();

protected:
    int m_ChaDbNum;
    WORD m_GroupNum;
    std::string m_GroupName;

public:
    virtual int Execute(NetServer* pServer) override;
};

class ChaFriendGroupUpdate : public DbAction
{
public:
    ChaFriendGroupUpdate(int ChaDbNum, WORD GroupNum, const std::string& GroupName);
    virtual ~ChaFriendGroupUpdate();

protected:
    int m_ChaDbNum;
    WORD m_GroupNum;
    std::string m_GroupName;

public:
    virtual int Execute(NetServer* pServer) override;
};

class ChaFriendGroupDelete : public DbAction
{
public:
    ChaFriendGroupDelete(int ChaDbNum, WORD GroupNum);
    virtual ~ChaFriendGroupDelete();

protected:
    int m_ChaDbNum;
    WORD m_GroupNum;

public:
    virtual int Execute(NetServer* pServer) override;
};

class ChaFriendGroupSet : public DbAction
{
public:
    ChaFriendGroupSet(DWORD ChaP, DWORD ChaS, WORD GroupNum);
    virtual ~ChaFriendGroupSet();

protected:
    DWORD m_ChaP;
    DWORD m_ChaS;
    WORD m_GroupNum;

public:
    virtual int Execute(NetServer* pServer) override;
};

class FriendStateSelect : public DbAction
{
public:
    FriendStateSelect(DWORD MainChar, DWORD SubChar, DWORD ReqChaDbNum);
    virtual ~FriendStateSelect();
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_ReqCharDbNum;
    DWORD m_MainChar;
    DWORD m_SubChar;    
};


/*
class CGetChaFriend : public CDbAction
{
public:
	CGetChaFriend(
		int nChaNum,
		DWORD dwClient,
		const char* szUserIP,
		USHORT uPort=0 );
	virtual ~CGetChaFriend() {};
	virtual int Execute(CServer* pServer);

protected:
	int m_nChaNum;
};
*/

/*
class CGetChaFriendSMS : public CDbAction
{
public:
	CGetChaFriendSMS(
		int nChaNum,
		DWORD dwClient,
		const char* szUserIP,
		USHORT uPort=0 );
	virtual ~CGetChaFriend() {};
	virtual int Execute(CServer* pServer);

protected:
	int m_nChaNum;
};
*/


} // namespace db

#endif // _GLOGIC_SERVER_DB_ACTION_GAME_FRIEND_H_