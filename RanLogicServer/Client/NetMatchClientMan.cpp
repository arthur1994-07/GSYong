#include "pch.h"
#include "./NetUser.h"
#include "./NetMatchClientMan.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Log/LogMan.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


NetMatchClientMan::NetMatchClientMan(int nMaxClient, HANDLE hIOServer, int nReferenceCountMax, A_SERVER_INFO *pAgentServerInfo)
	:NetClientManager(nMaxClient, hIOServer, nReferenceCountMax)
	,m_pAgentServerInfo(pAgentServerInfo)
{
}

NetMatchClientMan::~NetMatchClientMan()
{
}

int	NetMatchClientMan::SendClient(DWORD dwClient, NET_MSG_GENERIC* pBuffer)
{
	assert(pBuffer);
	GASSERT(m_ClientMan[dwClient]->IsClientSlot() == FALSE);	//MatchServer는 Client와 통신하지 않는다.

	if(!pBuffer)
        return NET_ERROR;

    if(m_ClientMan[dwClient]->IsClientSlot())	
	{
		return NET_ERROR;
	}

	return NetClientManager::SendClient(dwClient, pBuffer);
}

/*
int	NetMatchClientMan::SendInstance(DWORD dwInstanceSvrNum, NET_MSG_GENERIC* pBuffer)
{
	if (pBuffer == NULL)
		return NET_ERROR;

	GASSERT((dwChannel >= 0 && dwChannel < MAX_CHANNEL_NUMBER) && (dwFieldSvrNum >= 0 && dwFieldSvrNum < FIELDSERVER_MAX));
	if (!(dwChannel >= 0 && dwChannel < MAX_CHANNEL_NUMBER) && (dwFieldSvrNum >= 0 && dwFieldSvrNum < FIELDSERVER_MAX))
	{
		sc::writeLogError(sc::string::format("NetMatchClientMan::SendInstance failure. (Channel:%1%, FieldSvrNum:%2%)", dwChannel, dwFieldSvrNum));
		return NET_ERROR;
	}

	DWORD dwClient = m_pFieldServerInfo[dwChannel][dwFieldSvrNum].NetSlotNum;

	if (!IsOnline(dwClient))
	{
		sc::writeLogError(sc::string::format("NetMatchClientMan::SendInstance failure. FieldServer(%1%, %2%) is offline", dwChannel, dwFieldSvrNum));
		return NET_ERROR;
	}

	PER_IO_OPERATION_DATA* pIoWrite = NULL;
	
	DWORD dwSndBytes = 0;

	dwSndBytes = pBuffer->Size();
	m_dwOutPacketCount++;
	m_dwOutPacketSize += dwSndBytes;

	if (dwSndBytes > NET_DATA_BUFSIZE)
	{
		sc::writeLogError(sc::string::format("SendInstance %d dwSndBytes > NET_DATA_BUFSIZE", dwClient ));
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
			
			return SendInstance(dwClient, pIoWrite, dwSndBytes);
		}
	}
}
*/

int NetMatchClientMan::SendInstance(DWORD dwClient, NET_MSG_GENERIC *nmg)
{
	if (IsInstanceSlot(dwClient))
	{
		return SendServer(dwClient, nmg);
	}
	else
	{
		sc::writeLogError(sc::string::format("NetMatchClientMan::SendInstance() - Wrong Slot(%1%)", dwClient));
		return NET_ERROR;
	}
}

int NetMatchClientMan::SendAgent(DWORD dwClient, NET_MSG_GENERIC *nmg)
{
	if (IsAgentSlot(dwClient))
	{
		return SendServer(dwClient, nmg);
	}
	else
	{
		sc::writeLogError(sc::string::format("NetMatchClientMan::SendAgent() - Wrong Slot(%1%)", dwClient));
		return NET_ERROR;
	}
}

int	NetMatchClientMan::SendServer(DWORD dwClient, NET_MSG_GENERIC* pBuffer)
{
	if (pBuffer == NULL)
		return NET_ERROR;

	if ( INVALID_SOCKET == GetSocket( dwClient )  )
	{
		sc::writeLogError( sc::string::format( "NetMatchClientMan::SendServer(DWORD dwClient, NET_MSG_GENERIC* pBuffer) failure. INVALID_SOCKET == GetSocket( dwClient ) client: %1%", dwClient ) );
		return NET_ERROR;
	}

	PER_IO_OPERATION_DATA* pIoWrite = NULL;
	
	DWORD dwSndBytes = 0;

	dwSndBytes = pBuffer->Size();
	m_dwOutPacketCount++;
	m_dwOutPacketSize += dwSndBytes;

	if (dwSndBytes > NET_DATA_BUFSIZE)
	{
		sc::writeLogError(sc::string::format("SendServer %d dwSndBytes > NET_DATA_BUFSIZE", dwClient ));
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
			
			return SendServer(dwClient, pIoWrite, dwSndBytes);
		}
	}
}

