#pragma once

#include "./NetClientManager.h"


class NetMatchClientMan : public NetClientManager
{
public:
	NetMatchClientMan(int nMaxClient, HANDLE hIOServer, int nReferenceCountMax, A_SERVER_INFO *pAgentServerInfo);
	virtual ~NetMatchClientMan();

protected:
	//char	m_szAgentIP[MAX_IP_LENGTH+1]; // Match������ �������� Agent ������ �����Ѵ�.
	//DWORD	m_dwAgentSlot;
	
public:
	//mjeon.AF
	A_SERVER_INFO *m_pAgentServerInfo;

	//
	//mjeon.SetAcceptedClient�� ȣ���ϴ� ������ SlotType�� ��Ȯ�ϴٸ�, �ݵ�� �Ű������� �Ѱ��ֵ��� ����.
	//
	int  SetAcceptedClient(DWORD dwClient, SOCKET sSocket, net::SLOT_TYPE slotType = net::SLOT_UNKNOWN);	//mjeon.slotType�� ��Ȯ�� ��� ��������� �Ѱ�����.
	
	void SetSlotAgent(DWORD dwClient);
	void SetSlotAgent(UINT n, DWORD dwClient);

	void SetSlotInstance(DWORD dwClient);
	void SetSlotInstance(UINT n, DWORD dwClient);

	bool IsAgentSlot(DWORD dwClient);
	bool IsInstanceSlot(DWORD dwClient);

	//void SetAgentIP(const char* szIP);
	//DWORD GetSlotAgent();	//Match������ AgentSlot�� �������� �� �ִ�.
	//DWORD GetSlotAgent();
	//BOOL  IsAgentConnected() {return (m_dwAgentSlot != net::SLOT_UNKNOWN);}	//Agent������ ���ӵǾ� �ִ��� üũ

public:

	int SendInstance(DWORD dwClient, NET_MSG_GENERIC *nmg);
	int SendAgent(DWORD dwClient, NET_MSG_GENERIC *nmg);

	int	SendServer(DWORD dwClient, NET_MSG_GENERIC* pBuffer);
	int SendServer(DWORD dwClient, PER_IO_OPERATION_DATA* PerIoData, DWORD dwSize);

	int SendClient(DWORD dwClient, NET_MSG_GENERIC* pBuffer);
	//int	SendAgent(DWORD dwClient, NET_MSG_GENERIC* pBuffer);
	//int	SendAgent(DWORD dwSvrGroupNum, PER_IO_OPERATION_DATA* PerIoData, DWORD dwSize);
};

