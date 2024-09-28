#include "../pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Net/Overlapped.h"
#include "../../SigmaCore/Math/SeqUniqueGuid.h"

#include "../../RanLogic/Network/SendMsgBuffer.h"

#include "../FieldServer/GLGaeaServer.h"
#include "./NetUser.h"
#include "./NetClientManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

NetClientManager::NetClientManager( int nMaxClient, HANDLE hIOServer, int nReferenceCountMax )
	: m_nMaxClient(nMaxClient)
	, m_hIOServer(hIOServer)
	, m_dwInPacketCount(0) //< ���ŵ� �� ��Ŷ ����
	, m_dwInPacketSize(0) //< ���ŵ� �� ��Ŷ ������
	, m_dwOutPacketCount(0) //< �۽��� �� ��Ŷ ����
	, m_dwOutPacketSize(0) //< �۽��� �� ��Ŷ ������
	, m_dwOutCompressCount(0) //< �۽��� �� ���� ��Ŷ ����
	, m_dwOutCompressSize(0) //< �۽��� �� ���� ��Ŷ ũ��	
	, m_nReferenceCountMax( nReferenceCountMax )
{	    
	InitializeCriticalSectionAndSpinCount(&m_CS_ClientMan, sc::CRITICAL_SECTION_SPIN_COUNT);
	InitializeCriticalSectionAndSpinCount(&m_CS_CID, sc::CRITICAL_SECTION_SPIN_COUNT);
	InitializeCriticalSectionAndSpinCount(&m_CS_ChaNum, sc::CRITICAL_SECTION_SPIN_COUNT);

	//srand((unsigned) time(NULL));
    m_ClientMan.reserve(m_nMaxClient);
	
    for (UINT i=0; i<m_nMaxClient; ++i)
        m_ClientMan.push_back(std::tr1::shared_ptr<CNetUser> (new CNetUser));
	
	//mjeon.TBB
#ifdef _USE_TBB
	
	for (UINT i=1; i<m_nMaxClient; ++i)
	{
		m_cqCID_Consumer.push(i);
	}
	
	m_cqCID_Producer.clear();
	
#else
	// 0��(DBACTION_CLIENT_ID)�� ���μ����� ��ſ� ���Ǳ� ������ ������� �ʴ´�.
    // ex) DB->Server
	for (size_t i=DBACTION_CLIENT_ID + 1; i<m_nMaxClient; ++i)
	{
		m_deqSleepCID.push_back(i);
		m_mapSleepCID.insert(MAP_CLIENTS_VALUE(i, TRUE));
	}

	m_vecSleepCID.reserve(m_nMaxClient);
#endif
	
    m_pIoMan = new sc::net::IOManager;
}

NetClientManager::~NetClientManager()
{	
    for (size_t i=0; i<m_nMaxClient; ++i)
        m_ClientMan[i].reset();
    SAFE_DELETE(m_pIoMan);

    DeleteCriticalSection(&m_CS_ClientMan);
	DeleteCriticalSection(&m_CS_CID);
	DeleteCriticalSection(&m_CS_ChaNum);
}

bool NetClientManager::VerifyIP( const std::string& IpAddress )
{
	MAP_CONNECTED_IP::iterator it = m_mapConnectedIP.find( IpAddress );
	if ( it == m_mapConnectedIP.end() )
	{
		m_mapConnectedIP.insert( std::make_pair( IpAddress, sIPConnectCount() ) );
		return true;
	}
	else
	{
		sIPConnectCount& sCount = it->second;
		sCount.nCount += 1;
		DWORD dwCurrentTime = GetTickCount();
		if ( dwCurrentTime - sCount.dwLastConnectTime < 5000 )
		{
			return false;
		}
		sCount.dwLastConnectTime = dwCurrentTime;
	}
	return true;
}

bool NetClientManager::VerifyUserNum( DWORD dwUserDbNum, EMSERVICE_PROVIDER emSP )
{
	DWORD dwCurrentTime = GetTickCount();
	MAP_CONNECTED_USER_NUM::iterator it = m_mapConnectedUserNum.find( dwUserDbNum );
	if ( it == m_mapConnectedUserNum.end() )
	{
		m_mapConnectedUserNum.insert( std::make_pair( dwUserDbNum, dwCurrentTime ) );
		return true;
	}
	else
	{
		DWORD& dwConnectedTime = it->second;
		const DWORD dwCheckTime( emSP == SP_OFFICE_TEST? 5000 : 5001 );
		if ( dwCurrentTime - dwConnectedTime < dwCheckTime )
		{
			return false;
		}
		dwConnectedTime = dwCurrentTime;
	}
	return true;
}

void NetClientManager::UpdateUserLogoutTime( DWORD dwUserDbNum )
{
	DWORD dwCurrentTime = GetTickCount();
	MAP_CONNECTED_USER_NUM::iterator it = m_mapConnectedUserNum.find( dwUserDbNum );
	if ( it == m_mapConnectedUserNum.end() )
	{
		m_mapConnectedUserNum.insert( std::make_pair( dwUserDbNum, dwCurrentTime ) );
	}
	else
	{
		DWORD& dwConnectedTime = it->second;
		dwConnectedTime = dwCurrentTime;
	}
}

CRYPT_KEY NetClientManager::GetNewCryptKey()
{
	sc::MGUID sGUID;
	sc::SeqUniqueGUID::Instance()->getGUID( sGUID );

	CRYPT_KEY ck;
	// ck.nKeyDirection	= rand() % 2 + 1; // Direction Left or Right
	// ck.nKey			= rand() % 5 + 2; // Shift amount

	//ck.nKeyDirection	= 1; // Direction Left or Right	
	//ck.nKey				= 1; // Shift amount
	ck.guid = sGUID;
	return ck;
}

//! ���� ����(login) ����Ͽ����� ����.
bool NetClientManager::IsAccountPass(DWORD dwClient) const
{
	//GASSERT(IsClientSlot(dwClient));
	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->IsAccountPass();
	else
		return false;
}

