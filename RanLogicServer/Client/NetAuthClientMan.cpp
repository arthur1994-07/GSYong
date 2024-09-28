#include "pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "./NetUser.h"
#include "./NetAuthClientMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

NetAuthClientMan::NetAuthClientMan(int nMaxClient, HANDLE hIOServer, int nReferenceCountMax)
	: NetClientManager(nMaxClient, hIOServer, nReferenceCountMax)
{
}

NetAuthClientMan::~NetAuthClientMan()
{
    for (SERVER_INFO_MAP_ITER iter=m_ServerInfo.begin(); iter!=m_ServerInfo.end(); ++iter)
    {
        G_AUTH_INFO* pData = iter->second;
        delete pData;
        pData = NULL;
    }
}

int NetAuthClientMan::CloseClient( DWORD dwClientNum )
{
	int nResult = NetClientManager::CloseClient( dwClientNum );
	if ( NET_ERROR != nResult )
	{
		// 이 정보는 예전에는 바로 삭제가 되었었다. 이렇게 되어도 문제가 없어야 한다. 
		SERVER_INFO_MAP_ITER iter = m_ServerInfo.find( dwClientNum );
		if ( iter != m_ServerInfo.end() )
		{
			G_AUTH_INFO* pData = iter->second;
			delete pData;
			pData = NULL;
			m_ServerInfo.erase( iter );
		}
	}
	return nResult;
}

G_AUTH_INFO* NetAuthClientMan::GetServerInfoDataNotNull(DWORD dwClient)
{
    G_AUTH_INFO* pData = NULL;
    SERVER_INFO_MAP_ITER iter = m_ServerInfo.find(dwClient);
    if (iter != m_ServerInfo.end())
    {
        pData = iter->second;
    }
    else
    {
        pData = new G_AUTH_INFO;
        m_ServerInfo.insert(SERVER_INFO_MAP_VALUE(dwClient, pData));
    }
    return pData;
}

G_AUTH_INFO* NetAuthClientMan::GetServerInfoData(DWORD dwClient)
{    
    SERVER_INFO_MAP_ITER iter = m_ServerInfo.find(dwClient);
    if (iter != m_ServerInfo.end())
        return iter->second;
    else
        return NULL;
}

// 게임서버 -> 세션서버 : 게임서버의 상태 전송
void NetAuthClientMan::SetSvrInfo(DWORD dwClient, G_AUTH_INFO* gsi)
{    
    G_AUTH_INFO* pData = GetServerInfoDataNotNull(dwClient);
    *pData = *gsi;
}

// 주소와 포트를 이용해서 서버 번호를 찾는다.
DWORD NetAuthClientMan::FindServer(const char* szServerIP, int nServicePort)
{
	sc::writeLogInfo(std::string("서버 찾기 시작"));
	sc::writeLogInfo(std::string(szServerIP));

	for (UINT nClient=0; nClient < m_nMaxClient; ++nClient)
	{
		if (m_ClientMan[nClient]->IsOnline()) // 온라인이라면
		{
            const char* szIp = GetSvrIP(nClient);
            if (!szIp)
                continue;

			if (strcmp(szIp, szServerIP) == 0) // 주소조사
			{			
				if (GetSvrServicePort((DWORD) nClient) == nServicePort) // 포트조사
				{
					// 주소 포트가 동일하면 동일 서버로 간주한다
					return (DWORD) nClient; // 찾은 서버번호 리턴
				}
			}
		}
	}
	return NET_ERROR; // 찾지못하면 에러리턴
}

net::EMSERVERTYPE NetAuthClientMan::GetSvrType(DWORD dwClient)
{
    const G_AUTH_INFO* pData = GetServerInfoData(dwClient);
    if (pData)
        return pData->ServerType;
    else
        return net::SERVER_UNKNOWN;
}

int	NetAuthClientMan::GetSvrServicePort(DWORD dwClient)
{
    G_AUTH_INFO* pData = GetServerInfoData(dwClient);
    if (pData)
        return pData->nServicePort;
    else
        return 0;
}

char* NetAuthClientMan::GetSvrIP(DWORD dwClient)
{
    G_AUTH_INFO* pData = GetServerInfoData(dwClient);
    if (pData)
        return pData->szServerIP;
    else
        return NULL;
}

G_AUTH_INFO NetAuthClientMan::GetSvrInfo(DWORD dwClient)
{
    G_AUTH_INFO* pData = GetServerInfoData(dwClient);
    if (pData)
    {
        return *pData;
    }
    else
    {
        G_AUTH_INFO gsi;
        return gsi;
    }
}