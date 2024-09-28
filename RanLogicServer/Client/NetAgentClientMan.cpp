#include "../pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../RanLogic/Util/s_CCsvFile.h"
#include "../Server/NetServerHelper.h"

#include "./NetUser.h"
#include "./NetAgentClientMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

NetAgentClientMan::NetAgentClientMan(int nMaxClient, HANDLE hIOServer, int nReferenceCountMax, F_SERVER_INFO (* pFieldServerInfo)[FIELDSERVER_MAX], A_SERVER_INFO* pAgentServerInfo)
    : NetClientManager(nMaxClient, hIOServer, nReferenceCountMax)
	, m_pFieldServerInfo(pFieldServerInfo)	//mjeon.AF
	, m_pAgentServerInfo(pAgentServerInfo)
{
	InitializeCriticalSectionAndSpinCount(&m_CS_FieldInfo, sc::CRITICAL_SECTION_SPIN_COUNT);
}

NetAgentClientMan::~NetAgentClientMan()
{
	DeleteCriticalSection(&m_CS_FieldInfo);
}

void NetAgentClientMan::Reset(DWORD dwClient)
{
    //sc::CriticalSectionOwnerLock OwnerLock(m_CS_ClientMan);
    m_ClientMan[dwClient]->Reset();
}

int NetAgentClientMan::SetAcceptedClient(DWORD dwClient, SOCKET sSocket, net::SLOT_TYPE slotType)
{
	if ( NET_OK == NetClientManager::SetAcceptedClient(
		dwClient, sSocket, slotType ) )
	{
		std::tr1::shared_ptr< CNetUser > spData = m_ClientMan[ dwClient ];
		if ( !spData )
			return NET_ERROR;

		for ( int i=0; i<AGENTSERVER_MAX; ++i )
		{
			// 저장된 Agent Server 정보에서 접속한 경우;
			if ( spData->GetIP() != m_pAgentServerInfo[ i ].szPublicIp )
				continue;

			// Agent에 연결되는 Slot Type은 Client로만 취급한다;
			if ( slotType != net::SLOT_CLIENT )
				continue;

			sc::writeLogInfo( std::string(
				"NetAgentClientMan::SetAcceptedClient() - Agent server connected." ) );

			// 현재 Agent 와의 연결이 없는 경우;
			if ( net::INVALID_SLOT == m_pAgentServerInfo[ i ].NetSlotNum || 
				INVALID_SOCKET == GetSocket( m_pAgentServerInfo[ i ].NetSlotNum ) )
			{
				spData->SetSlotType( net::SLOT_AGENT_AGENT );
				m_pAgentServerInfo[ i ].NetSlotNum = dwClient;
				
				return NET_OK;
			}
			else
			{
				// 지금 Agent 와의 연결이 있지만 Agent 에서 접속을 했다;
				// 이전 연결을 종료 시킨다;
				CloseClient( m_pAgentServerInfo[ i ].NetSlotNum );

				spData->SetSlotType( net::SLOT_AGENT_AGENT );

				m_pAgentServerInfo[ i ].NetSlotNum = dwClient;

				return NET_OK;
			}
		}

		return NET_OK;
	}
	else
	{
		return NET_ERROR;
	}
}

int	NetAgentClientMan::SetUpFieldInfo(DWORD dwClient, int nFieldServer, DWORD dwGaeaID, int nServerChannel)
{
	if(nServerChannel < 0 || nServerChannel > MAX_CHANNEL_NUMBER)
	{
		sc::writeLogError(sc::string::format("NetAgentClientMan::SetUpFieldInfo() - Wrong Channel(%1%)", nServerChannel));
		return NET_ERROR;
	}

	if(nFieldServer < 0 || nFieldServer > FIELDSERVER_MAX)
	{
		sc::writeLogError(sc::string::format("NetAgentClientMan::SetUpFieldInfo() - Wrong FieldServer(%1%)", nFieldServer));
		return NET_ERROR;
	}

	sc::CriticalSectionOwnerLock OwnerLock(m_CS_FieldInfo);

	m_mapFieldServer.erase(dwClient);
	m_mapFieldServer[dwClient] = FIELDSERVER_INFO(nServerChannel, nFieldServer);

	return NET_OK;
}

