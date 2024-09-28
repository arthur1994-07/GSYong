#include "../pch.h"
#include "../../RanLogic/Msg/ServerMsg.h"
#include "../Client/NetAgentClientMan.h"
#include "./AgentServerConfigXml.h"
#include "./NetServerHelper.h"
#include "./AgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//! Agent Server 와 연결한다;
//! Agent 다운 후 Agent 가 재가동되었을때도 호출된다;
void CAgentServer::AgentConnectAll ()
{
	bool bReconnect = false;

	for ( DWORD i=0; i<AGENTSERVER_MAX; ++i )
	{
		if ( m_AgentServerInfo[ i ].IsValid() )
		{
			DWORD SlotNum = m_AgentServerInfo[ i ].NetSlotNum;
			if ( SlotNum == net::INVALID_SLOT )
			{
				bReconnect = true;

				AgentConnect( i );
			}
			else
			{
				if ( INVALID_SOCKET == m_pClientManager->GetSocket( SlotNum ) )
				{
					bReconnect = true;

					AgentSlotReset( i );

					AgentConnect( i );
				}
			}
		}
	}

	if ( bReconnect )
	{
		// Agent Server를 다시 연결했기 때문에 동기화를 위해서 리셋해준다;
		// Agent Server에 PERIOD time update;
		GLMSG::SNET_PERIOD NetPeriod;
		NetPeriod.sPRERIODTIME = GLPeriod::GetInstance().GetPeriod ();
		//SendAllAgent( &NetPeriod );

		// 무효화되는 정보 리셋을 필드에 요청;
		GLMSG::SNET_FIELDINFO_RESET NetMsgInfoReset;
		//SendAllChannel( &NetPeriod );

		// 무효화되는 정보 리셋;
		m_pGLAgentServer->ResetFieldInfo();
	}
}

void CAgentServer::FindAgentSlot ( DWORD dwClient, DWORD& dwAgentNum )
{
	for ( DWORD i=0; i<AGENTSERVER_MAX; ++i )
	{
		if ( m_AgentServerInfo[ i ].IsValid() )
		{
			if ( dwClient == m_AgentServerInfo[ i ].NetSlotNum )
			{
				dwAgentNum = i;

				return;
			}
		}
	}	
}

int CAgentServer::AgentConnect ( DWORD dwAgentNum )
{
	if ( dwAgentNum < 0 || dwAgentNum >= FIELDSERVER_MAX )
	{
		sc::writeLogError( sc::string::format(
			"Failed Connect Agent. Agent Number %1%", dwAgentNum ) );

		return NET_ERROR;
	}

	A_SERVER_INFO sAgentInfo = m_AgentServerInfo[ dwAgentNum ];
	if ( sAgentInfo.IsValid() )
	{
		DWORD dwSlotNum = sAgentInfo.NetSlotNum;

		if ( INVALID_SOCKET != m_pClientManager->GetSocket( dwSlotNum ) )
		{
			return NET_OK;
		}
		else
		{
			unsigned long ulTargetIP =
				m_AgentServerInfo[ dwAgentNum ].ulPublicIp;

			return AgentConnect(
				ulTargetIP,
				m_AgentServerInfo[ dwAgentNum ].nServicePort,
				dwAgentNum );
		}
	}

	return NET_ERROR;
}

int	CAgentServer::AgentConnect ( unsigned long ulServerIP
							   , int nPort
							   , DWORD dwAgentNum )
{
	// Slot Number를 획득한다;
	DWORD SlotNum = m_pClientManager->GetFreeClientID( true );
	if ( SlotNum == NET_ERROR )
	{
		sc::writeLogError( sc::string::format(
			"Failed AgentConnect GetFreeClientID Agent Number %1%",
			dwAgentNum ) );

		return NET_ERROR;
	}

	// Connect & Socket 을 획득하고 SlotNum과 연동한다;
	net::CONNECT_RESULT Connect = net::ConnectAndReceive(
		ulServerIP, nPort, m_hIOServer, SlotNum );
	int Result = Connect.get< 0 > ();
	SOCKET ConnectSocket = Connect.get< 1 > ();
	if ( Result == NET_ERROR )
	{
		m_pClientManager->ReleaseClientID( SlotNum );

		sc::writeLogError( sc::string::format(
			"Failed AgentConnect Connect Agent Number %1%",
			dwAgentNum ) );

		return NET_ERROR;
	}

	// Type is Agent Server, all slots are itself.
	if ( NET_ERROR == m_pClientManager->SetAcceptedClient(
		SlotNum, ConnectSocket, net::SLOT_AGENT_AGENT ) )
	{
		::closesocket( ConnectSocket );

		m_pClientManager->ReleaseClientID( SlotNum );

		sc::writeLogError( std::string( "Failed AgentConnect SetAcceptedClient" ) );

		return NET_ERROR;
	}

	m_pClientManager->SetSlotType( SlotNum, net::SLOT_AGENT_AGENT );

	PER_IO_OPERATION_DATA* pIOData =
		m_pClientManager->GetFreeOverIO( NET_RECV_POSTED );
	if ( NULL == pIOData )
	{
		CloseClient( SlotNum, net::ERROR_IO_OPERATION, WSAGetLastError() );

		sc::writeLogError( std::string( "Failed AgentConnect GetFreeOverIO" ) );

		return NET_ERROR;
	}

	// add reference
	m_pClientManager->AddReference( SlotNum );

	DWORD Flags = 0;
	DWORD dwRcvBytes = sizeof( NET_MSG_GENERIC );
	int nRetCode = WSARecv(
		ConnectSocket,
		&( pIOData->DataBuf ),
		1,
		&dwRcvBytes,
		&Flags,
		&( pIOData->Overlapped ),
		NULL );
	if ( nRetCode == SOCKET_ERROR &&
		( WSAGetLastError() != ERROR_IO_PENDING ) )
	{
		// release reference
		m_pClientManager->ReleaseReference( SlotNum );
		m_pClientManager->ReleaseOperationData( pIOData );

		CloseClient( SlotNum, net::ERROR_IO_OPERATION, WSAGetLastError() );

		sc::writeLogError( sc::string::format(
			"AgentConnect WSARecv %1%",
			WSAGetLastError() ) );

		return NET_ERROR;
	}

	sc::writeLogInfo( sc::string::format(
		"Agent Server Agent Number %1% Slot %2% Connection OK",
		dwAgentNum,
		SlotNum ) );

	// Setting Slot;
	AgentSlotSet( dwAgentNum, SlotNum );
}

