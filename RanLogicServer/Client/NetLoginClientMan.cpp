#include "../pch.h"
#include "./NetLoginClientMan.h"

#include "../../SigmaCore/DebugInclude.h"


NetLoginClientMan::NetLoginClientMan(int nMaxClient, HANDLE hIOServer, int nReferenceCountMax)
	: NetClientManager(nMaxClient, hIOServer, nReferenceCountMax)
{
}

NetLoginClientMan::~NetLoginClientMan()
{
}

/*
mjeon.NetClientMan(parent)�� ���� �״�� �ᵵ �����ҵ�

// ���� �������� Ŭ���̾�Ʈ ����
int NetLoginClientMan::GetCurrentClientNumber()
{
	return m_nMaxClient - (int) m_deqSleepCID.size() - 1;
}
*/