int	NetAgentClientMan::SetUpInstanceInfo(DWORD dwClient, int nInstanceServer, int nServerChannel /*Instance로 넘어가기 전에 캐릭터가 속했던 채널*/)
{
	if(nInstanceServer < 0 || nInstanceServer > MAXNUM_OF_INSTANCESERVER_TO_MATCH)
	{
		sc::writeLogError(sc::string::format("NetAgentClientMan::SetUpInstanceInfo() - Wrong InstanceServer(%1%)", nInstanceServer));
		return NET_ERROR;
	}

	sc::CriticalSectionOwnerLock OwnerLock(m_CS_FieldInfo);

	m_mapFieldServer.erase(dwClient);
	m_mapFieldServer[dwClient] = FIELDSERVER_INFO(nServerChannel, nInstanceServer, TRUE);

	return NET_OK;
}

BOOL NetAgentClientMan::IsBoundByInstance(DWORD dwClient, DWORD &dwInstanceServerNum)
{
	dwInstanceServerNum = MAXNUM_OF_INSTANCESERVER_TO_MATCH;

	MAP_FIELDSERVER_INFO_ITER iter = m_mapFieldServer.find(dwClient);
	if (iter != m_mapFieldServer.end())
	{
		FIELDSERVER_INFO &info = iter->second;
		if (info.bInstance)
		{
			dwInstanceServerNum = info.nSvrNum;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

void NetAgentClientMan::ResetFieldInfo(DWORD dwClient)
{
    if (dwClient >= (DWORD) m_nMaxClient)
    {
		return;
    }

	sc::CriticalSectionOwnerLock OwnerLock(m_CS_FieldInfo);

	m_mapFieldServer.erase(dwClient);
}


DWORD NetAgentClientMan::AdjestSlotNum( DWORD dwClient )
{
	if( IsAgentFieldSlot( dwClient ) )
		return dwClient;

	if( IsClientSlot( dwClient ) )
	{
		MAP_FIELDSERVER_INFO::iterator	iter = m_mapFieldServer.find( dwClient );
		if( iter == m_mapFieldServer.end() )
		{
			sc::writeLogError(
				sc::string::format( 
					"NetAgentClientMan::GetFieldSocket() - The CliendID(%1%) is not mapped to FieldServerInfo yet.", 
					dwClient ) );
			return net::INVALID_SLOT;
		} //if

		const FIELDSERVER_INFO&		fi = iter->second;
		return m_pFieldServerInfo[fi.nChannel][fi.nSvrNum].NetSlotNum;
	} //if

	sc::writeLogError( sc::string::format( "NetAgentClientMan::SendField() - Wrong Slot(%1%)", dwClient ) );
	return net::INVALID_SLOT;
} //NetAgentClientMan::AdjestSlotNum

DWORD NetAgentClientMan::GetServerGroup ( DWORD dwClient )
{
	for ( int i=0; i<AGENTSERVER_MAX; ++i )
	{
		if ( dwClient == m_pAgentServerInfo[ i ].NetSlotNum )
			return i;
	}

	return AGENTSERVER_MAX;
}

DWORD NetAgentClientMan::GetClientID_ServerGroup ( DWORD dwServerGroup )
{
	if ( dwServerGroup < 0 || AGENTSERVER_MAX <= dwServerGroup )
		return net::INVALID_SLOT;

	return m_pAgentServerInfo[ dwServerGroup ].NetSlotNum;
}

SOCKET NetAgentClientMan::GetFieldSocket(DWORD dwClient)
{
	GASSERT(m_pFieldServerInfo);
	//GASSERT(IsClientSlot(dwClient));

	if(!IsClientSlot(dwClient))
		return INVALID_SOCKET;

	if (m_mapFieldServer.count(dwClient) <= 0)
	{
		sc::writeLogError(sc::string::format("NetAgentClientMan::GetFieldSocket() - The CliendID(%1%) is not mapped to FieldServerInfo yet.", dwClient));

		return INVALID_SOCKET;
	}
	
	FIELDSERVER_INFO fi = m_mapFieldServer[dwClient];

	DWORD dwSlotField = m_pFieldServerInfo[fi.nChannel][fi.nSvrNum].NetSlotNum;

	if (dwSlotField == net::INVALID_SLOT)
		return INVALID_SOCKET;
	
	return m_ClientMan[dwSlotField]->GetSOCKET();
}

DWORD NetAgentClientMan::GetFieldClientSlot( DWORD dwClient )
{
	GASSERT( m_pFieldServerInfo );

	if( !IsClientSlot( dwClient ) )
		return net::INVALID_SLOT;

	if ( m_mapFieldServer.count( dwClient ) <= 0 )
	{
		sc::writeLogError( 
			sc::string::format( 
			"NetAgentClientMan::GetFieldClientSlot() - The CliendID(%1%) is not mapped to FieldServerInfo yet.", dwClient ) );
		return net::INVALID_SLOT;
	}

	FIELDSERVER_INFO fi = m_mapFieldServer[dwClient];

	return m_pFieldServerInfo[fi.nChannel][fi.nSvrNum].NetSlotNum;
}

void NetAgentClientMan::SendClientFinal()
{
	NetClientManager::SendClientFinal();

	DWORD dwSendSize = 0;
	
	for (DWORD dwClient=0; dwClient < (DWORD) m_nMaxClient; ++dwClient)
	{	
		if (dwSendSize > 0)
		{
			// 압축된 데이터 전송
			m_dwOutCompressCount++;
			m_dwOutCompressSize += dwSendSize;			
		}
	}
}

int	NetAgentClientMan::SendAgent ( DWORD dwAgentNum, NET_MSG_GENERIC* pBuffer )
{
	if ( NULL == pBuffer )
		return NET_ERROR;

	GASSERT( dwAgentNum >= 0 && dwAgentNum < AGENTSERVER_MAX );

	if ( dwAgentNum >= 0 && dwAgentNum < AGENTSERVER_MAX )
	{
		sc::writeLogError(sc::string::format(
			"NetAgentClientMan::SendAgent failure. (AgentSvrNum:%1%)",
			dwAgentNum ) );

		return NET_ERROR;
	}

	DWORD dwClient = m_pAgentServerInfo[ dwAgentNum ].NetSlotNum;

	return SendClient( dwClient, pBuffer );
}

int	NetAgentClientMan::SendField(DWORD dwChannel, DWORD dwFieldSvrNum, NET_MSG_GENERIC* pBuffer)
{
	if (pBuffer == NULL)
		return NET_ERROR;

	GASSERT((dwChannel >= 0 && dwChannel < MAX_CHANNEL_NUMBER) && (dwFieldSvrNum >= 0 && dwFieldSvrNum < FIELDSERVER_MAX));
	if (!(dwChannel >= 0 && dwChannel < MAX_CHANNEL_NUMBER) && (dwFieldSvrNum >= 0 && dwFieldSvrNum < FIELDSERVER_MAX))
	{
		sc::writeLogError(sc::string::format("NetAgentClientMan::SendField failure. (Channel:%1%, FieldSvrNum:%2%)", dwChannel, dwFieldSvrNum));
		return NET_ERROR;
	}

	DWORD dwClient = m_pFieldServerInfo[dwChannel][dwFieldSvrNum].NetSlotNum;

	if ( INVALID_SOCKET == GetSocket( dwClient )/*!IsOnline(dwClient)*/ )
	{
		sc::writeLogError(sc::string::format("NetAgentClientMan::SendField failure. FieldServer(%1%, %2%) is offline", dwChannel, dwFieldSvrNum));
		return NET_ERROR;
	}

	PER_IO_OPERATION_DATA* pIoWrite = NULL;
	
	DWORD dwSndBytes = 0;

	dwSndBytes = pBuffer->Size();
	m_dwOutPacketCount++;
	m_dwOutPacketSize += dwSndBytes;

	if (dwSndBytes > NET_DATA_BUFSIZE)
	{
		sc::writeLogError(sc::string::format("SendField %d dwSndBytes > NET_DATA_BUFSIZE", dwClient ));
		return NET_ERROR;
	}
	else
	{
		pIoWrite = (PER_IO_OPERATION_DATA*) GetFreeOverIO(NET_SEND_POSTED);

		if (pIoWrite == NULL)
		{
			return NET_ERROR;
		}
		else
		{
			::CopyMemory(pIoWrite->Buffer, pBuffer, dwSndBytes);
			pIoWrite->dwTotalBytes = dwSndBytes;
			
			return SendField(dwClient, pIoWrite, dwSndBytes);
		}
	}
}

int	NetAgentClientMan::SendField(DWORD dwClient, NET_MSG_GENERIC* pBuffer)
{
	if (pBuffer == NULL)
		return NET_ERROR;

	if ( INVALID_SOCKET == GetSocket( dwClient )/*!IsOnline(dwClient)*/ )
	{
		return NET_ERROR;
	}

	PER_IO_OPERATION_DATA* pIoWrite = NULL;
	
	DWORD dwSndBytes = 0;

	dwSndBytes = pBuffer->Size();
	m_dwOutPacketCount++;
	m_dwOutPacketSize += dwSndBytes;

	if (dwSndBytes > NET_DATA_BUFSIZE)
	{
		sc::writeLogError(sc::string::format("SendField %d dwSndBytes > NET_DATA_BUFSIZE", dwClient ));
		return NET_ERROR;
	}
	else
	{
		pIoWrite = (PER_IO_OPERATION_DATA*) GetFreeOverIO(NET_SEND_POSTED);

		if (pIoWrite == NULL)
		{
			return NET_ERROR;
		}
		else
		{
			::CopyMemory(pIoWrite->Buffer, pBuffer, dwSndBytes);
			pIoWrite->dwTotalBytes = dwSndBytes;
			
			return SendField(dwClient, pIoWrite, dwSndBytes);
		}
	}
}

static int nSendFieldError = 0;

int NetAgentClientMan::SendField( DWORD dwClient, PER_IO_OPERATION_DATA* PerIoData, DWORD dwSize )
{
	// MSG_OOB
	// MSG_DONTROUTE
	// MSG_PARTIAL
	INT		nRetCode = 0;
	DWORD	dwFlags = 0;
	DWORD	dwSndSize = dwSize;

	PerIoData->OperationType = NET_SEND_POSTED;
	PerIoData->DataBuf.len = static_cast<u_long>(dwSize);

	//mjeon.AF
	if ( dwClient >= ( DWORD )m_nMaxClient )
	{
		ReleaseOperationData( PerIoData );
		return NET_ERROR;
	}

	SOCKET		sSocket					= INVALID_SOCKET;
	DWORD		dwRequestedClientSlot	= AdjestSlotNum( dwClient );
	if( dwRequestedClientSlot == net::INVALID_SLOT )
	{
		ReleaseOperationData( PerIoData );
		return NET_ERROR;
	}

	sSocket = m_ClientMan[dwRequestedClientSlot]->GetSOCKET();
	if( sSocket == INVALID_SOCKET )
	{
		ReleaseOperationData( PerIoData );
		return NET_ERROR;
	} //if

	AddReference( dwRequestedClientSlot );

	nRetCode = ::WSASend(
					sSocket,
					&( PerIoData->DataBuf ),
					1,
					&dwSndSize,
					dwFlags,
					&( PerIoData->Overlapped ),
					NULL );

	if ( nRetCode == SOCKET_ERROR ) 
	{
		nRetCode = ::WSAGetLastError();
		if ( nRetCode != WSA_IO_PENDING ) // WSA_IO_PENDING is not error.
		{
			// 이 에러메시지 삭제해야
			if ( nSendFieldError < 100 )
			{
				NET_MSG_GENERIC* pMsg = ( NET_MSG_GENERIC* ) PerIoData->Buffer;
				if ( pMsg && pMsg->nType > 0 )
				{
					sc::writeLogError( 
						sc::string::format( 
						"SendField Client slot %1% Error %2% Type %3%", 
						dwRequestedClientSlot, 
						nRetCode, 
						pMsg->nType ) );
					nSendFieldError++;
				}
			}

			::PostQueuedCompletionStatus( m_hIOServer, 0, dwRequestedClientSlot, &PerIoData->Overlapped );

			return NET_ERROR;
		}
	}

	return NET_OK;
}

int NetAgentClientMan::addRcvMsg(DWORD dwClient, void* pMsg, DWORD dwSize)
{
	ASSERT(pMsg);	
	if (dwSize > NET_DATA_BUFSIZE)
		return NET_ERROR;

	// 수신받은 크기 및 카운터 기록
	m_dwInPacketCount++;
	m_dwInPacketSize += dwSize;

	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return 0;
	}
	else
		return m_ClientMan[dwClient]->addRcvMsg(pMsg, dwSize);
}

void NetAgentClientMan::ResetRcvBuffer(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return;
	}
	else
        m_ClientMan[dwClient]->ResetRcvBuffer();
}

void* NetAgentClientMan::getRcvMsg(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return 0;
	}
	else
        return m_ClientMan[dwClient]->getRcvMsg(); // 클라이언트로 부터 온 메시지
}

