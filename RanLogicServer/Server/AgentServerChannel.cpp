#include "../pch.h"
#include "../../RanLogic/Msg/ServerMsg.h"
#include "../../RanLogic/Msg/GLContrlMsg.h"
#include "../Client/NetAgentClientMan.h"
#include "./AgentServerConfigXml.h"
#include "./NetServerHelper.h"
#include "./AgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

int CAgentServer::IncreaseChannelUser(int nChannel)
{
	//lockOn();
	lockOn();
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER)
	{
		lockOff();
		//lockOff();
		return NET_ERROR;
	}
	else
	{
		++m_ChannelUser[nChannel];

		sc::writeLogDebug( sc::string::format(
			"[ Channel Info ] [ %1% Channel, User Count : %2% ]", nChannel, m_ChannelUser[nChannel] ) );

		lockOff();
		//lockOff();
		SessionSndChannelState(nChannel);



		/*if( m_ChannelUser[nChannel] >= (m_pClientManager->GetCurrentClientNumber() + NET_RESERVED_SLOT) )
		{
			CConsoleMessage::GetInstance()->WriteConsole(_T("INFO:Channel(%d) CHUserNum(%d) >= CurrentNumber+100(%d)"),	
														     nChannel, m_ChannelUser[nChannel], (m_pClientManager->GetCurrentClientNumber() + NET_RESERVED_SLOT));
		}*/

		/*sc::writeLogError(
			_T("### INCREASE_CHANNEL_USER   CHUserNum : %d deqIDSize : %d CurrentClientNum : %d"), 
			m_ChannelUser[nChannel], m_pClientManager->m_deqSleepCID.size(), m_pClientManager->GetCurrentClientNumber() );*/


		return NET_OK;
	}
}

int CAgentServer::DecreaseChannelUser(int nChannel)
{	
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER)
	{
		sc::writeLogError(
            sc::string::format(
                "DecreaseChannelUser Channel Number Wrong %1%", nChannel));
		return NET_ERROR;
	}
	else
	{
		--m_ChannelUser[nChannel];

		sc::writeLogDebug( sc::string::format(
			"[ Channel Info ] [ %1% Channel, User Count : %2% ]", nChannel, m_ChannelUser[nChannel] ) );

		SessionSndChannelState(nChannel);
		return NET_OK;
	}
}

int CAgentServer::GetChannelUser(int nChannel)
{
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER)
	{
		sc::writeLogError(sc::string::format("GetChannelUser Channel Number Wrong(%d)", nChannel));
		return NET_ERROR;
	}
	else
	{
		return m_ChannelUser[nChannel];
	}
}

bool CAgentServer::IsChannelFull(int nChannel)
{
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER)
	{
		sc::writeLogError(sc::string::format("IsChannelFull nChannel:%d", nChannel ));
		return true;
	}
	else
	{
		// if (m_ChannelUser[nChannel] > (m_nMaxChannelUser-NET_RESERVED_SLOT))
		if (m_ChannelUser[nChannel] >= m_nMaxChannelUser)
		{
			sc::writeLogInfo(
                sc::string::format(
                    "INFO:Channel(%d) is full, CHUserNum : %d vecIDSize : %d deqIDSize : %d MaxClientNum : %d CurrentClientNum : %d ",
                    nChannel,
                    m_ChannelUser[nChannel],
                    m_pClientManager->GetVecSleepSize(),
                    m_pClientManager->GetDeqSleepSize(),	
                    m_pClientManager->GetMaxClient(),
                    m_pClientManager->GetCurrentClientNumber()));
			return true;
		}
		else
		{
			return false;
		}
	}
}

void CAgentServer::ChangeChannelUser ( DWORD dwClient, int nPreChannel, int nAfterChannel )
{
	// 채널의 유저수를 조정한다;
	DecreaseChannelUser( nPreChannel );
	IncreaseChannelUser( nAfterChannel );

	// 유저의 채널정보를 변경한다;
	m_pClientManager->SetChannel( dwClient, nAfterChannel );
}

void CAgentServer::FindFieldSlot( DWORD dwClient, DWORD& dwChannel, DWORD& dwFieldNum )
{
	for ( DWORD i=0; i<MAX_CHANNEL_NUMBER; ++i )
	{
		for ( DWORD j=0; j<FIELDSERVER_MAX; ++j )
		{
			if ( m_FieldServerInfo[i][j].IsValid() )
			{
				if ( dwClient == m_FieldServerInfo[i][j].NetSlotNum )
				{
					dwChannel = i;
					dwFieldNum = j;
					return;
				}
			}
		}
	}
}

