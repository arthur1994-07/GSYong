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
mjeon.NetClientMan(parent)의 것을 그대로 써도 무방할듯

// 현재 접속중인 클라이언트 숫자
int NetLoginClientMan::GetCurrentClientNumber()
{
	return m_nMaxClient - (int) m_deqSleepCID.size() - 1;
}
*/