SOCKET NetAgentClientMan::GetSocket(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return INVALID_SOCKET;
	}
	else
        return m_ClientMan[dwClient]->GetSOCKET();
}

void NetAgentClientMan::SaveFreeSlot()
{
#ifdef _USE_TBB
	//mjeon.ToDo.DoNothing
#else
    //sc::CriticalSectionOwnerLock OwnerLock(m_CS_ClientMan);
    SERVER_UTIL::CCsvFile SaveFile(_T("dequeSleepCID"));	
	for (int nSize=0; nSize < (int) m_deqSleepCID.size(); ++nSize)
		SaveFile.Write(_T("%d"), m_deqSleepCID[nSize]);
#endif
}

void NetAgentClientMan::SetHeartBeat(DWORD dwClient)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return;
	}
    else
        m_ClientMan[dwClient]->SetHeartBeat();
}

void NetAgentClientMan::ResetHeartBeat(DWORD dwClient)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return;
	}
	else
		m_ClientMan[dwClient]->ResetHeartBeat();
}

bool NetAgentClientMan::CheckHeartBeat(DWORD dwClient)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return 0;
	}
	else
        return m_ClientMan[dwClient]->CheckHeartBeat();
}

bool NetAgentClientMan::IsOnline(DWORD dwClient)
{
    if (dwClient == net::INVALID_SLOT)
        return false;

	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return 0;
	}
	else
        return m_ClientMan[dwClient]->IsOnline();
}

