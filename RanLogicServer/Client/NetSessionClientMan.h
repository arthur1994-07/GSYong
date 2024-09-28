#ifndef _CLIENT_SESSION_H_
#define _CLIENT_SESSION_H_

#pragma once

#include "../../RanLogic/Network/NetLogicDefine.h"
#include "./NetServerDefine.h"
#include "./NetClientManager.h"

#define USER_MAP_START_SIZE 100

//! \version 1.1
//! 
//! \date 2003-02-28 
//! 
//! \author jgkim
//! 
//! \todo 
//! 
//! \bug 
//! 
class NetSessionClientMan : public NetClientManager
{
public:
	NetSessionClientMan(int nMaxClient, HANDLE hIOServer, int nReferenceCountMax);
	virtual ~NetSessionClientMan();

protected:	
	CMapStringToPtr*	m_pUsrMap;
	CRITICAL_SECTION	m_CS_UserMap;

    typedef std::map<DWORD, G_SERVER_INFO*> SERVER_INFO_MAP;
    typedef SERVER_INFO_MAP::iterator       SERVER_INFO_MAP_ITER;
    typedef SERVER_INFO_MAP::const_iterator SERVER_INFO_MAP_CITER;
    typedef SERVER_INFO_MAP::value_type     SERVER_INFO_MAP_VALUE;
	
    SERVER_INFO_MAP m_ServerInfo;

private:    
    G_SERVER_INFO* GetServerInfoDataNotNull(DWORD dwClient);
    G_SERVER_INFO* GetServerInfoData(DWORD dwClient);

public:
	void	SetSvrInfo			(DWORD dwClient, G_SERVER_INFO *nsi);
	void	SetSvrCurInfo		(DWORD dwClient, G_SERVER_CUR_INFO *nsi);

	G_SERVER_INFO GetSvrInfo(DWORD dwClient);
	net::EMSERVERTYPE GetSvrType(DWORD dwClient);
	int		GetSvrGrp			(DWORD dwClient);
	int		GetSvrNum			(DWORD dwClient);
	int		GetSvrField			(DWORD dwClient);
	
	int		GetSvrCrtClient		(DWORD dwClient);
	int		GetSvrMaxClient		(DWORD dwClient);
	int		GetSvrServicePort	(DWORD dwClient);
	char*	GetSvrIP			(DWORD dwClient);
	bool	IsGameServer		(DWORD dwClient);

	DWORD	FindServer		(const char* szServerIP, int nServicePort);
	DWORD	FindServer		(int nType, int nSvrGrp, int nSvrNum, int nField);
	DWORD	FindServer		(int nType, int nIndex);
	int FindServerCount(int nType);
	
	void UserInsert(const CString& strUsrID, LPCNT_USR_INFO pData);	
	void UpdateChaJoinOK(
        CString strUsrID,
        int nUsrNum, 
        int nChaNum, 
        DWORD dwGaeaID,
        const char* szChaName);

	BOOL UserErase(const CString& strUsrID);
	void UserEraseServer(int nSvrGrp, int nSvrNum);
	void UserEraseAll();
	LPCNT_USR_INFO UserFind(const CString& strUsrID);
	
	void PrintUsrList();

	int	CloseClient(DWORD dwClientNum);
};

#endif // _CLIENT_SESSION_H_