void NetClientManager::SetAccountPass (DWORD dwClient, bool bPASS) // ���� ����(login) ��� ����.
{
	//GASSERT(IsClientSlot(dwClient));
	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->SetAccountPass(bPASS);
}

int NetClientManager::IsUse2ndPass(DWORD dwClient) const
{
	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->IsUse2ndPass();
	else
		return false;
}

void NetClientManager::SetUse2ndPass (DWORD dwClient, int bUse2ndPass)
{
	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->SetUse2ndPass(bUse2ndPass);
}

CRYPT_KEY NetClientManager::GetCryptKey(DWORD dwClient)
{
	//GASSERT(IsClientSlot(dwClient));
	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->GetCryptKey();
	else
	{
		CRYPT_KEY sKey;
		return sKey;
	}
}

void NetClientManager::SetCryptKey(DWORD dwClient, CRYPT_KEY ck)
{
	//GASSERT(IsClientSlot(dwClient));
	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->SetCryptKey(ck);
}

void NetClientManager::ResetUserID(DWORD dwClient)
{
	//GASSERT(IsClientSlot(dwClient));
	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->ResetUserID();
}

bool NetClientManager::IsOnline(DWORD dwClient)
{
	//GASSERT(dwClient<(DWORD)m_nMaxClient);
    if (dwClient < m_nMaxClient)
	    return m_ClientMan[dwClient]->IsOnline();
    else
        return false;
}

/*
bool NetClientManager::IsOffline(DWORD dwClient)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	return m_ClientMan[dwClient]->IsOffline();
}
*/

// GetFreeClientID
// Return the free client slot
int	NetClientManager::GetFreeClientID(bool bForce)
{
#ifdef _USE_TBB

	//mjeon.lock-free�ϰ� CID�� �����ϸ�, UpdateThread���� �Ź� �������� �ʾƵ� �ȴ�. 
	// �� ��� ResetPreSleepCID()�� �ƹ��� ó���� ���� �ʴ� dummy�Լ��̴�.

	UINT	nSize = m_cqCID_Consumer.unsafe_size();
	if( nSize < NUM_OF_RESERVED_CID_FOR_SERVER_CONNECTIONS )
	{
		UINT		nFreedCID;
		SET_CID		setCID_Filter;
		sc::CriticalSectionOwnerLock OwnerLock( m_CS_CID );
		while( m_cqCID_Producer.try_pop( nFreedCID ) )
		{
			if( setCID_Filter.insert( nFreedCID ).second )
				m_cqCID_Consumer.push( nFreedCID );
			else
				sc::writeLogError( sc::string::format( "GetFreeClientID() - Already CloseClient %1%", nFreedCID ) );
		} //while
		nSize = m_cqCID_Consumer.unsafe_size();
	} //if

	if( nSize >= NUM_OF_RESERVED_CID_FOR_SERVER_CONNECTIONS || bForce )
	{
		UINT	nClient = NET_ERROR;
		if( m_cqCID_Consumer.try_pop( nClient ) )
			return nClient;
	} //if

	sc::writeLogError( std::string( "GetFreeClientID() - There is not enough CID." ) );
	return NET_ERROR;

#else
	sc::CriticalSectionOwnerLock OwnerLock(m_CS_CID);
	if (m_deqSleepCID.empty())
	{
		// m_vecSleepCID queue�� ���¸� üũ
		ResetPreSleepCID();
		return NET_ERROR;
	}
	else
	{
        // 30���� ������ ������ ���ؼ� ���ܵд�.
        if (m_deqSleepCID.size() < NUM_OF_RESERVED_CID_FOR_SERVER_CONNECTIONS && !bForce)
        {
            return NET_ERROR;
        }

		int nClient = m_deqSleepCID.front();
		m_deqSleepCID.pop_front();

        MAP_CLIENTS_ITER iter = m_mapSleepCID.find(nClient);
        if (iter != m_mapSleepCID.end())
		    m_mapSleepCID.erase(iter);

		//m_ClientMan[nClient]->SetOnLine(); // ?
		return nClient;
	}
#endif
}

void NetClientManager::ReleaseClientID(DWORD dwClient)
{
	if ( dwClient < m_nMaxClient )
	{
#ifdef _USE_TBB

		m_cqCID_Producer.push(dwClient);

#else
		sc::CriticalSectionOwnerLock OwnerLock(m_CS_CID);
		VEC_CLIENTS_CITER iter = std::find(m_vecSleepCID.begin(), m_vecSleepCID.end(), dwClient);
		if (iter == m_vecSleepCID.end())
			m_vecSleepCID.push_back(dwClient);
#endif
	}
}

#ifndef _USE_TBB
//! ���� Ŭ���̾�Ʈ ID�� ��Ƽ� �Ѳ����� ���. (Ŭ���̾�Ʈ ID�� ���� ����� ����)
void NetClientManager::ResetPreSleepCID()
{
	//DO NOTHING with TBB
	sc::CriticalSectionOwnerLock OwnerLock(m_CS_CID);
	int nTemp = -1;
	size_t vecSize = m_vecSleepCID.size();
	for (size_t i=0; i<vecSize; ++i)
	{
		if (nTemp != m_vecSleepCID[i])
		{
			if (m_mapSleepCID.count(m_vecSleepCID[i]) == 0)
			{
				m_deqSleepCID.push_back(m_vecSleepCID[i]);
				m_mapSleepCID.insert(MAP_CLIENTS_VALUE(m_vecSleepCID[i], TRUE));
				nTemp = m_vecSleepCID[i];
			}
			else
			{
				sc::writeLogError(
                    sc::string::format(
                        "Already CloseClient %1%", m_vecSleepCID[i]));
			}
		}
		else
		{
			sc::writeLogError(
                sc::string::format(
                    "ResetPreSleepCID SleepCID %1%" , m_vecSleepCID[i]));
		}
	}
	m_vecSleepCID.clear();
}
#endif