static int nSendInstanceError = 0;

int NetMatchClientMan::SendServer(DWORD dwClient, PER_IO_OPERATION_DATA* PerIoData, DWORD dwSize)
{
	// MSG_OOB
	// MSG_DONTROUTE
	// MSG_PARTIAL
	INT		nRetCode = 0;
	DWORD	dwFlags = 0;
	DWORD	dwSndSize = dwSize;

	PerIoData->OperationType = NET_SEND_POSTED;
	PerIoData->DataBuf.len   = static_cast<u_long>(dwSize);

	//mjeon.AF
	GASSERT(dwClient < (DWORD)m_nMaxClient);
	if (dwClient >= (DWORD)m_nMaxClient)
	{
		ReleaseOperationData(PerIoData);
		return NET_ERROR;
	}

	SOCKET sSocket = INVALID_SOCKET;

	sSocket = m_ClientMan[dwClient]->GetSOCKET();
	/*
	if (IsInstanceSlot(dwClient))
	{
		sSocket = m_ClientMan[dwClient]->GetSOCKET();
	}
	else
	{
		sc::writeLogError(sc::string::format("NetMatchClientMan::SendInstance() - Wrong Slot(%1%)", dwClient));
		ReleaseOperationData(PerIoData);
		return NET_ERROR;
	}
	*/

	if (sSocket == INVALID_SOCKET)
	{
        sc::writeLogError(sc::string::format("SendServer GetFieldSocket %1%", dwClient));
		ReleaseOperationData(PerIoData);
		return NET_ERROR;
	}
	nRetCode = ::WSASend(
					sSocket,
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
			// 현재는 WSA_IO_PENDING 이외의 에러시에는 연결을 종료시켜버린다.
			// CloseField(dwClient);
			::PostQueuedCompletionStatus(
								m_hIOServer,
							    0,
							    dwClient, 
							    &PerIoData->Overlapped);
			if (nSendInstanceError < 100)
			{
				NET_MSG_GENERIC* pMsg = (NET_MSG_GENERIC*) PerIoData->Buffer;
				if (pMsg && pMsg->nType > 0)
				{
					sc::writeLogError(
                        sc::string::format(
                            "SendServer Client slot %1% Error %2% Type %3%",
                            dwClient,
                            nRetCode,
                            pMsg->nType));
					nSendInstanceError++;
				}
			}
			return NET_ERROR;
		}
	}

	return NET_OK;
}


int NetMatchClientMan::SetAcceptedClient(DWORD dwClient, SOCKET sSocket, net::SLOT_TYPE slotType)
{
    std::tr1::shared_ptr<CNetUser> spData = m_ClientMan[dwClient];	
	if (!spData)
        return NET_ERROR;

	if (spData->SetAcceptedClient(sSocket) == NET_OK)	//AgentServer or InstanceServer
	{
		/*
		for (int i=0; i<MAXNUM_OF_AGENTSERVER_TO_MATCH; i++)
		{
			if (m_pAgentServerInfo[i].IsValid())
			{
				std::string strIP(m_pAgentServerInfo[i].szPublicIp);
				
				if (strIP == spData->GetIP())
				{
					//
					//AgentServer와 InstanceServer는 (서로 다른 서버군의 AgentServer처럼) 다른 IP를 사용해야 한다.
					//
					//spData->SetSlotType(net::SLOT_AGENT);
					SetSlotAgent(i, dwClient);

					break;
				}
			}
		}
		*/
		
		return NET_OK;
	}
	else
	{
		return NET_ERROR;
	}
}

void NetMatchClientMan::SetSlotAgent(UINT n, DWORD dwClient)
{
	GASSERT(n < MAXNUM_OF_AGENTSERVER_TO_MATCH);

	if(n >= MAXNUM_OF_AGENTSERVER_TO_MATCH)
	{
		sc::writeLogError(sc::string::format("NetMatchClientMan::SetSlotAgent() - index(%1%) is greater than MAXNUM_OF_AGENTSERVER_TO_MATCH(%2%)", n, MAXNUM_OF_AGENTSERVER_TO_MATCH));

		return;
	}

	if(dwClient == net::SLOT_UNKNOWN)
	{
		sc::writeLogError(std::string("NetMatchClientMan::SetSlotAgent() - Wrong AgentSlot!"));

		return;
	}

	std::tr1::shared_ptr<CNetUser> spData = m_ClientMan[dwClient];
	if (!spData)
	{
		sc::writeLogError(std::string("NetMatchClientMan::SetSlotAgent() - NetUser does not exist!"));

		return;
	}

	spData->SetSlotType(net::SLOT_AGENT);

	if(m_pAgentServerInfo[n].NetSlotNum != net::SLOT_UNKNOWN)
	{
		sc::writeLogWarn(sc::string::format("AgentSlot[%1%] is changed from %2% to %3%",n,  m_pAgentServerInfo[n].NetSlotNum, dwClient));
	}
	else
	{
		sc::writeLogInfo(sc::string::format("AgentSlot[%1%] is set to %2%",n, dwClient));
	}

	m_pAgentServerInfo[n].NetSlotNum = dwClient;
}


