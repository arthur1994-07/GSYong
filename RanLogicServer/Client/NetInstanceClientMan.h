#pragma once

#include "./NetClientManager.h"

class NetInstanceClientMan : public NetClientManager
{
public:
	NetInstanceClientMan(int nMaxClient, HANDLE	hIOServer, int nReferenceCountMax);
	virtual ~NetInstanceClientMan();

protected:
	char	m_szMatchIP[MAX_IP_LENGTH+1]; // Match 서버 IP
	DWORD	m_dwMatchSlot;
	
public:
	//
	//mjeon.SetAcceptedClient를 호출하는 시점에 SlotType이 명확하다면, 반드시 매개변수로 넘겨주도록 하자.
	//
	int  SetAcceptedClient(DWORD dwClient, SOCKET sSocket, net::SLOT_TYPE slotType = net::SLOT_UNKNOWN);	//mjeon.slotType이 명확한 경우 명시적으로 넘겨주자.
	void SetMatchIP(const char* szIP);
	
	void CloseClient(DWORD dwClient);

	void  SetSlotMatch(DWORD dwClient);
	DWORD GetSlotMatch();
	BOOL  IsMatchConnected() {return (m_dwMatchSlot != net::SLOT_UNKNOWN);}

public:
	int	SendMatch(DWORD dwClient, NET_MSG_GENERIC* pBuffer);
	int	SendMatch(PER_IO_OPERATION_DATA* PerIoData, DWORD dwSize);
	int SendClient(DWORD dwClient, NET_MSG_GENERIC* pBuffer);
};