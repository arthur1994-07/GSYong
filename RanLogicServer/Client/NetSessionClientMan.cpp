#include "../pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "./NetUser.h"
#include "./NetSessionClientMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

NetSessionClientMan::NetSessionClientMan(int nMaxClient, HANDLE hIOServer, int nReferenceCountMax)
	: NetClientManager(nMaxClient, hIOServer, nReferenceCountMax)
{
	m_pUsrMap = new CMapStringToPtr (USER_MAP_START_SIZE);
	InitializeCriticalSectionAndSpinCount(&m_CS_UserMap, sc::CRITICAL_SECTION_SPIN_COUNT);
}

NetSessionClientMan::~NetSessionClientMan()
{
	UserEraseAll();
	SAFE_DELETE(m_pUsrMap);

    for (SERVER_INFO_MAP_ITER iter=m_ServerInfo.begin(); iter!=m_ServerInfo.end(); ++iter)
    {
        G_SERVER_INFO* pData = iter->second;
        delete pData;
        pData = NULL;
    }
	DeleteCriticalSection(&m_CS_UserMap);
}

int NetSessionClientMan::CloseClient( DWORD dwClientNum )
{
	int nResult = NetClientManager::CloseClient( dwClientNum );
	if ( NET_ERROR != nResult )
	{
		// �� ������ �������� �ٷ� ������ �Ǿ�����. �̷��� �Ǿ ������ ����� �Ѵ�. 
		SERVER_INFO_MAP_ITER iter = m_ServerInfo.find( dwClientNum );
		if ( iter != m_ServerInfo.end() )
		{
			G_SERVER_INFO* pData = iter->second;
			delete pData;
			pData = NULL;
			m_ServerInfo.erase( iter );
		}
	}
	return nResult;
}

G_SERVER_INFO* NetSessionClientMan::GetServerInfoDataNotNull(DWORD dwClient)
{
    G_SERVER_INFO* pData = NULL;
    SERVER_INFO_MAP_ITER iter = m_ServerInfo.find(dwClient);
    if (iter != m_ServerInfo.end())
    {
        pData = iter->second;
    }
    else
    {
        pData = new G_SERVER_INFO;
        m_ServerInfo.insert(SERVER_INFO_MAP_VALUE(dwClient, pData));
    }
    return pData;
}

G_SERVER_INFO* NetSessionClientMan::GetServerInfoData(DWORD dwClient)
{    
    SERVER_INFO_MAP_ITER iter = m_ServerInfo.find(dwClient);
    if (iter != m_ServerInfo.end())
        return iter->second;
    else
        return NULL;
}

// ���Ӽ��� -> ���Ǽ��� : ���Ӽ����� ���� ����
void NetSessionClientMan::SetSvrInfo(DWORD dwClient, G_SERVER_INFO* gsi)
{    
    G_SERVER_INFO* pData = GetServerInfoDataNotNull(dwClient);
    *pData = *gsi;
}

// ���Ӽ��� -> ���Ǽ��� : ���Ӽ����� ������� ���� (�������ڼ�/�ִ����ڼ�)
void NetSessionClientMan::SetSvrCurInfo(DWORD dwClient, G_SERVER_CUR_INFO *nsi)
{
    G_SERVER_INFO* pData = GetServerInfoDataNotNull(dwClient);
	pData->nServerCurrentClient = nsi->nServerCurrentClient;
	pData->nServerMaxClient     = nsi->nServerMaxClient;
}

DWORD NetSessionClientMan::FindServer(int nType, int nSvrGrp, int nSvrNum, int nField)
{	
    DWORD dwClient = 0;
	for (SERVER_INFO_MAP_CITER iter=m_ServerInfo.begin(); iter!=m_ServerInfo.end(); ++iter)
	{
		const G_SERVER_INFO* pData = iter->second;
		if (nType == net::SERVER_AGENT)
		{
			if (pData->ServerType == (DWORD) nType && pData->nServerGroup == (DWORD) nSvrGrp)
				return iter->first;
		}
        else if (nType == net::SERVER_CACHE)
		{
			if (pData->ServerType == (DWORD) nType && pData->nServerGroup == (DWORD) nSvrGrp)
				return iter->first;
		}
		else if ( nType == net::SERVER_INTEGRATION_CACHE )
		{
			if ( pData->ServerType == static_cast< DWORD >( nType ) && pData->nServerGroup == static_cast< DWORD >( nSvrGrp ) )
				return iter->first;
		}
        else
        {
			 if (pData->ServerType == (DWORD) nType &&
                 pData->nServerGroup == (DWORD) nSvrGrp && 
                 pData->nServerNumber == (DWORD) nSvrNum &&
				 pData->nServerField == (DWORD) nField)
            {
				return iter->first;
            }
		}
	}
	return NET_ERROR; // ã�����ϸ� ��������
}

