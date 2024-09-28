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
		// �� ������ �������� �ٷ� ������ �Ǿ�����. �̷��� �Ǿ ������ ����� �Ѵ�. 
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

// ���Ӽ��� -> ���Ǽ��� : ���Ӽ����� ���� ����
void NetAuthClientMan::SetSvrInfo(DWORD dwClient, G_AUTH_INFO* gsi)
{    
    G_AUTH_INFO* pData = GetServerInfoDataNotNull(dwClient);
    *pData = *gsi;
}

// �ּҿ� ��Ʈ�� �̿��ؼ� ���� ��ȣ�� ã�´�.
DWORD NetAuthClientMan::FindServer(const char* szServerIP, int nServicePort)
{
	sc::writeLogInfo(std::string("���� ã�� ����"));
	sc::writeLogInfo(std::string(szServerIP));

	for (UINT nClient=0; nClient < m_nMaxClient; ++nClient)
	{
		if (m_ClientMan[nClient]->IsOnline()) // �¶����̶��
		{
            const char* szIp = GetSvrIP(nClient);
            if (!szIp)
                continue;

			if (strcmp(szIp, szServerIP) == 0) // �ּ�����
			{			
				if (GetSvrServicePort((DWORD) nClient) == nServicePort) // ��Ʈ����
				{
					// �ּ� ��Ʈ�� �����ϸ� ���� ������ �����Ѵ�
					return (DWORD) nClient; // ã�� ������ȣ ����
				}
			}
		}
	}
	return NET_ERROR; // ã�����ϸ� ��������
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