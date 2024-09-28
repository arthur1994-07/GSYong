#ifndef _CLIENT_AUTH_H_
#define _CLIENT_AUTH_H_

#pragma once

#include "../../RanLogic/Network/NetLogicDefine.h"
#include "./NetServerDefine.h"
#include "./NetClientManager.h"

#define USER_MAP_START_SIZE 100

class NetAuthClientMan : public NetClientManager
{
public:
	NetAuthClientMan(int nMaxClient, HANDLE hIOServer, int nReferenceCountMax);
	virtual ~NetAuthClientMan();

protected:	
    typedef std::map<DWORD, G_AUTH_INFO*> SERVER_INFO_MAP;
    typedef SERVER_INFO_MAP::iterator       SERVER_INFO_MAP_ITER;
    typedef SERVER_INFO_MAP::const_iterator SERVER_INFO_MAP_CITER;
    typedef SERVER_INFO_MAP::value_type     SERVER_INFO_MAP_VALUE;
	
    SERVER_INFO_MAP m_ServerInfo;

private:    
    G_AUTH_INFO* GetServerInfoDataNotNull(DWORD dwClient);
    G_AUTH_INFO* GetServerInfoData(DWORD dwClient);

public:
	void	SetSvrInfo			(DWORD dwClient, G_AUTH_INFO *nsi);

	G_AUTH_INFO GetSvrInfo(DWORD dwClient);
	net::EMSERVERTYPE GetSvrType(DWORD dwClient);
	int		GetSvrServicePort	(DWORD dwClient);
	char*	GetSvrIP			(DWORD dwClient);

	DWORD	FindServer		(const char* szServerIP, int nServicePort);

	int	CloseClient(DWORD dwClientNum);
};

#endif 