//! 필드서버와 연결한다.
//! Field 다운후 Field 가 재 가동되었을때도 호출된다.
void CAgentServer::FieldConnectAll()
{
	bool bReconnect = false;

    for ( DWORD i=0; i<MAX_CHANNEL_NUMBER; ++i )
    {
        for ( DWORD j=0; j<FIELDSERVER_MAX; ++j )
        {
            if ( m_FieldServerInfo[i][j].IsValid() )
            {
                DWORD SlotNum = m_FieldServerInfo[i][j].NetSlotNum;
                if ( SlotNum == net::INVALID_SLOT )
                {
                    bReconnect = true;
                    FieldConnect( j, i );
                }
                else
                {
                    if ( INVALID_SOCKET == m_pClientManager->GetSocket( SlotNum )/*!m_pClientManager->IsOnline( SlotNum )*/ )
                    {
                        bReconnect = true;
                        FieldSlotReset( i, j );
                        FieldConnect( j, i );
                    }
                }
            }
        }
    }
	
    if (bReconnect)
	{
		// 필드를 다시 연결했기 때문에 동기화를 위해서 리셋해준다.
		// 필드 서버에 PERIOD time update
		GLMSG::SNET_PERIOD NetPeriod;
		NetPeriod.sPRERIODTIME = GLPeriod::GetInstance().GetPeriod ();
		SendAllChannel( &NetPeriod );

		// 무효화되는 정보 리샛을 필드에 요청.		
		GLMSG::SNET_FIELDINFO_RESET NetMsgInfoReset;
		SendAllChannel( &NetPeriod );

		// 무효화되는 정보 리셋.
		m_pGLAgentServer->ResetFieldInfo();
	}
}

int CAgentServer::FieldConnect( DWORD FieldNum, DWORD Channel )
{
	if ( Channel >= static_cast< DWORD >( m_nServerChannelNumber ) || FieldNum >= FIELDSERVER_MAX )
	{
		sc::writeLogError( sc::string::format( "FieldConnect fail. Channel %1% Field %2%", Channel, FieldNum ) );
		return NET_ERROR;
	}

    F_SERVER_INFO* pFieldInfo = GetFieldServerInfo( FieldNum, Channel );
	if ( pFieldInfo && pFieldInfo->IsValid() )
	{
        DWORD SlotNum = pFieldInfo->NetSlotNum;

		if ( INVALID_SOCKET != m_pClientManager->GetSocket( SlotNum )/*m_pClientManager->IsOnline( SlotNum )*/ )
		{
			return NET_OK;
		}
		else
		{
			// 에이전트서버가 필드서버에 접속할 때 내부 아이피로 접속하도록 한다. 우선, GS만 대상으로 하고 나중에 모든 국가로 한다.
			unsigned long ulTargetIP = 0;
			switch ( GetServiceProvider() )
			{
			case SP_KOREA:
			case SP_GS:
				ulTargetIP = m_FieldServerInfo[Channel][FieldNum].ulPrivateIP;
				break;
			default:
				ulTargetIP = m_FieldServerInfo[Channel][FieldNum].ulPublicIp;
				break;
			}

			return FieldConnect(
						ulTargetIP,
				        m_FieldServerInfo[Channel][FieldNum].nServicePort,
						FieldNum,
						Channel);
		}
	}
//	else
//	{
//		// 이 부분이 필요하나? 여기로 안올텐데
//		sc::writeLogError( sc::string::format( "FieldConnect CH %1% FIELD %2%", Channel, FieldNum ) );
//		m_pGLAgentServer->SetFieldOnline( Channel, FieldNum, false ); // 오프라인 된 필드 서버를 알린다. by luxes.
//		return NET_ERROR;
//	}
	return NET_ERROR;
}