int NetSessionClientMan::FindServerCount(int nType)
{	
    int nCount = 0;

	for (SERVER_INFO_MAP_CITER iter=m_ServerInfo.begin(); iter!=m_ServerInfo.end(); ++iter)
	{
		if( (iter->second)->ServerType == (DWORD) nType )
		{
			nCount++;
		}
	}

	return nCount; 
}

DWORD NetSessionClientMan::FindServer(int nType, int nIndex)
{	
    DWORD dwClient = 0;
	int nCount = 0;

	for (SERVER_INFO_MAP_CITER iter=m_ServerInfo.begin(); iter!=m_ServerInfo.end(); ++iter)
	{
		if( (iter->second)->ServerType == (DWORD) nType )
		{
			if( nCount == nIndex )
			{
				return iter->first;
			}
			else
			{
				nCount++;
			}
		}
	}

	return NET_ERROR; // ã�����ϸ� ��������
}

// �ּҿ� ��Ʈ�� �̿��ؼ� ���� ��ȣ�� ã�´�.
DWORD NetSessionClientMan::FindServer(const char* szServerIP, int nServicePort)
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

int NetSessionClientMan::GetSvrGrp(DWORD dwClient)
{
    G_SERVER_INFO* pData = GetServerInfoData(dwClient);
    if (pData)
        return pData->nServerGroup;
    else
        return 0;
}

int NetSessionClientMan::GetSvrNum(DWORD dwClient)
{
    G_SERVER_INFO* pData = GetServerInfoData(dwClient);
    if (pData)
        return pData->nServerNumber;
    else
        return 0;
}

int NetSessionClientMan::GetSvrField(DWORD dwClient)
{
    G_SERVER_INFO* pData = GetServerInfoData(dwClient);
    if (pData)
        return pData->nServerField;
    else
        return 0;
}

net::EMSERVERTYPE NetSessionClientMan::GetSvrType(DWORD dwClient)
{
    const G_SERVER_INFO* pData = GetServerInfoData(dwClient);
    if (pData)
        return pData->ServerType;
    else
        return net::SERVER_UNKNOWN;
}

int	NetSessionClientMan::GetSvrServicePort(DWORD dwClient)
{
    G_SERVER_INFO* pData = GetServerInfoData(dwClient);
    if (pData)
        return pData->nServicePort;
    else
        return 0;
}

int NetSessionClientMan::GetSvrCrtClient(DWORD dwClient)
{
    G_SERVER_INFO* pData = GetServerInfoData(dwClient);
    if (pData)
        return pData->nServerCurrentClient;
    else
        return 0;
}

int NetSessionClientMan::GetSvrMaxClient(DWORD dwClient)
{
    G_SERVER_INFO* pData = GetServerInfoData(dwClient);
    if (pData)
        return pData->nServerChannellMaxClient;
    else
        return 0;
}

char* NetSessionClientMan::GetSvrIP(DWORD dwClient)
{
    G_SERVER_INFO* pData = GetServerInfoData(dwClient);
    if (pData)
        return pData->szServerIP;
    else
        return NULL;
}

G_SERVER_INFO NetSessionClientMan::GetSvrInfo(DWORD dwClient)
{
    G_SERVER_INFO* pData = GetServerInfoData(dwClient);
    if (pData)
    {
        return *pData;
    }
    else
    {
        G_SERVER_INFO gsi;
        return gsi;
    }
}

void NetSessionClientMan::UpdateChaJoinOK(
    CString strUsrID, 							
    int nUsrNum, 
    int nChaNum, 
    DWORD dwGaeaID,
    const char* szChaName)
{
	if (!szChaName)
        return;

	LPCNT_USR_INFO pExistData = NULL;	

	// ������ ���ӵǾ� �ִ��� �˻��Ѵ�.
	pExistData = UserFind(strUsrID);	
	if (!pExistData) // ����ڰ� ����
		return;

	// ����ڰ� ������ ���� ������Ʈ
	pExistData->nUsrNum = nUsrNum;
	pExistData->nChaNum = nChaNum;
	pExistData->dwGaeaID = dwGaeaID;	
	::StringCchCopy(pExistData->szChaName, CHR_ID_LENGTH+1, szChaName); // �÷������� ĳ���͸�
}

///////////////////////////////////////////////////////////////////////////////
// User management function
///////////////////////////////////////////////////////////////////////////////