int NetClientManager::GetMaxClient()
{
	return m_nMaxClient;
}

// Ŭ���̾�Ʈ�� ó�� ���������� ȣ���
// Ŭ���̾�Ʈ�� ip, port, ���ӽð��� �����.
int NetClientManager::SetAcceptedClient(DWORD dwClient, SOCKET sSocket, net::SLOT_TYPE slotType)
{
	if ( dwClient >= m_nMaxClient )
	{
		sc::writeLogError(
			sc::string::format(
			"Accept Client Failed. Client Slot wrong :%1%",
			dwClient ) );
		return NET_ERROR;
	}

	int nRetCode = 0;
    std::tr1::shared_ptr<CNetUser> spData = m_ClientMan[dwClient];
	if (!spData)
        return NET_ERROR;

	if (spData->SetAcceptedClient(sSocket) == NET_OK)
	{
		spData->SetSlotType(slotType);

		sc::writeLogInfo(
            sc::string::format(
                "Client Slot:%1% - %2%:%3%",
                dwClient,
                spData->GetIP(),
                spData->GetPort()));
		return NET_OK;
	}
	else
	{
		sc::writeLogError(
            sc::string::format(
                "Accept Client Failed. Client Slot:%1% - %2%:%3%",
                dwClient,
                spData->GetIP(),
                spData->GetPort()));
		return NET_ERROR;
	}
}

void NetClientManager::CloseAllClient()
{	
	for (UINT i=0; i<m_nMaxClient; ++i)
		CloseClient(i);
}

//! CloseClient
//! Close client socket 
int NetClientManager::CloseClient( DWORD dwClient )
{
	sc::CriticalSectionOwnerLock OwnerLock( m_CS_ChaNum );

	if ( dwClient >= m_nMaxClient )
	{
		return NET_ERROR;
	}

	// ������ ���´�.
	int nResult = m_ClientMan[dwClient]->CloseClient( dwClient );
	if ( NET_OK == nResult )
	{
		// Ŭ���̾�Ʈ ��ȣ�� ���޽��Կ� ���� �ִ´�.
		ReleaseClientID( dwClient );
	}

	DWORD dwChaNum = m_ClientMan[dwClient]->GetChaDbNum();
	m_mapChaNum.erase( dwChaNum );

	return nResult;
}

//! Return current client number
int	NetClientManager::GetCurrentClientNumber(void)
{
#ifdef _USE_TBB

	/*
	unsafe_size()

	Returns number of items in queue. May return incorrect value 
	if any push or try_pop operations are concurrently in flight.
	*/

	return (m_nMaxClient - (UINT)( m_cqCID_Consumer.unsafe_size() + m_cqCID_Producer.unsafe_size() ));
	
#else
	return m_nMaxClient - (int) m_deqSleepCID.size();
#endif
}

std::string NetClientManager::GetClientIP(DWORD dwClient) const
{
	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->GetIP();
	else
		return std::string( "wrong index client." );
}

USHORT NetClientManager::GetClientPort(DWORD dwClient)
{
	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->GetPort();
	else
		return 0;
}

int	NetClientManager::addRcvMsg(DWORD dwClient, void* pMsg, DWORD dwSize)
{
	ASSERT(pMsg);

	if ( dwClient < m_nMaxClient )
	{
		// ���Ź��� ũ�� �� ī���� ���
		m_dwInPacketCount++;
		m_dwInPacketSize += dwSize;
		return m_ClientMan[dwClient]->addRcvMsg(pMsg, dwSize);
	}
	else
	{
		return 0;
	}
}

void* NetClientManager::getRcvMsg(DWORD dwClient)
{
	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->getRcvMsg();
	else
		return NULL;
}

// �޽����� ���� ���۸� �����Ѵ�
void NetClientManager::ResetRcvBuffer(DWORD dwClient)
{
	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->ResetRcvBuffer();
}

SOCKET NetClientManager::GetSocket(DWORD dwClient)
{
	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->GetSOCKET();
	else
		return INVALID_SOCKET;
}

void NetClientManager::CloseSocket(DWORD dwClient)
{
	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->CloseSocket();
}

void NetClientManager::SendClientFinal()
{
	DWORD dwSendSize = 0;
	for (DWORD dwClient=0; dwClient < (DWORD) m_nMaxClient; ++dwClient)
	{
		dwSendSize = m_ClientMan[dwClient]->getSendSize();
		if (dwSendSize > 0) 
		{
			// ����� ������ ����
			m_dwOutCompressCount++;
			m_dwOutCompressSize += dwSendSize;
			SendClient2( dwClient, ( NET_MSG_GENERIC* ) m_ClientMan[dwClient]->getSendBuffer() );
		}
	}
}

int	NetClientManager::SendClient(DWORD dwClient, NET_MSG_GENERIC* pBuffer)
{
	if (!pBuffer)
        return NET_ERROR;

	if ( dwClient >= m_nMaxClient )
		return NET_ERROR;

	DWORD dwSendSize = pBuffer->Size();
	m_dwOutPacketCount++;
	m_dwOutPacketSize += dwSendSize;

	int nResult = m_ClientMan[dwClient]->addSendMsg(pBuffer, dwSendSize);
	
	switch (nResult) 
	{
	case CSendMsgBuffer::BUFFER_ERROR:
		return NET_ERROR;
		break;

	case CSendMsgBuffer::BUFFER_ADDED:		
		break;

	case CSendMsgBuffer::BUFFER_SEND:
		//
		//mjeon
		//Msg is Compressed in CSendMsgBuffer::getSendSize() method.
		//And it will be Decompressed in RcvMsgBuffer::getMsg() method.
		//
		dwSendSize = m_ClientMan[dwClient]->getSendSize();
		if (dwSendSize > 0) 
		{
			// ����� ������ ����
			m_dwOutCompressCount++;
			m_dwOutCompressSize += dwSendSize;
			SendClient2(dwClient, (NET_MSG_GENERIC*) m_ClientMan[dwClient]->getSendBuffer());
		}
		break;

	case CSendMsgBuffer::BUFFER_SEND_ADD:
		//
		//mjeon
		//Msg is Compressed in CSendMsgBuffer::getSendSize() method.
		//And it will be Decompressed in RcvMsgBuffer::getMsg() method.
		//
		dwSendSize = m_ClientMan[dwClient]->getSendSize();		
		if (dwSendSize > 0) 
		{
			// ����� ������ ����
			m_dwOutCompressCount++;
			m_dwOutCompressSize += dwSendSize;
			SendClient2(dwClient, (NET_MSG_GENERIC*) m_ClientMan[dwClient]->getSendBuffer());			
			m_ClientMan[dwClient]->addSendMsg(pBuffer, pBuffer->Size());
		}
		break;

	default:
		return NET_ERROR;
		break;
	}

	return NET_OK;
}