int	CAgentServer::FieldConnect( unsigned long ulServerIP, int nPort, DWORD FieldNum, DWORD Channel )
{	
    DWORD SlotNum = m_pClientManager->GetFreeClientID( true );
    if ( SlotNum == NET_ERROR )
	{
		sc::writeLogError( sc::string::format( "FieldConnect GetFreeClientID failed CH %1% FIELD %2%", Channel, FieldNum ) );
        return NET_ERROR;
	}

    net::CONNECT_RESULT Connect = net::ConnectAndReceive( ulServerIP, nPort, m_hIOServer, SlotNum );
    int Result = Connect.get<0> ();
    SOCKET ConnectSocket = Connect.get<1> ();
    if ( Result == NET_ERROR )
    {
        m_pClientManager->ReleaseClientID( SlotNum );
		sc::writeLogError( sc::string::format( "FieldConnect connect failed CH %1% FIELD %2%", Channel, FieldNum ) );
		return NET_ERROR;
	}

	// Head receive mode
	//m_pClientManager->SetNetMode(dwID, NET_PACKET_HEAD);
    
	// Type is FIELD Server, all slots are itself.
	if ( NET_ERROR == m_pClientManager->SetAcceptedClient( SlotNum, ConnectSocket, net::SLOT_AGENT_FIELD ) )
	{
		::closesocket( ConnectSocket );
		m_pClientManager->ReleaseClientID( SlotNum );
		sc::writeLogError( std::string( "FieldConnect SetAcceptedClient failed" ) );
		return NET_ERROR;
	}

	m_pClientManager->SetSlotType( SlotNum, net::SLOT_AGENT_FIELD );
	m_pClientManager->SetChannel( SlotNum, Channel );

	PER_IO_OPERATION_DATA* pIOData = m_pClientManager->GetFreeOverIO( NET_RECV_POSTED );
	if ( NULL == pIOData )
	{
		CloseClient( SlotNum, net::ERROR_IO_OPERATION, WSAGetLastError() );
		sc::writeLogError( std::string( "FieldConnect GetFreeOverIO failed" ) );
		return NET_ERROR;
	}

	// add reference
	m_pClientManager->AddReference( SlotNum );

	DWORD Flags = 0;
	DWORD dwRcvBytes = sizeof( NET_MSG_GENERIC );
	int nRetCode = WSARecv( ConnectSocket, &( pIOData->DataBuf ), 1, &dwRcvBytes, &Flags, &( pIOData->Overlapped ), NULL );
	if ( nRetCode == SOCKET_ERROR && ( WSAGetLastError() != ERROR_IO_PENDING ) )
	{
		// release reference
		m_pClientManager->ReleaseReference( SlotNum );
		m_pClientManager->ReleaseOperationData( pIOData );
		CloseClient( SlotNum, net::ERROR_IO_OPERATION, WSAGetLastError() );
		sc::writeLogError( sc::string::format( "FieldConnect WSARecv %1%", WSAGetLastError() ) );
		return NET_ERROR;
	}

	sc::writeLogInfo( sc::string::format( "Field Server CH %1% FIELD %2% Slot %3% Connection OK", Channel, FieldNum, SlotNum ) );

    FieldSlotSet( Channel, FieldNum, SlotNum );
	m_pGLAgentServer->SetFieldOnline( Channel, FieldNum, true ); // 온라인 된 필드 서버를 알린다. by luxes.

	// 필드서버에 접속후 자신이 Agent 서버임을 알리고 서버간 통신을 위한 정보를 전송한다.
    NET_I_AM_AGENT nmg;
    nmg.CrcValue = sc::string::getCrc32( &nmg, nmg.Size()-sizeof( unsigned int ) );
	int nResult = SendClient( SlotNum, &nmg );
	if ( NET_OK == nResult )
	{
		// 생뚱맞지만 ctf 시간을 보내야 한다. 추후에 수정합니다.
		GLMSG::SNET_CTF_NEXT_READY_TIME msgAF2;
		msgAF2.ctReadyTime = m_pGLAgentServer == NULL ? CTime::GetCurrentTime() : m_pGLAgentServer->GetCTFReadyTime();
		SendClient( SlotNum, &msgAF2 );
	}
	return nResult;
}

void CAgentServer::BroadcastToField(NET_MSG_GENERIC* pBuffer, DWORD ServerChannel)
{
	if (!pBuffer || ServerChannel >= static_cast<DWORD>(m_nServerChannelNumber))
	{
		sc::writeLogError(sc::string::format("BroadcastToField CH %1%", ServerChannel));
		return;
	}	

	for (size_t j=0; j<FIELDSERVER_MAX; ++j)
    {
        DWORD SlotNum = m_FieldServerInfo[ServerChannel][j].NetSlotNum;
        if (SlotNum != net::INVALID_SLOT &&	m_pClientManager->IsOnline(SlotNum))
	    {
		    if (BroadcastToField(SlotNum, pBuffer) == NET_ERROR)
		    {
			    sc::writeLogError(
                    sc::string::format(
                        "BroadcastToField:%1% CH:%2%", SlotNum, ServerChannel));
		    }
	    }
    }

	/*
    for (size_t i=0; i<MAX_CHANNEL_NUMBER; ++i)
    {
        for (size_t j=0; j<FIELDSERVER_MAX; ++j)
        {
            DWORD SlotNum = m_FieldServerInfo[i][j].NetSlotNum;
            if (SlotNum != net::INVALID_SLOT &&		
			    m_pClientManager->IsOnline(SlotNum) &&
			    m_pClientManager->GetChannel(SlotNum) == ServerChannel)
		    {
			    if (BroadcastToField(SlotNum, pBuffer) == NET_ERROR)
			    {
				    sc::writeLogError(
                        sc::string::format(
                            "BroadcastToField:%1% CH:%2%", SlotNum, ServerChannel));
			    }
		    }
	    }
    }
	*/
}

