#pragma once

#include "./NetClientManager.h"

class NetInstanceClientMan : public NetClientManager
{
public:
	NetInstanceClientMan(int nMaxClient, HANDLE	hIOServer, int nReferenceCountMax);
	virtual ~NetInstanceClientMan();

protected:
	char	m_szMatchIP[MAX_IP_LENGTH+1]; // Match ���� IP
	DWORD	m_dwMatchSlot;
	
public:
	//
	//mjeon.SetAcceptedClient�� ȣ���ϴ� ������ SlotType�� ��Ȯ�ϴٸ�, �ݵ�� �Ű������� �Ѱ��ֵ��� ����.
	//
	int  SetAcceptedClient(DWORD dwClient, SOCKET sSocket, net::SLOT_TYPE slotType = net::SLOT_UNKNOWN);	//mjeon.slotType�� ��Ȯ�� ��� ��������� �Ѱ�����.
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