int	NetClientManager::SendClient2( DWORD dwClient, NET_MSG_GENERIC* pBuffer )
{
	SOCKET sSocket = GetSocket( dwClient );
	if ( sSocket == INVALID_SOCKET )
	{
		return NET_ERROR;
	}

	if ( pBuffer && pBuffer->Size() <= NET_DATA_BUFSIZE )
	{		
		PER_IO_OPERATION_DATA* pIoWrite = ( PER_IO_OPERATION_DATA* ) GetFreeOverIO( NET_SEND_POSTED );		
		if ( pIoWrite )
		{	
			CopyMemory( pIoWrite->Buffer, pBuffer, pBuffer->Size() );
			pIoWrite->dwTotalBytes = pBuffer->Size();
			
			return SendClient2( dwClient, pIoWrite, pBuffer->Size() );
		}
        else
        {
			std::string Msg( "critical error out of send io memory." );
			sc::writeLogError( Msg );
            return NET_ERROR;
        }
    }
    else
    {
        return NET_ERROR;
    }
}

int NetClientManager::SendClient2( DWORD dwClient, PER_IO_OPERATION_DATA* PerIoData, DWORD dwSize )
{
	// MSG_OOB
	// MSG_DONTROUTE
	// MSG_PARTIAL
	INT		nRetCode  = 0;
	DWORD	dwFlags   = 0;
	DWORD	dwSndSize = dwSize;

	PerIoData->OperationType = NET_SEND_POSTED;
	PerIoData->DataBuf.len = static_cast< u_long >( dwSize );
	
	SOCKET sSocket = GetSocket( dwClient );
	if ( sSocket == INVALID_SOCKET )
	{
		ReleaseOperationData( PerIoData );
		return NET_ERROR;
	}

	AddReference( dwClient );

	nRetCode = ::WSASend(sSocket,
					     &( PerIoData->DataBuf ),
					     1,
					     &dwSndSize,
					     dwFlags,
					     &( PerIoData->Overlapped ),
					     NULL );
	if ( nRetCode == SOCKET_ERROR ) 
	{
		nRetCode = ::WSAGetLastError();
		
		if ( WSA_IO_PENDING != nRetCode ) 
		{
			::PostQueuedCompletionStatus( m_hIOServer, 0, dwClient, &PerIoData->Overlapped );

			sc::writeLogError(
				sc::string::format(
				"NetClientManager::SendClient2 Client %1% ERR %2%",
				dwClient, nRetCode ) );

			return NET_ERROR;
		}
	}

	return NET_OK;
}

void NetClientManager::SetSlotType(DWORD dwClient, net::SLOT_TYPE emType)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	m_ClientMan[dwClient]->SetSlotType(emType);
}

bool NetClientManager::IsClientSlot(DWORD dwClient) const
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	return m_ClientMan[dwClient]->IsClientSlot();
}

bool NetClientManager::IsAgentSlot(DWORD dwClient)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	return m_ClientMan[dwClient]->IsAgentSlot();
}

net::SLOT_TYPE NetClientManager::GetSlotType(DWORD dwClient)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	return m_ClientMan[dwClient]->GetSlotType();
}

void NetClientManager::ResetHeartBeat(DWORD dwClient)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	m_ClientMan[dwClient]->ResetHeartBeat();
}

void NetClientManager::SetHeartBeat(DWORD dwClient)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	m_ClientMan[dwClient]->SetHeartBeat();
}

bool NetClientManager::CheckHeartBeat(DWORD dwClient)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	return m_ClientMan[dwClient]->CheckHeartBeat();
}

void NetClientManager::GspSetUserID( DWORD dwClient, const TCHAR* szGspUserID )
{
	//GASSERT(IsClientSlot(dwClient)&&"GspSetUserID()");

	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->GspSetUserID( szGspUserID );
}

TCHAR* NetClientManager::GspGetUserID( DWORD dwClient )
{
	//GASSERT(IsClientSlot(dwClient)&&"GspGetUserID()");

	if (!IsClientSlot(dwClient))
		return NULL;

	return m_ClientMan[dwClient]->GspGetUserID();
}

void NetClientManager::SetUserID( DWORD dwClient, const TCHAR* szUserID )
{
	//GASSERT(IsClientSlot(dwClient)&&"SetUserID()");
	GASSERT(szUserID != NULL);

	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->SetUserID( szUserID );
}

void NetClientManager::SetUserID(DWORD dwClient, const std::string& UserID)
{
	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->SetUserID(UserID);
}

TCHAR* NetClientManager::GetUserID( DWORD dwClient )
{
	//GASSERT(IsClientSlot(dwClient)&&"GetUserID()");

	static TCHAR s_szErrUserId[] = _T("[err_userid]");
	if (!IsClientSlot(dwClient))
		return s_szErrUserId;

	return m_ClientMan[dwClient]->GetUserID();
}

void NetClientManager::SetUserDbNum(DWORD dwClient, INT nUserNum)
{
	//GASSERT(IsClientSlot(dwClient)&&"SetUserDbNum()");

	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->SetUserDbNum(nUserNum);
}

