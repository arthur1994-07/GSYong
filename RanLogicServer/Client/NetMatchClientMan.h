#pragma once

#include "./NetClientManager.h"


class NetMatchClientMan : public NetClientManager
{
public:
	NetMatchClientMan(int nMaxClient, HANDLE hIOServer, int nReferenceCountMax, A_SERVER_INFO *pAgentServerInfo);
	virtual ~NetMatchClientMan();

protected:
	//char	m_szAgentIP[MAX_IP_LENGTH+1]; // Match서버로 여러개의 Agent 서버가 접속한다.
	//DWORD	m_dwAgentSlot;
	
public:
	//mjeon.AF
	A_SERVER_INFO *m_pAgentServerInfo;

	//
	//mjeon.SetAcceptedClient를 호출하는 시점에 SlotType이 명확하다면, 반드시 매개변수로 넘겨주도록 하자.
	//
	int  SetAcceptedClient(DWORD dwClient, SOCKET sSocket, net::SLOT_TYPE slotType = net::SLOT_UNKNOWN);	//mjeon.slotType이 명확한 경우 명시적으로 넘겨주자.
	
	void SetSlotAgent(DWORD dwClient);
	void SetSlotAgent(UINT n, DWORD dwClient);

	void SetSlotInstance(DWORD dwClient);
	void SetSlotInstance(UINT n, DWORD dwClient);

	bool IsAgentSlot(DWORD dwClient);
	bool IsInstanceSlot(DWORD dwClient);

	//void SetAgentIP(const char* szIP);
	//DWORD GetSlotAgent();	//Match서버는 AgentSlot이 여러개일 수 있다.
	//DWORD GetSlotAgent();
	//BOOL  IsAgentConnected() {return (m_dwAgentSlot != net::SLOT_UNKNOWN);}	//Agent서버가 접속되어 있는지 체크

public:

	int SendInstance(DWORD dwClient, NET_MSG_GENERIC *nmg);
	int SendAgent(DWORD dwClient, NET_MSG_GENERIC *nmg);

	int	SendServer(DWORD dwClient, NET_MSG_GENERIC* pBuffer);
	int SendServer(DWORD dwClient, PER_IO_OPERATION_DATA* PerIoData, DWORD dwSize);

	int SendClient(DWORD dwClient, NET_MSG_GENERIC* pBuffer);
	//int	SendAgent(DWORD dwClient, NET_MSG_GENERIC* pBuffer);
	//int	SendAgent(DWORD dwSvrGroupNum, PER_IO_OPERATION_DATA* PerIoData, DWORD dwSize);
};