net::SLOT_TYPE NetAgentClientMan::GetSlotType(DWORD dwClient)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return net::SLOT_UNKNOWN;
	}
	else
        return m_ClientMan[dwClient]->GetSlotType();
}

bool NetAgentClientMan::IsAgentFieldSlot(DWORD dwClient)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
    if (dwClient >= (DWORD) m_nMaxClient)
	{
        return false;
	}
    else
        return m_ClientMan[dwClient]->IsAgentFieldSlot();
}

bool NetAgentClientMan::IsMatchSlot(DWORD dwClient)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
    if (dwClient >= (DWORD) m_nMaxClient)
	{
        return false;
	}
    else
        return m_ClientMan[dwClient]->IsMatchSlot();
}

bool NetAgentClientMan::IsAgentAgentSlot ( DWORD dwClient )
{
	//GASSERT( dwClient < (DWORD)m_nMaxClient );
	
	if ( dwClient >= (DWORD)m_nMaxClient )
	{
		return false;
	}
	else
		return m_ClientMan[ dwClient ]->IsAgentAgentSlot();

}

std::string NetAgentClientMan::GetClientIP(DWORD dwClient) const
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient > (DWORD)m_nMaxClient)
	{
		return std::string("0.0.0.0");
	}
	else
		return m_ClientMan[dwClient]->GetIP();
}