void NetClientManager::SetLoginTime(DWORD dwClient)
{
	//GASSERT(IsClientSlot(dwClient)&&"SetLoginTime()");

	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->SetLoginTime();
}

void NetClientManager::SetGaeaID(DWORD dwClient, DWORD dwGaeaID)
{
	//GASSERT(IsClientSlot(dwClient)&&"SetGaeaID()");
	GASSERT(dwGaeaID != GAEAID_NULL);

	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->SetGaeaID(dwGaeaID);
}

void NetClientManager::ResetGaeaID(DWORD dwClient)
{
	//GASSERT(IsClientSlot(dwClient)&&"SetGaeaID()");

	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->SetGaeaID(GAEAID_NULL);
}

INT NetClientManager::GetLoginTime(DWORD dwClient)
{
	//GASSERT(IsClientSlot(dwClient)&&"GetLoginTime()");

	if ( dwClient < m_nMaxClient )
	{
		CTime crtTime;
		crtTime = CTime::GetCurrentTime();
		CTimeSpan ts = crtTime - m_ClientMan[dwClient]->GetLoginTime();
		return (INT) ts.GetTotalMinutes();
	}
	else
		return 0;
}

CTime NetClientManager::GetLoginTimeEx(DWORD dwClient)
{
	//GASSERT(IsClientSlot(dwClient)&&"GetLoginTimeEx()");

	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->GetLoginTime();	 
	else
	{
		CTime sTime;
		return sTime;
	}
}

int NetClientManager::UserDbNum(DWORD dwClient) const
{
	//GASSERT(IsClientSlot(dwClient)&&"UserDbNum()");

	if (!IsClientSlot(dwClient))
		return -1;

	if (dwClient >= (DWORD) m_nMaxClient)
	{
		GASSERT(0&&"m_nMaxClient30");
		return -1;
	}
	
	return m_ClientMan[dwClient]->UserDbNum();
}

DWORD NetClientManager::GetGaeaID( DWORD dwClient )
{
	if ( !IsClientSlot( dwClient ) || !IsOnline( dwClient ) )
	{
		/*bool bOnline = IsOnline( dwClient );
		net::SLOT_TYPE slotType = GetSlotType( dwClient );
		sc::writeLogError( 
			sc::string::format( 
			"NetClientManager::GetGaeaID() - [%1%]dwClient(%2%) is not a client slot but it's slotType is %3%!", 
			bOnline ? "ONLINE" : "OFFLINE", 
			dwClient, 
			( UINT )slotType ) );*/

		return GAEAID_NULL;
	}

	return m_ClientMan[dwClient]->GetGaeaID();
}

// �����ڰ� �÷��� �������� �˷��ش�.
bool NetClientManager::IsGaming(DWORD dwClient)
{
	if ( dwClient >= m_nMaxClient )
		return false;

	// �¶��� �����̰� ���̾� ID �� ������ �������̴�
    if (m_ClientMan[dwClient]->IsOnline() && m_ClientMan[dwClient]->GetGaeaID() != GAEAID_NULL && m_ClientMan[dwClient]->IsClientSlot())
		return true;
	else
		return false;
}

void NetClientManager::SetChaNum(DWORD dwClient, int nChaNum)
{
	sc::CriticalSectionOwnerLock OwnerLock(m_CS_ChaNum);
	//GASSERT(IsClientSlot(dwClient)&&"SetChaNum()");

	if ( dwClient < m_nMaxClient )
	{
		m_ClientMan[dwClient]->ChaDbNum(nChaNum);
		m_ClientMan[dwClient]->SetSlotType(net::SLOT_CLIENT);	//���⿡�� �ѹ��� slot type�� ����������

		m_mapChaNum[nChaNum] = dwClient;	//mjeon.AF
	}
}

int	NetClientManager::ChaDbNum(DWORD dwClient) const
{
	//GASSERT(IsClientSlot(dwClient)&&"ChaDbNum()");

	if (!IsClientSlot(dwClient))
		return -1;

	return m_ClientMan[dwClient]->GetChaDbNum();
}

DWORD NetClientManager::GetClientByUserNum ( DWORD dwUserNum )
{
	for ( int i=0; i<m_ClientMan.size(); ++i )
	{
		if ( m_ClientMan.at( i )->UserDbNum() == dwUserNum )
			return i;
	}

	return -1;
}

DWORD NetClientManager::GetClientID(DWORD dwChaNum)
{
	sc::CriticalSectionOwnerLock OwnerLock(m_CS_ChaNum);
	
	/*
	if (m_mapChaNum.count(dwChaNum) <= 0)
		return CLIENTID_NULL;

	DWORD dwClient = m_mapChaNum[dwChaNum];
	*/
	MAP_CHANUM_ITER iter = m_mapChaNum.find(dwChaNum);
	if (iter == m_mapChaNum.end())
		return CLIENTID_NULL;
	
	DWORD dwClient = iter->second;

/*
#ifdef _RELEASED
	if (!IsClientSlot(dwClient))
	{
		//IsClientSlot���� �����ؼ� GASSERT�� �ɸ��� ��츦 �����ϱ� ���� �ӽ� ����� �ڵ�
		net::SLOT_TYPE slotType = GetSlotType(dwClient);

		sc::writeLogDebug(stringformat("NetClientManager::GetClientID - Char(%1%)'s IsClientSlot(%2%) failure. It's slotType is %3%", dwChaNum, dwClient, slotType));
	}
#endif
*/
	////GASSERT(IsClientSlot(dwClient));	//��� ĳ���Ͱ� ��������(CNetUser::Reset() ȣ��)������ ���� m_mapChaNum�� �����Ǳ� �� ������ ����� ���� �� �ִ�.
	if (!IsClientSlot(dwClient))
	{
		m_mapChaNum.erase(dwChaNum);

		return CLIENTID_NULL;
	}

	return dwClient;
}

void NetClientManager::SetUserType(DWORD dwClient, int nType)
{
	//GASSERT(IsClientSlot(dwClient)&&"SetUserType()");

	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->SetUserType(nType);
}

