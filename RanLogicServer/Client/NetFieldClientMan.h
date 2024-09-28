#ifndef _NET_FIELD_CLIENT_MAN_H_
#define _NET_FIELD_CLIENT_MAN_H_

#pragma once

#include "./NetClientManager.h"

enum EMSERVICE_PROVIDER;

//! class CClientField
class NetFieldClientMan : public NetClientManager
{
public:
	NetFieldClientMan( int nMaxClient, HANDLE hIOServer, int nReferenceCountMax, EMSERVICE_PROVIDER eServiceProvider );
	virtual ~NetFieldClientMan();

protected:
	char	m_szAgentIP[MAX_IP_LENGTH+1]; // Agent 서버 IP
    char	m_szCacheIP[MAX_IP_LENGTH+1]; // Cache 서버 IP
	DWORD	m_dwAgentSlot;
	EMSERVICE_PROVIDER	m_ServiceProvider;
	
public:
	//
	//mjeon.SetAcceptedClient를 호출하는 시점에 SlotType이 명확하다면, 반드시 매개변수로 넘겨주도록 하자.
	//
	int  SetAcceptedClient(DWORD dwClient, SOCKET sSocket, net::SLOT_TYPE slotType = net::SLOT_UNKNOWN);	//mjeon.slotType이 명확한 경우 명시적으로 넘겨주자.
	void SetAgentIP(const char* szIP);
    void SetCacheIP(const char* szIP);
	
	void CloseClient(DWORD dwClient);
    void SetCountry(DWORD ClientSlot, wb::EM_COUNTRY Country);
    wb::EM_COUNTRY GetCountry(DWORD ClientSlot);

	void  SetSlotAgent(DWORD dwClient);
	DWORD GetSlotAgent();
	BOOL  IsAgentConnected() {return (m_dwAgentSlot != net::INVALID_SLOT);}	//Agent서버가 접속되어 있는지 체크

public:
	int	SendAgent(DWORD dwClient, NET_MSG_GENERIC* pBuffer);
	int	SendAgent(PER_IO_OPERATION_DATA* PerIoData, DWORD dwSize);
	int SendClient(DWORD dwClient, NET_MSG_GENERIC* pBuffer);
};

#endif // _NET_FIELD_CLIENT_MAN_H_