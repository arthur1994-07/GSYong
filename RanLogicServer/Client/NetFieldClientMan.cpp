#include "../pch.h"
#include "../FieldServer/GLGaeaServer.h"
#include "./NetUser.h"
#include "./NetFieldClientMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

NetFieldClientMan::NetFieldClientMan( int nMaxClient, HANDLE hIOServer, int nReferenceCountMax, EMSERVICE_PROVIDER eServiceProvider )
	: NetClientManager( nMaxClient, hIOServer, nReferenceCountMax )
	, m_dwAgentSlot( net::INVALID_SLOT )
	, m_ServiceProvider( eServiceProvider )
{
	memset(m_szAgentIP, 0, sizeof(char) * (MAX_IP_LENGTH+1));
    memset(m_szCacheIP, 0, sizeof(char) * (MAX_IP_LENGTH+1));
}

NetFieldClientMan::~NetFieldClientMan()
{
}

// 클라이언트가 처음 접속했을때 호출됨
// 클라이언트의 ip, port, 접속시간을 기록함.
int NetFieldClientMan::SetAcceptedClient( DWORD dwClient, SOCKET sSocket, net::SLOT_TYPE slotType )
{
    std::tr1::shared_ptr< CNetUser > spData = m_ClientMan[dwClient];
	if ( !spData )
        return NET_ERROR;

	if ( spData->SetAcceptedClient( sSocket ) == NET_OK )
	{
		// 접속한 ip가 서버인지 확인한다.
		// 필드로 접속하는 서버는 에이전트뿐이다.
		if ( spData->GetIP() == m_szAgentIP /*|| spData->GetIP() == m_szCacheIP*/ )
		{
			// Field로 접속한 슬롯이다(AgentServer). 그 외의 경우는 FieldServer가 직접 접속하는 경우이므로, SlotType이 명확하다.
			if ( slotType == net::SLOT_UNKNOWN )
			{
                sc::writeLogInfo( std::string( "NetFieldClientMan::SetAcceptedClient() - Agent server connected." ) );

				// 현재 Agent 와의 연결이 없는 경우
				if ( 
					net::INVALID_SLOT == m_dwAgentSlot || 
					INVALID_SOCKET == GetSocket( m_dwAgentSlot ) )
				{
					spData->SetSlotType( net::SLOT_AGENT );
					SetSlotAgent( dwClient );
				}
				else
				{
					// 테스트 개인서버 사용할 수 있도록
					if ( SP_OFFICE_TEST == m_ServiceProvider )
					{
						spData->SetSlotType( net::SLOT_CLIENT );
					}
					else
					{
						// 지금 Agent 와의 연결이 있지만 Agent 에서 접속을 했다.
						// 이전 연결을 종료 시킨다.
						CloseClient( m_dwAgentSlot );

						spData->SetSlotType( net::SLOT_AGENT );
						SetSlotAgent( dwClient );
					}
				}
			}
			else
			{
				spData->SetSlotType( slotType );
			}
		}
		else
		{
			spData->SetSlotType( net::SLOT_CLIENT );
		}

		return NET_OK;
	}	
	else
	{
		return NET_ERROR;
	}
}

void NetFieldClientMan::SetSlotAgent(DWORD dwClient)
{
    if(dwClient == net::INVALID_SLOT)
    {
        sc::writeLogError(std::string("NetFieldClientMan::SetSlotAgent() - Wrong AgentSlot!"));
    }

    if(m_dwAgentSlot != net::INVALID_SLOT)
    {
        sc::writeLogWarn(sc::string::format("AgentSlot is changed from %1% to %2%", m_dwAgentSlot, dwClient));
    }
    else
    {
        sc::writeLogInfo(sc::string::format("AgentSlot is set to %1%", dwClient));
    }

    m_dwAgentSlot = dwClient;
}


DWORD NetFieldClientMan::GetSlotAgent()
{
	if(m_dwAgentSlot == net::INVALID_SLOT)
	{
		sc::writeLogError(std::string("NetFieldClientMan::GetSlotAgent() - Wrong AgentSlot!"));
	}

	return m_dwAgentSlot;
}

void NetFieldClientMan::CloseClient( DWORD dwClient )
{
	NetClientManager::CloseClient( dwClient );
}