int NetClientManager::GetUserType(DWORD dwClient)
{
	//GASSERT(IsClientSlot(dwClient)&&"GetUserType()");
	
	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->GetUserType();
	else
		return 0;
}

void NetClientManager::SetChaRemain(DWORD dwClient, WORD wChaNum)
{
	//GASSERT(IsClientSlot(dwClient)&&"SetChaRemain()");

	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->SetChaRemain(wChaNum);
}

void NetClientManager::SetChaTestRemain(DWORD dwClient, WORD wChaNum)
{
	//GASSERT(IsClientSlot(dwClient)&&"SetChaTestRemain()");

	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->SetChaTestRemain(wChaNum);
}

WORD NetClientManager::GetChaRemain(DWORD dwClient)
{
	//GASSERT(IsClientSlot(dwClient)&&"GetChaRemain()");

	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->GetChaRemain();
	else
		return 0;
}

WORD NetClientManager::GetChaTestRemain(DWORD dwClient)
{
	//GASSERT(IsClientSlot(dwClient)&&"GetChaTestRemain()");

	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->GetChaTestRemain();
	else
		return 0;
}

__time64_t NetClientManager::GetPremiumDate(DWORD dwClient)
{
	//GASSERT(IsClientSlot(dwClient)&&"GetPremiumDate()");

	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->GetPremiumDate();
	else
		return 0;
}

void NetClientManager::SetPremiumDate(DWORD dwClient, __time64_t tTime)
{
	//GASSERT(IsClientSlot(dwClient)&&"SetPremiumDate()");

	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->SetPremiumDate(tTime);
}

__time64_t NetClientManager::GetChatBlockDate(DWORD dwClient)
{
	//GASSERT(IsClientSlot(dwClient)&&"GetChatBlockDate()");

	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->GetChatBlockDate();
	else
		return 0;
}

void NetClientManager::SetChatBlockDate(DWORD dwClient, __time64_t tTime)
{
	//GASSERT(IsClientSlot(dwClient)&&"SetChatBlockDate()");

	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->SetChatBlockDate(tTime);
}

Country::SCOUNTRY_INFO NetClientManager::GetCountryInfo ( DWORD dwClient )
{
	if ( dwClient < m_nMaxClient )
		return m_ClientMan[ dwClient ]->GetUserCountry();
	else
		return Country::SCOUNTRY_INFO();
}

void NetClientManager::SetCountryInfo ( DWORD dwClient, const Country::SCOUNTRY_INFO& sCountryInfo )
{
	if ( dwClient < m_nMaxClient )
		m_ClientMan[ dwClient ]->SetUserCountry ( sCountryInfo );
}

void NetClientManager::SetChannel(DWORD dwClient, int nChannel)
{
	////GASSERT(IsClientSlot(dwClient)&&"SetChannel()");

	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->SetChannel(nChannel);
}

int NetClientManager::GetChannel(DWORD dwClient)
{
	//GASSERT(IsClientSlot(dwClient)&&"GetChannel()");
	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->GetChannel();
	return 0;
}

//! nProtect GameGuard ������ ������ �������� ���� ȣ��
bool NetClientManager::nProtectSetAuthQuery(DWORD dwClient)
{
	//GASSERT(IsClientSlot(dwClient)&&"nProtectSetAuthQuery()");

	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->nProtectSetAuthQuery();
	else
		return false;
}

int NetClientManager::nProtect30SetAuthQuery(DWORD dwClient)
{
	//GASSERT(IsClientSlot(dwClient)&&"nProtectSetAuthQuery()");

	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->nProtect30SetAuthQuery();
	else
		return false;
}
    	
//! nProtect GameGuard ������ ���� ��������
GG_AUTH_DATA NetClientManager::nProtectGetAuthQuery(DWORD dwClient)
{
	//GASSERT(IsClientSlot(dwClient)&&"nProtectGetAuthQuery()");

	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->nProtectGetAuthQuery();
	else
	{
		// dummy data
		GG_AUTH_DATA sData;
		return sData;
	}
}

BYTE* NetClientManager::nProtect30GetAuthQuery(DWORD dwClient)
{
	//GASSERT(IsClientSlot(dwClient)&&"nProtectGetAuthQuery()");

	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->nProtect30GetAuthQuery();
	else
	{
		// dummy data
		return NULL;
	}
}

//! nProtect GameGuard ���� ���� ��������ClientTrigger
GG_AUTH_DATA NetClientManager::nProtectGetAuthAnswer( DWORD dwClient )
{
	//GASSERT(IsClientSlot(dwClient)&&"nProtectGetAuthAnswer()");

	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->nProtectGetAuthAnswer();
	else
	{
		// dummy data
		GG_AUTH_DATA sData;
		return sData;
	}
}

BYTE* NetClientManager::nProtect30GetAuthAnswer( DWORD dwClient )
{
	//GASSERT(IsClientSlot(dwClient)&&"nProtectGetAuthAnswer()");

	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->nProtect30GetAuthAnswer();
	else
	{
		return NULL;
	}
}

//! nProtect GameGuard ���� ���� ����
void NetClientManager::nProtectSetAuthAnswer(DWORD dwClient, GG_AUTH_DATA& ggad)
{
	//GASSERT(IsClientSlot(dwClient)&&"nProtectSetAuthAnswer()");

	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->nProtectSetAuthAnswer(ggad);
}

void NetClientManager::nProtect30SetAuthAnswer(DWORD dwClient, BYTE* ggad, int nPacketSeq)
{
	//GASSERT(IsClientSlot(dwClient)&&"nProtectSetAuthAnswer()");

	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->nProtect30SetAuthAnswer(ggad, nPacketSeq);
}

//! nProtect GameGuard ���� ��� ��������
DWORD NetClientManager::nProtectCheckAuthAnswer(DWORD dwClient)
{
	//GASSERT(IsClientSlot(dwClient)&&"nProtectCheckAuthAnswer()");

	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->nProtectCheckAuthAnswer();
	else
		return 0;
}

