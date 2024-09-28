#include "pch.h"
#include "../FieldServer/GLGaeaServer.h"
#include "./NetUser.h"
#include "./NetInstanceClientMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

NetInstanceClientMan::NetInstanceClientMan(int nMaxClient, HANDLE hIOServer, int nReferenceCountMax)
	:NetClientManager(nMaxClient, hIOServer, nReferenceCountMax)
	,m_dwMatchSlot(net::SLOT_UNKNOWN)
{
	memset(m_szMatchIP, 0, sizeof(char) * (MAX_IP_LENGTH+1));
}

NetInstanceClientMan::~NetInstanceClientMan()
{
}

// 클라이언트가 처음 접속했을때 호출됨
// 클라이언트의 ip, port, 접속시간을 기록함.
int NetInstanceClientMan::SetAcceptedClient(DWORD dwClient, SOCKET sSocket, net::SLOT_TYPE slotType)
{
    std::tr1::shared_ptr<CNetUser> spData = m_ClientMan[dwClient];	
	if (!spData)
        return NET_ERROR;

	if (spData->SetAcceptedClient(sSocket) == NET_OK)
	{
		// 접속한 ip 와 Match 서버 ip 와 동일한지 검사한다.	
		if (spData->GetIP() == m_szMatchIP)
		{
			if (slotType == net::SLOT_MATCH)		//Instance가 접속한 경우
			{
				spData->SetSlotType(net::SLOT_MATCH);

				SetSlotMatch(dwClient);
			}
			else if (slotType == net::SLOT_UNKNOWN)	//Match가 접속하게 되면 SLOT_UNKNOWN일 것이다.
			{
				spData->SetSlotType(net::SLOT_MATCH);

				SetSlotMatch(dwClient);
			}
			else
			{
				spData->SetSlotType(slotType);
			}
		}
		else
		{
			spData->SetSlotType(net::SLOT_CLIENT);
		}
		return NET_OK;
	}	
	else
	{
		return NET_ERROR;
	}
}

void NetInstanceClientMan::SetSlotMatch(DWORD dwClient)
{
	if(dwClient == net::SLOT_UNKNOWN)
	{
		sc::writeLogError(std::string("NetInstanceClientMan::SetSlotMatch() - Wrong MatchSlot!"));
	}

	if(m_dwMatchSlot != net::SLOT_UNKNOWN)
	{
		sc::writeLogWarn(sc::string::format("MatchSlot is changed from %1% to %2%", m_dwMatchSlot, dwClient));
	}
	else
	{
		sc::writeLogInfo(sc::string::format("MatchSlot is set to %1%", dwClient));
	}

	m_dwMatchSlot = dwClient;
}

DWORD NetInstanceClientMan::GetSlotMatch()
{
	if(m_dwMatchSlot == net::SLOT_UNKNOWN)
	{
		sc::writeLogError(std::string("NetInstanceClientMan::GetSlotMatch() - Wrong MatchSlot!"));
	}

	return m_dwMatchSlot;
}

void NetInstanceClientMan::CloseClient( DWORD dwClient )
{
    //sc::CriticalSectionOwnerLock OwnerLock(m_CS_ClientMan);

    if (m_ClientMan[dwClient]->IsOnline())
    {
		if (m_ClientMan[dwClient]->CloseClient( dwClient ) == NET_OK)
        {                    
            ReleaseClientID(dwClient);
        }
        else
        {
            sc::writeLogInfo(sc::string::format("Slot %1% is already closed.", dwClient));
        }
    }
}

void NetInstanceClientMan::SetMatchIP(const char* szIP)
{
	if (szIP)
	    ::StringCchCopy(m_szMatchIP, MAX_IP_LENGTH+1, szIP);
}

int	NetInstanceClientMan::SendMatch(DWORD dwClient, NET_MSG_GENERIC* pBuffer)
{
	assert(pBuffer);

	if (!pBuffer)
        return NET_ERROR;

	if ( INVALID_SOCKET == GetSocket( dwClient )  )
	{
		sc::writeLogError( sc::string::format( "NetInstanceClientMan::SendMatch(DWORD dwClient, NET_MSG_GENERIC* pBuffer) failure. INVALID_SOCKET == GetSocket( dwClient ) client: %1%", dwClient ) );
		return NET_ERROR;
	}
	
	PER_IO_OPERATION_DATA* pIoWrite = NULL;
	
	DWORD dwSndBytes = 0;	
	dwSndBytes = pBuffer->Size();

	if (dwSndBytes > NET_DATA_BUFSIZE)
	{
		sc::writeLogError(
            sc::string::format(
                "SendMatch %1% dwSndBytes > NET_DATA_BUFSIZE", dwClient));
		return NET_ERROR;
	}
	else
	{
		pIoWrite = GetFreeOverIO(NET_SEND_POSTED);
		if (pIoWrite)
		{
			::CopyMemory(pIoWrite->Buffer, pBuffer, dwSndBytes);
			pIoWrite->dwTotalBytes	= dwSndBytes;        
			m_dwOutPacketCount++;
			m_dwOutPacketSize += dwSndBytes;
			return SendMatch(pIoWrite, dwSndBytes);
		}
        else
        {
            return NET_ERROR;
        }
	}
}

int	NetInstanceClientMan::SendMatch(PER_IO_OPERATION_DATA* PerIoData, DWORD dwSize)
{
	// MSG_OOB
	// MSG_DONTROUTE
	// MSG_PARTIAL
	INT		nRetCode = 0;
	DWORD	dwFlags = 0;
	DWORD	dwSndSize = dwSize;

	DWORD	dwSlot = GetSlotMatch();

	PerIoData->OperationType = NET_SEND_POSTED;
	PerIoData->DataBuf.len = (u_long) dwSize;
	
	SOCKET sSocket = GetSocket(dwSlot);
	if (sSocket == INVALID_SOCKET)
	{
		ReleaseOperationData(PerIoData);
		return NET_ERROR;
	}
	nRetCode = ::WSASend(sSocket,
					&(PerIoData->DataBuf),
					1,
					&dwSndSize,
					dwFlags,
					&(PerIoData->Overlapped),
					NULL);

	if (nRetCode == SOCKET_ERROR) 
	{
		nRetCode = ::WSAGetLastError();
		if (nRetCode != WSA_IO_PENDING) // WSA_IO_PENDING is not error.
		{
			::PostQueuedCompletionStatus(m_hIOServer,
										0,
										dwSlot,
										&PerIoData->Overlapped);

			sc::writeLogError(sc::string::format("SendMatch through slot %1% failure. ErrorCode: %2%", dwSlot, nRetCode));
			return NET_ERROR;		
		}
	}

	return NET_OK;
}
	
int	NetInstanceClientMan::SendClient(DWORD dwClient, NET_MSG_GENERIC* pBuffer)
{
	assert(pBuffer);
	GASSERT(m_ClientMan[dwClient]->IsClientSlot());

	if(!pBuffer)
        return NET_ERROR;	

    if(!m_ClientMan[dwClient]->IsClientSlot())	
	{
		return NET_ERROR;
	}

	return NetClientManager::SendClient(dwClient, pBuffer);
}