LPCNT_USR_INFO NetSessionClientMan::UserFind(const CString& strUsrID)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_CS_UserMap);
	BOOL bRetCode = 0;
	LPCNT_USR_INFO pData = NULL;
	bRetCode = m_pUsrMap->Lookup(strUsrID, (void*&) pData);
	if (bRetCode == 0)
		return NULL;
	else
		return pData;
}

// ���� ������ ������ ����Ѵ�.
void NetSessionClientMan::UserInsert(const CString& strUsrID, LPCNT_USR_INFO pData)
{	
	if (!pData)
        return;

    sc::CriticalSectionOwnerLock OwnerLock(m_CS_UserMap);

	LPCNT_USR_INFO pExistData = NULL;	
	
	// ������ ���ӵǾ� �ִ��� �˻��Ѵ�.
	pExistData = UserFind(strUsrID);
	if (pExistData != NULL) // �̹� ���ӵǾ� ������ ���������ڸ� �����Ѵ�.
		UserErase(strUsrID);

	LPCNT_USR_INFO pDataTemp = new CNT_USR_INFO;

	pDataTemp->dwGameClient	= pData->dwGameClient;
	pDataTemp->dwSvrClient  = pData->dwSvrClient;
	pDataTemp->nSvrGrp      = pData->nSvrGrp;
	pDataTemp->nSvrNum      = pData->nSvrNum;
	pDataTemp->nUsrNum      = pData->nUsrNum;
	::StringCchCopy(pDataTemp->szUserID, USR_ID_LENGTH+1, pData->szUserID); // ����� ID
	m_pUsrMap->SetAt(strUsrID, (LPCNT_USR_INFO) pDataTemp);
}

BOOL NetSessionClientMan::UserErase(const CString& strUsrID)
{
    sc::CriticalSectionOwnerLock OwnerLock(m_CS_UserMap);

    BOOL bFind;
	LPCNT_USR_INFO pData;
	bFind = m_pUsrMap->Lookup(strUsrID, (void*&) pData);
	if (bFind == 0)	
    {
		return FALSE; // Can't find, return.
    }
	else
	{
		// Find and delete, erase.
		delete pData;
		m_pUsrMap->RemoveKey(strUsrID);
		return TRUE;
	}
}

void NetSessionClientMan::UserEraseServer(int nSvrGrp, int nSvrNum)
{	
	sc::CriticalSectionOwnerLock OwnerLock(m_CS_UserMap);

    CString			strKey;
    LPCNT_USR_INFO	pData;
	// Remove the elements with even key values.
	POSITION pos = m_pUsrMap->GetStartPosition();
	if (pos) 
	{		
	    // Loop all elements and find nSvrGrp, nSvrNum, delete element	
	    while (pos)
	    {
		    m_pUsrMap->GetNextAssoc(pos, strKey, (void*&) pData);

		    if (pData->nSvrGrp == nSvrGrp && pData->nSvrNum == nSvrNum)
		    {
			    delete pData;
			    m_pUsrMap->RemoveKey(strKey);
		    }
	    }
    }
}

void NetSessionClientMan::UserEraseAll()
{
    sc::CriticalSectionOwnerLock OwnerLock(m_CS_UserMap);
    
    CString			strKey;
	LPCNT_USR_INFO	pData;
	POSITION		pos;

	// Remove the elements with even key values.
	pos = m_pUsrMap->GetStartPosition();
	if (pos) 
	{
		// Loop all elements and delete, erase.		
		while (pos)
		{
			m_pUsrMap->GetNextAssoc(pos, strKey, (void*&) pData);
			delete pData;
			m_pUsrMap->RemoveKey(strKey);			
		}
	}
}

void NetSessionClientMan::PrintUsrList()
{
	CString			strKey;
	LPCNT_USR_INFO	pData;	
	POSITION		pos = m_pUsrMap->GetStartPosition();
	
	if (pos == NULL) return;
	
	// Loop all elements and delete, erase.
	while (pos != NULL)
	{		
		m_pUsrMap->GetNextAssoc(pos, strKey, (void*&) pData);
		sc::writeLogInfo(sc::string::format("(%s:%d)(%d:%d)(%d:%d)", strKey, pData->nUsrNum, pData->nSvrGrp, pData->nSvrNum, pData->dwSvrClient, pData->dwGameClient ));
	}
}

bool NetSessionClientMan::IsGameServer(DWORD dwClient)
{
	// server type [type]
	// 1 : Login server
    // 2 : Session server
    // 3 : Field server
    // 4 : Agent server	
    G_SERVER_INFO* pData = GetServerInfoData(dwClient);
    if (pData)
        return pData->ServerType == net::SERVER_AGENT ? true : false;
    else
		return false;
}