//! nProtect GameGuard ���� ���¸� ���½�Ų��.
void NetClientManager::nProtectResetAuth(DWORD dwClient)
{
	//GASSERT(IsClientSlot(dwClient)&&"nProtectResetAuth()");

	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->nProtectResetAuth();
}

void NetClientManager::SetRandomPassNumber(DWORD dwClient, int nNumber)
{
	//GASSERT(IsClientSlot(dwClient)&&"SetRandomPassNumber()");

	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->SetRandomPassNumber(nNumber);
}

int NetClientManager::GetRandomPassNumber(DWORD dwClient)
{
	////GASSERT(IsClientSlot(dwClient)&&"GetRandomPassNumber()");

	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->GetRandomPassNumber();
	else
		return -1;
}

void NetClientManager::ResetRandomPassNumber(DWORD dwClient)
{
	//GASSERT(IsClientSlot(dwClient)&&"ResetRandomPassNumber()");

	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->ResetRandomPassNumber();
}

PER_IO_OPERATION_DATA* NetClientManager::GetFreeOverIO(EMMSG_IO_TYPE nType)
{
    return m_pIoMan->GetIO(nType);
}

void NetClientManager::ReleaseOperationData( PER_IO_OPERATION_DATA* pData )
{
    if ( pData )
	{
		if ( !( static_cast< DWORD >( pData->Overlapped.Internal ) != STATUS_PENDING ) )
		{
#if defined(_RELEASED) || defined(_DEBUG)
			sc::writeLogWarn( "ReleaseOperationData HasOverlappedIoCompleted failed." );
#endif
		}
		m_pIoMan->ReleaseIO( pData );
	}
}

//! ��� ��Ŷ ī���͸� �ʱ�ȭ �Ѵ�.
void NetClientManager::resetPacketCount()
{
	m_dwInPacketCount    = 0;
	m_dwInPacketSize     = 0;	
	m_dwOutPacketCount   = 0;
	m_dwOutPacketSize    = 0;
	m_dwOutCompressCount = 0;
	m_dwOutCompressSize  = 0;
}

void NetClientManager::SetExtremeCreate( DWORD dwClient, bool bExtreme )
{
	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->SetExtremeCreate( bExtreme );
}

// �ذ��� ���� ���ɿ��θ� �����´�.
bool NetClientManager::GetExtreme( DWORD dwClient )
{
	//GASSERT(IsClientSlot(dwClient)&&"GetExtremeW()");

	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->GetExtreme();
	else
		return false;
}

void NetClientManager::SetJumpingCreate( DWORD dwClient, bool bJumping )
{
	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->SetJumpingCreate( bJumping );
}

bool NetClientManager::GetJumping( DWORD dwClient )
{
	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->GetJumping();
	else
		return false;
}

void NetClientManager::SetActorCreate ( DWORD dwClient, bool bCreate )
{
	if ( dwClient < m_nMaxClient )
		m_ClientMan[ dwClient ]->SetActorCreate( bCreate );
}

const bool NetClientManager::GetCreateActor ( DWORD dwClient )
{
	if ( dwClient < m_nMaxClient )
		return m_ClientMan[ dwClient ]->GetCreateActor();
	else
		return false;
}

// �±� User Class Type�� �����Ѵ�.
void NetClientManager::SetIPBonusClass( DWORD dwClient, EMIP_BONUS_CLASS emIPBonus )
{
	//GASSERT(IsClientSlot(dwClient)&&"SetIPBonusClass()");

	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->SetIPBonusClass( emIPBonus );
}

// �±� User Class Type�� �����´�.
EMIP_BONUS_CLASS NetClientManager::GetIPBonusClass( DWORD dwClient )
{
	//GASSERT(IsClientSlot(dwClient)&&"GetIPBonusClass()");

	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->GetIPBonusClass();
	else
		return EMIP_BONUS_NONE;
}

// �߱� GameTime�� ������ �д�.
void NetClientManager::SetChinaGameTime( DWORD dwClient, int nChinaGameTime )
{
	//GASSERT(IsClientSlot(dwClient)&&"SetChinaGameTime()");

	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->SetChinaGameTime( nChinaGameTime );
}

// �߱� GameTime�� ������ �´�.
int NetClientManager::GetChinaGameTime( DWORD dwClient )
{
	//GASSERT(IsClientSlot(dwClient)&&"GetChinaGameTime()");

	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->GetChinaGameTime();
	else
		return 0;
}

// �߱� OfflineTime�� ������ �д�.
void NetClientManager::SetChinaOfflineTime( DWORD dwClient, int nChinaOfflineTime )
{
	//GASSERT(IsClientSlot(dwClient)&&"SetChinaOfflineTime()");

	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->SetChinaOfflineTime( nChinaOfflineTime );
}

// �߱� OfflineTime�� ������ �´�.
int NetClientManager::GetChinaOfflineTime( DWORD dwClient )
{
	//GASSERT(IsClientSlot(dwClient)&&"SetChinaOfflineTime()");

	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->GetChinaOfflineTime();
	else
		return 0;
}

// �߱� LastLoginDate�� �����ϰ� ������ �´�.
void NetClientManager::SetLastLoginDate( DWORD dwClient, __time64_t tTime )
{
	//GASSERT(IsClientSlot(dwClient)&&"SetLastLoginDate()");

	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->SetLastLoginDate( tTime );
}

__time64_t NetClientManager::GetLastLoginDate( DWORD dwClient )
{
	//GASSERT(IsClientSlot(dwClient)&&"GetLastLoginDate()");

	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->GetLastLoginDate();
	else
		return 0;
}

// �߱� UserAge�� ������ �д�.
void NetClientManager::SetChinaUserAge( DWORD dwClient, int nChinaUserAge )
{
	//GASSERT(IsClientSlot(dwClient)&&"SetChinaUserAge()");

	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->SetChinaUserAge( nChinaUserAge );
}