int	CAgentServer::AgentCloseConnect ( DWORD dwSlotNum )
{
	SOCKET sTemp = m_pClientManager->GetSocket( dwSlotNum );
	if ( sTemp != INVALID_SOCKET )
	{	
		int nRetCode = ::closesocket( sTemp );
		if ( nRetCode == SOCKET_ERROR )
		{
			sc::writeLogInfo( sc::string::format(
				"AgentCloseConnect slot %1% closesocket %2%",
				dwSlotNum,
				WSAGetLastError() ) );
		}

		m_pClientManager->Reset( dwSlotNum );
		m_pClientManager->ReleaseClientID( dwSlotNum );

		AgentSlotReset( dwSlotNum );

		return NET_OK;
	}
	else
	{
		sc::writeLogError( std::string( "AgentCloseConnect INVALID_SOCKET" ));
		return NET_OK;
	}
}

void CAgentServer::AgentCloseConnectAll ()
{
	if ( m_pClientManager )
	{
		for ( size_t i=0; i<AGENTSERVER_MAX; ++i )
		{
			DWORD dwSlotNum = m_AgentServerInfo[i].NetSlotNum;
			if ( dwSlotNum != net::INVALID_SLOT &&
				m_pClientManager->IsOnline( dwSlotNum ) )
			{
				AgentCloseConnect( dwSlotNum );
			}
		}	
	}
}

DWORD CAgentServer::GetAgentSlotNum ( DWORD dwAgentNum )
{
	if ( dwAgentNum >= 0 && dwAgentNum < AGENTSERVER_MAX )
	{	
		return m_AgentServerInfo[ dwAgentNum ].NetSlotNum;
	}

	return 0;
}

void CAgentServer::AgentSlotSet ( DWORD dwAgentNum, DWORD dwSlotNum )
{
	if ( dwAgentNum >= 0 && dwAgentNum < AGENTSERVER_MAX )
	{	
		m_AgentServerInfo[ dwAgentNum ].NetSlotNum = dwSlotNum;
	}
}

void CAgentServer::AgentSlotReset ( DWORD dwAgentNum )
{
	if ( dwAgentNum >= 0 && dwAgentNum < AGENTSERVER_MAX )
		m_AgentServerInfo[ dwAgentNum ].NetSlotNum = net::INVALID_SLOT;
}

void CAgentServer::CloseClientAgentAgent ( DWORD dwClient, net::CLOSE_TYPE emCloseType )
{
	// Agent Server와 연결 종료 됨;
	if ( INVALID_SOCKET != m_pClientManager->GetSocket( dwClient ) )
	{
		DWORD dwAgentNum = net::INVALID_SLOT;

		FindAgentSlot( dwClient, dwAgentNum );

		if ( net::INVALID_SLOT != dwAgentNum )
		{
			sc::writeLogError(
				sc::string::format(
				"AgentServer NUM: %1% Connection is closed. Trying to reconnect. Close Type %2%",
				dwAgentNum,
				emCloseType ) );
		}
		else
		{
			sc::writeLogError( sc::string::format(
				"AgentServer Connection is closed. Trying to reconnect. Close Type %1%",
				emCloseType ) );
		}

		m_pGLAgentServer->SENDTOMYSELF(
			&GLMSG::SNET_SERVER_CLOSE_AGENT( dwAgentNum ) );
	}

	int nResult = m_pClientManager->CloseClient( dwClient );

	// 서버가 운영 중이라면 Agent Server와 다시 연결시도를 한다;
	if ( m_bIsRunning )
		AgentConnectAll();
}