int CAgentServer::SendAllChannel(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        return SendAllChannel(&MsgPack);

	return 0;
}

int CAgentServer::SendAllChannel(NET_MSG_GENERIC* pBuffer)
{ 
	int nSendCount(0);
	if (pBuffer)
    {
        for (size_t i=0; i<MAX_CHANNEL_NUMBER; ++i)
        {
            for (size_t j=0; j<FIELDSERVER_MAX; ++j)
            {
                DWORD SlotNum = m_FieldServerInfo[i][j].NetSlotNum;
                if (SlotNum != net::INVALID_SLOT && m_pClientManager->IsOnline(SlotNum))
                {
			        if (BroadcastToField(SlotNum, pBuffer) == NET_ERROR)
				        sc::writeLogError(sc::string::format("SendAllChannel Slot %1% Type %2%", SlotNum, pBuffer->Type()));
					else
						++nSendCount;
			    }
		    }
	    }
    }
	return nSendCount;
}

int CAgentServer::BroadcastToField(DWORD SlotNum, NET_MSG_GENERIC* pBuffer)
{
	return SendClient(SlotNum, pBuffer);
}

int	CAgentServer::BroadcastToField(DWORD FieldNum, NET_MSG_GENERIC* pBuffer, DWORD Channel)
{
    if (Channel >= MAX_CHANNEL_NUMBER && FieldNum >= FIELDSERVER_MAX)
        return NET_ERROR;

	DWORD SlotNum = m_FieldServerInfo[Channel][FieldNum].NetSlotNum;
    if (SlotNum != net::INVALID_SLOT)
	    return SendClient(SlotNum, pBuffer);
    else
        return NET_ERROR;
}

int CAgentServer::FieldCloseConnect(DWORD SlotNum)
{
    SOCKET sTemp = m_pClientManager->GetSocket(SlotNum);
	if (sTemp != INVALID_SOCKET)
	{	
		int nRetCode = ::closesocket(sTemp);
		if (nRetCode == SOCKET_ERROR)
			sc::writeLogInfo(sc::string::format("FieldCloseConnect slot %1% closesocket %2%", SlotNum, WSAGetLastError()));
		m_pClientManager->Reset(SlotNum);
        m_pClientManager->ReleaseClientID(SlotNum);
        FieldSlotReset(SlotNum);
		return NET_OK;
	}
	else
	{
		sc::writeLogError( std::string("FieldCloseConnect INVALID_SOCKET"));
		return NET_OK;
	}
}

void CAgentServer::FieldCloseConnectAll()
{
	if (m_pClientManager)
    {
        for (size_t i=0; i<MAX_CHANNEL_NUMBER; ++i)
        {
            for (size_t j=0; j<FIELDSERVER_MAX; ++j)
            {
                DWORD SlotNum = m_FieldServerInfo[i][j].NetSlotNum;
                if (SlotNum != net::INVALID_SLOT && m_pClientManager->IsOnline(SlotNum))
		        {
                    FieldCloseConnect(SlotNum);
		        }
            }
	    }
    }
}

void CAgentServer::FieldSlotReset(DWORD Channel, DWORD FieldNum)
{
    if (Channel < MAX_CHANNEL_NUMBER && FieldNum < FIELDSERVER_MAX)
        m_FieldServerInfo[Channel][FieldNum].NetSlotNum = net::INVALID_SLOT;
}

void CAgentServer::FieldSlotReset(DWORD SlotNum)
{
    for (size_t i=0; i<MAX_CHANNEL_NUMBER; ++i)
    {
        for (size_t j=0; j<FIELDSERVER_MAX; ++j)
        {
            if (m_FieldServerInfo[i][j].NetSlotNum == SlotNum)
                m_FieldServerInfo[i][j].NetSlotNum = net::INVALID_SLOT;
        }
    }
}

void CAgentServer::FieldSlotSet(DWORD Channel, DWORD FieldNum, DWORD SlotNum)
{
    if (Channel < MAX_CHANNEL_NUMBER && FieldNum < FIELDSERVER_MAX)
	{	
        m_FieldServerInfo[Channel][FieldNum].NetSlotNum = SlotNum;
	}
}