USHORT NetAgentClientMan::GetClientPort(DWORD dwClient)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient > (DWORD)m_nMaxClient)
	{
		return 0;
	}
	else
        return m_ClientMan[dwClient]->GetPort();
}

//! 나이를 세팅한다.
void NetAgentClientMan::SetAge(DWORD dwClient, WORD Age)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
    if (dwClient >= (DWORD) m_nMaxClient)
	{
        return;
	}
	else
		m_ClientMan[dwClient]->SetAge(Age);
}

WORD NetAgentClientMan::GetAge(DWORD dwClient) const
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
    if (dwClient >= (DWORD) m_nMaxClient)
	{
		return 0;
	}
	else
		return m_ClientMan[dwClient]->GetAge();
}

wb::EM_COUNTRY NetAgentClientMan::GetCountry(DWORD ClientSlot) const
{
    if (ClientSlot >= (DWORD) m_nMaxClient)
        return wb::UNKNOWN_COUNTRY;
    else
        return m_ClientMan[ClientSlot]->GetCountry();
}

void NetAgentClientMan::SetCountry(DWORD ClientSlot, wb::EM_COUNTRY Country)
{
    if (ClientSlot < (DWORD) m_nMaxClient)
        m_ClientMan[ClientSlot]->SetCountry(Country);
}