void NetMatchClientMan::SetSlotAgent(DWORD dwClient)
{
	GASSERT(dwClient < (DWORD)m_nMaxClient);

	if (dwClient >= (DWORD) m_nMaxClient)
    {
		return;
    }

	if(dwClient == net::SLOT_UNKNOWN)
	{
		sc::writeLogError(std::string("NetMatchClientMan::SetSlotAgent() - Wrong AgentSlot!"));

		return;
	}

	for (int i=0; i<MAXNUM_OF_AGENTSERVER_TO_MATCH; i++)
	{
		if (m_pAgentServerInfo[i].IsValid())
		{
			std::string strIP(m_pAgentServerInfo[i].szPublicIp);
			
			if (strIP == m_ClientMan[dwClient]->GetIP())
			{
				SetSlotAgent(i, dwClient);

				break;
			}
		}
	}
}

void NetMatchClientMan::SetSlotInstance(UINT n, DWORD dwClient)
{
	GASSERT(n < MAXNUM_OF_INSTANCESERVER_TO_MATCH);

	if(n >= MAXNUM_OF_AGENTSERVER_TO_MATCH)
	{
		sc::writeLogError(sc::string::format("NetMatchClientMan::SetSlotInstance() - index(%1%) is greater than MAXNUM_OF_AGENTSERVER_TO_MATCH(%2%)", n, MAXNUM_OF_AGENTSERVER_TO_MATCH));

		return;
	}

	if(dwClient == net::SLOT_UNKNOWN)
	{
		sc::writeLogError(std::string("NetMatchClientMan::SetSlotInstance() - Wrong AgentSlot!"));

		return;
	}

	m_ClientMan[dwClient]->SetSlotType(net::SLOT_INSTANCE);
}

void NetMatchClientMan::SetSlotInstance(DWORD dwClient)
{
	if(dwClient == net::SLOT_UNKNOWN)
	{
		sc::writeLogError(std::string("NetMatchClientMan::SetSlotInstance() - Wrong AgentSlot!"));

		return;
	}

	m_ClientMan[dwClient]->SetSlotType(net::SLOT_INSTANCE);
}

bool NetMatchClientMan::IsAgentSlot(DWORD dwClient)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
    if (dwClient >= (DWORD) m_nMaxClient)
	{
        return false;
	}
    else
        return m_ClientMan[dwClient]->IsAgentSlot();
}

bool NetMatchClientMan::IsInstanceSlot(DWORD dwClient)
{
	GASSERT(dwClient<(DWORD)m_nMaxClient);
    if (dwClient >= (DWORD) m_nMaxClient)
	{
        return false;
	}
    else
        return m_ClientMan[dwClient]->IsInstanceSlot();
}


/*
DWORD NetMatchClientMan::GetSlotAgent()
{
	if(m_dwAgentSlot == net::SLOT_UNKNOWN)
	{
		sc::writeLogError(std::string("NetMatchClientMan::GetSlotAgent() - Wrong AgentSlot!"));
	}

	return m_dwAgentSlot;
}

void NetMatchClientMan::SetAgentIP(const char* szIP)
{
	if (szIP)
	    ::StringCchCopy(m_szAgentIP, MAX_IP_LENGTH+1, szIP);
}

int	NetMatchClientMan::SendAgent(DWORD dwClient, NET_MSG_GENERIC* pBuffer)
{
	assert(pBuffer);

	if (!pBuffer)
        return NET_ERROR;
	
	PER_IO_OPERATION_DATA* pIoWrite = NULL;
	
	DWORD dwSndBytes = 0;	
	dwSndBytes = pBuffer->Size();

	if (dwSndBytes > NET_DATA_BUFSIZE)
	{
		sc::writeLogError(
            sc::string::format(
                "SendAgent %1% dwSndBytes > NET_DATA_BUFSIZE", dwClient));
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
			return SendAgent(pIoWrite, dwSndBytes);
		}
        else
        {
            return NET_ERROR;
        }
	}
}

int	NetMatchClientMan::SendAgent(PER_IO_OPERATION_DATA* PerIoData, DWORD dwSize)
{
	// MSG_OOB
	// MSG_DONTROUTE
	// MSG_PARTIAL
	INT		nRetCode = 0;
	DWORD	dwFlags = 0;
	DWORD	dwSndSize = dwSize;

	DWORD	dwSlot = GetSlotAgent();

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

			sc::writeLogError(sc::string::format("SendAgent through slot %1% failure. ErrorCode: %2%", dwSlot, nRetCode));
			return NET_ERROR;		
		}
	}

	return NET_OK;
}
*/