void NetFieldClientMan::SetAgentIP(const char* szIP)
{
	if (szIP)
	    ::StringCchCopy(m_szAgentIP, MAX_IP_LENGTH+1, szIP);
}

void NetFieldClientMan::SetCacheIP( const char* szIP )
{
    if (szIP)
        ::StringCchCopy(m_szCacheIP, MAX_IP_LENGTH+1, szIP);
}

int	NetFieldClientMan::SendAgent(DWORD dwClient, NET_MSG_GENERIC* pBuffer)
{
	assert( pBuffer );

	if ( !pBuffer )
        return NET_ERROR;
	
	if ( INVALID_SOCKET == GetSocket( dwClient )  )
	{
		return NET_ERROR;
	}

	PER_IO_OPERATION_DATA* pIoWrite = NULL;
	
	DWORD dwSndBytes = 0;	
	dwSndBytes = pBuffer->Size();

	if ( dwSndBytes > NET_DATA_BUFSIZE )
	{
		sc::writeLogError(
            sc::string::format(
                "SendAgent %1% dwSndBytes > NET_DATA_BUFSIZE", dwClient ) );
		return NET_ERROR;
	}
	else
	{
		pIoWrite = GetFreeOverIO( NET_SEND_POSTED );
		if (pIoWrite)
		{
			::CopyMemory( pIoWrite->Buffer, pBuffer, dwSndBytes );
			pIoWrite->dwTotalBytes = dwSndBytes;        
			m_dwOutPacketCount++;
			m_dwOutPacketSize += dwSndBytes;
			return SendAgent( pIoWrite, dwSndBytes );
		}
        else
        {
            return NET_ERROR;
        }
	}
}

int	NetFieldClientMan::SendAgent( PER_IO_OPERATION_DATA* PerIoData, DWORD dwSize )
{
	// MSG_OOB
	// MSG_DONTROUTE
	// MSG_PARTIAL
	INT		nRetCode = 0;
	DWORD	dwFlags = 0;
	DWORD	dwSndSize = dwSize;

	DWORD	dwSlot = GetSlotAgent();

	PerIoData->OperationType = NET_SEND_POSTED;
	PerIoData->DataBuf.len = ( u_long ) dwSize;
	
	SOCKET sSocket = GetSocket( dwSlot );
	if ( sSocket == INVALID_SOCKET )
	{
		ReleaseOperationData( PerIoData );
		return NET_ERROR;
	}
	
	AddReference( dwSlot );

	nRetCode = ::WSASend(sSocket,
					&( PerIoData->DataBuf ),
					1,
					&dwSndSize,
					dwFlags,
					&( PerIoData->Overlapped ),
					NULL);

	if ( nRetCode == SOCKET_ERROR ) 
	{
		nRetCode = ::WSAGetLastError();
		if ( nRetCode != WSA_IO_PENDING ) // WSA_IO_PENDING is not error.
		{
			::PostQueuedCompletionStatus( m_hIOServer, 0, dwSlot, &PerIoData->Overlapped );
			sc::writeLogError( sc::string::format( "SendAgent through slot %1% failure. ErrorCode: %2%", dwSlot, nRetCode ) );
			return NET_ERROR;		
		}
	}

	return NET_OK;
}
	
int	NetFieldClientMan::SendClient(DWORD dwClient, NET_MSG_GENERIC* pBuffer)
{
	assert(pBuffer);
	//GASSERT(m_ClientMan[dwClient]->IsClientSlot());

	if(!pBuffer)
        return NET_ERROR;	

    if(!m_ClientMan[dwClient]->IsClientSlot())	
	{
		return NET_ERROR;
	}

	return NetClientManager::SendClient(dwClient, pBuffer);
}

void NetFieldClientMan::SetCountry(DWORD ClientSlot, wb::EM_COUNTRY Country)
{
    std::tr1::shared_ptr<CNetUser> spData = m_ClientMan[ClientSlot];	
    if (spData)
        spData->SetCountry(Country);
}

wb::EM_COUNTRY NetFieldClientMan::GetCountry(DWORD ClientSlot)
{
    std::tr1::shared_ptr<CNetUser> spData = m_ClientMan[ClientSlot];	
    if (spData)
        return spData->GetCountry();
    else
        return wb::UNKNOWN_COUNTRY;
}