void NetAgentClientMan::KorSetUUID( DWORD dwClient, const TCHAR* szUUID )
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return;
	}
	else
        m_ClientMan[dwClient]->KorSetUUID( szUUID );
}

TCHAR* NetAgentClientMan::KorGetUUID( DWORD dwClient )
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return 0;
	}
	else
        return m_ClientMan[dwClient]->KorGetUUID();
}

void NetAgentClientMan::JPSetUUID( DWORD dwClient, const TCHAR* szUUID )
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return;
	}
	else
        m_ClientMan[dwClient]->JPSetUUID( szUUID );
}

TCHAR* NetAgentClientMan::JPGetUUID( DWORD dwClient )
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return 0;
	}
	else
        return m_ClientMan[dwClient]->JPGetUUID();
}

void NetAgentClientMan::GspSetUUID( DWORD dwClient, const TCHAR* szUUID )
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return;
	}
	else
        m_ClientMan[dwClient]->GspSetUUID( szUUID );
}

TCHAR* NetAgentClientMan::GspGetUUID( DWORD dwClient )
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return 0;
	}
	else
        return m_ClientMan[dwClient]->GspGetUUID();
}

void NetAgentClientMan::SetEncKey( DWORD dwClient, const TCHAR* szEncKey )
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
        return;
	}
	else
        m_ClientMan[dwClient]->SetEncKey( szEncKey );
}

TCHAR*  NetAgentClientMan::GetEncKey( DWORD dwClient )
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return 0;
	}
	else
        return m_ClientMan[dwClient]->GetEncKey();
}

void NetAgentClientMan::ResetValidChaNumList(DWORD dwClient)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	m_ClientMan[dwClient]->ResetValidChaNum();
}

void NetAgentClientMan::ValidateChaNum(DWORD dwClient, DWORD dwChaNum)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	m_ClientMan[dwClient]->ValidateChaNum(dwChaNum);
}

void NetAgentClientMan::InvalidateChaNum(DWORD dwClient, DWORD dwChaNum)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	m_ClientMan[dwClient]->InvalidateChaNum(dwChaNum);
}

BOOL NetAgentClientMan::IsValidChaNum(DWORD dwClient, DWORD dwChaNum)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	return m_ClientMan[dwClient]->IsValidChaNum(dwChaNum);
}

void NetAgentClientMan::TerraSetDecodedTID( DWORD dwClient, const TCHAR* szDecodedTID )
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return;
	}
	else
        m_ClientMan[dwClient]->TerraSetDecodedTID(szDecodedTID);
}

