#ifndef _DB_ACTION_AUTH_H_
#define _DB_ACTION_AUTH_H_

#pragma once

#include "../DbDefineLog.h"
#include "DbAction.h"

#include "../../../RanLogic/GLogicData.h"
#include "../../../RanLogic/Util/GlobalAuthManager.h"


//namespace sc
//{
//	struct SAuthData;
//}

struct G_AUTH_EVENT_INFO;

namespace db
{

class LogAuthState : public DbAction
{
public:
	enum { MAX_IP_LEN = 21 };

public:
	LogAuthState(int nCounrty, int nServerType, TCHAR* szIP, int nPort, int nCurUser, int nMaxUser);
	virtual ~LogAuthState() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nCounrty;
	int m_nServerType;
	TCHAR m_szIP[MAX_IP_LEN];
	int m_nPort;
	int m_nCurUser;
	int m_nMaxUser;
    __time64_t m_LogDate;
};

class CertificationAuthData : public DbAction
{
public:
	CertificationAuthData( DWORD dwClient, sc::SAuthData *authData );
	virtual ~CertificationAuthData() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD m_dwClient;
	sc::SAuthData m_AuthData;
};

class AuthEventInfo : public DbAction
{
public:
	AuthEventInfo(G_AUTH_EVENT_INFO &eventInfo);
	virtual ~AuthEventInfo() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	G_AUTH_EVENT_INFO m_EventInfo;
};


class LogAuthServerStateInit : public DbAction
{
public:
	LogAuthServerStateInit();
	virtual ~LogAuthServerStateInit() {};
	virtual int Execute(NetServer* pServer) override;	
};

class LogAuthServerState : public DbAction
{
public:
	enum { MAX_IP_LEN = 21 };

public:
	LogAuthServerState(int nSessionSvrID, int nCounrty, int nServerType, TCHAR* szIP, int nPort);
	virtual ~LogAuthServerState() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	int m_nSessionSvrID;
	int m_nCounrty;
	int m_nServerType;
	TCHAR m_szIP[MAX_IP_LEN];
	int m_nPort;	
};

} 
#endif 