// �߱� UserAge�� ������ �´�.
int NetClientManager::GetChinaUserAge( DWORD dwClient )
{
	//GASSERT(IsClientSlot(dwClient)&&"GetChinaUserAge()");

	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->GetChinaUserAge();
	else
		return 0;
}

void NetClientManager::SetVTGameTime( DWORD dwClient, int nVTGameTime )
{
	//GASSERT(IsClientSlot(dwClient)&&"SetVTGameTime()");

	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->SetVTGameTime( nVTGameTime );
}

// ��Ʈ�� GameTime�� ������ �´�.
int NetClientManager::GetVTGameTime( DWORD dwClient )
{
	//GASSERT(IsClientSlot(dwClient)&&"GetVTGameTime()");

	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->GetVTGameTime();
	else
		return 0;
}

bool NetClientManager::IncPacketCount( DWORD dwClient, int nMsgID )
{
	//GASSERT(IsClientSlot(dwClient)&&"IncPacketCount()");

	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->IncPacketCount( nMsgID );
	else
		return false;
}

void NetClientManager::WritePacketLog( DWORD dwClient )
{
	//GASSERT(IsClientSlot(dwClient)&&"WritePacketLog()");

	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->WritePacketLog();
}

void NetClientManager::AddReference( DWORD dwClient, bool bIOCheck/* = true*/ )
{
	// bIOCheck ���Ǽ����� Ŭ���̾�Ʈ�� �������̱� ������ Ȯ������ �ʵ��� �ϱ� �����̴�.
	// �Ϲ����� ������ io��û�� ������ ������������..
	// ���Ǽ������� �� �������� SLOT_CLIENT �̴�.
	// 1000 �� �ڵ忡 �������� ��ɾ������ ���������ϰ� �ؾ��Ѵ�.
	if ( dwClient < m_nMaxClient )
	{
		LONG nReferenceCount = m_ClientMan[dwClient]->GetReferenceCount();
		net::SLOT_TYPE eSlotType = m_ClientMan[dwClient]->GetSlotType();
		if ( 
			nReferenceCount > m_nReferenceCountMax && 
			( net::SLOT_CLIENT == eSlotType || net::SLOT_UNKNOWN == eSlotType ) && 
			true == bIOCheck )
		{
			// io �� �����ִ��� ������ ���´�. �� �����ִ� io�� ó���� ������ m_ClientMan[dwClient] �� ����� �� ����.
			sc::writeLogError( 
				sc::string::format( 
				"AddReference this client will be close. This client is hack user. slottype: %1%, slot: %2%, id: %3%, chanum: %4%, ip: %5%, port: %6%", 
				eSlotType, 
				dwClient, 
				m_ClientMan[dwClient]->GetUserID(), 
				m_ClientMan[dwClient]->GetChaDbNum(), 
				m_ClientMan[dwClient]->GetIP(), 
				m_ClientMan[dwClient]->GetPort() ) );
			CloseClient( dwClient );
			return;
		}
		m_ClientMan[dwClient]->AddReference( dwClient );
	}
}

void NetClientManager::ReleaseReference( DWORD dwClient )
{
	if ( dwClient < m_nMaxClient )
		m_ClientMan[dwClient]->ReleaseReference( dwClient );
}

LONG NetClientManager::GetReferenceCount( DWORD dwClient )
{
	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->GetReferenceCount();
	return 0;
}

DWORD NetClientManager::GetLastRequestIOTime( DWORD dwClient )
{
	if ( dwClient < m_nMaxClient )
		return m_ClientMan[dwClient]->GetLastRequestIOTime();
	return 0;
}

void NetClientManager::CleanDummyClientSlot()
{
	// �ڵ� ���� ����
	// ���
	volatile DWORD dwRemainCount = 0;
	volatile DWORD dwRemainCountWarn = 0;
	DWORD dwCurrentTime = timeGetTime();
	for ( UINT i=1; i<m_nMaxClient; ++i )
	{
		if ( 
			INVALID_SOCKET == m_ClientMan[i]->GetSOCKET() && 
			m_ClientMan[i]->GetReferenceCount() )
		{
			// ������ �����µ� ���۷��� ī��Ʈ�� ���� �ִ� ���
			dwRemainCount++;
			if ( dwCurrentTime - m_ClientMan[i]->GetLastRequestIOTime() > 180000 )
			{
				// ������ io ��û �ð��� 15�ʰ� ���� ��� - Unknown reason connection close ���� ������ 3������ ����
				dwRemainCountWarn++;
			}
		}
	}

	if ( dwRemainCount > 0 || dwRemainCount > 0 )
	{
		sc::writeLogWarn( sc::string::format( "CleanDummyClientSlot remain client count %1% %2% ", dwRemainCount, dwRemainCountWarn ) );
	}

	// ó�� + zombie ó��
	for ( UINT i=1; i<m_nMaxClient; ++i )
	{
		if ( 
			INVALID_SOCKET == m_ClientMan[i]->GetSOCKET() && 
			m_ClientMan[i]->GetReferenceCount() && 
			dwCurrentTime - m_ClientMan[i]->GetLastRequestIOTime() > 180000 )	// - Unknown reason connection close ���� ������ 3������ ����
		{
			m_ClientMan[i]->ResetReference();
			CloseClient( static_cast< DWORD >( i ) );
			sc::writeLogWarn( sc::string::format( "CleanDummyClientSlot CloseClient client: %1%", i ) );
		}

		if ( INVALID_SOCKET == m_ClientMan[i]->GetSOCKET() &&
			 -1 == m_ClientMan[i]->UserDbNum() && 
			 net::SLOT_CLIENT == m_ClientMan[i]->GetSlotType() && 
			 net::ONLINE == m_ClientMan[i]->GetNetState() && 
			 0 == m_ClientMan[i]->GetReferenceCount() )
		{
			CloseClient( static_cast< DWORD >( i ) );
			sc::writeLogWarn( sc::string::format( "CleanDummyClientSlot zombie CloseClient client: %1%", i ) );
		}
	}
}