TCHAR* NetAgentClientMan::TerraGetDecodedTID( DWORD dwClient )
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return 0;
	}
	else
        return m_ClientMan[dwClient]->TerraGetDecodedTID();
}

// 계정 인증중인지 검사
bool NetAgentClientMan::IsAccountPassing(DWORD dwClient)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return false;
	}
	else 
        return m_ClientMan[dwClient]->IsAccountPassing();
}

// 계정 인증중 세팅
void NetAgentClientMan::SetAccountPassing(DWORD dwClient, bool bPassing)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return;
	}
	else 
        m_ClientMan[dwClient]->SetAccountPassing(bPassing);
}

//! 말레이지아 요청으로 1 회 접속시 5 회 이상 로그인 요청시 
//! 사용자의 접속을 끊음...
//! 로그인 요청 횟수 증가시킴
void NetAgentClientMan::IncLoginAttempt(DWORD dwClient)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return;
	}
	else   
        m_ClientMan[dwClient]->IncLoginAttempt();
}

//! 로그인 요청 횟수 가져옴
WORD NetAgentClientMan::GetLoginAttempt(DWORD dwClient)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return 0;
	}
	else 
        return m_ClientMan[dwClient]->GetLoginAttempt();
}

/**
* Thailand 의 사용자 플래그를 세팅한다.
* \param dwClient
* \param wFlag 
*/
void NetAgentClientMan::ThaiSetFlag (DWORD dwClient, WORD wFlag)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return;
	}
	else 
        m_ClientMan[dwClient]->ThaiSetFlag(wFlag);
}

WORD NetAgentClientMan::ThaiGetFlag(DWORD dwClient)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return 0;
	}
	else  
        return m_ClientMan[dwClient]->ThaiGetFlag();
}

void NetAgentClientMan::ThaiSetTodayPlayTime (DWORD dwClient, WORD wMinutes)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return;
	}
	else   
        return m_ClientMan[dwClient]->ThaiSetTodayPlayTime (wMinutes);
}

bool NetAgentClientMan::ThaiIs3HourOver (DWORD dwClient, CTime& currentTime)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return false;
	}
	else
		return m_ClientMan[dwClient]->ThaiIs3HourOver(currentTime);
}

// 2차비번
bool NetAgentClientMan::IsBlockBy2ndPassword(DWORD dwUserDBNum)
{
	MAP_2NDPASSWORD_BLOCK_INFO_ITER iter = m_map2ndPasswordBlock.find(dwUserDBNum);

	if( iter != m_map2ndPasswordBlock.end() )
	{
		if( (iter->second).IsBlock() )
		{
			return true;
		}
	} 

	return false;
}
CTime NetAgentClientMan::GetBlockExpiredTime(DWORD dwUserDBNum)
{
	CTime strExpiredTime;

	MAP_2NDPASSWORD_BLOCK_INFO_ITER iter = m_map2ndPasswordBlock.find(dwUserDBNum);

	if( iter != m_map2ndPasswordBlock.end() )
	{
		strExpiredTime = (iter->second).GetBlockTime();
	} 

	return strExpiredTime;
}

void NetAgentClientMan::ResetCertification2ndPassword(DWORD dwUserDBNum)
{
	MAP_2NDPASSWORD_BLOCK_INFO_ITER iter = m_map2ndPasswordBlock.find(dwUserDBNum);

	if( iter != m_map2ndPasswordBlock.end() )
	{
		(iter->second).Reset();
	} 
}

void NetAgentClientMan::Increase2ndPasswordError(DWORD dwUserDBNum)
{
	MAP_2NDPASSWORD_BLOCK_INFO_ITER iter = m_map2ndPasswordBlock.find(dwUserDBNum);

	if( iter != m_map2ndPasswordBlock.end() )
	{
		(iter->second).IncreaseCount();
	} 
	else
	{
		m_map2ndPasswordBlock[dwUserDBNum] = PASSWORD_BLOCK();
	}
}
///////////////////////////////////////////