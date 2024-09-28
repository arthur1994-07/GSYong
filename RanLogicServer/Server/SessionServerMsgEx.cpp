#include "pch.h"
#include "../../EngineLib/Common/profile.h"
#include "../../RanLogic/Msg/TraceMsg.h"
#include "../../RanLogic/Msg/ServerMsg.h"
#include "../../RanLogic/Msg/GLContrlServerMsg.h"
#include "../../RanLogic/s_NetGlobal.h"

#include "../Client/NetSessionClientMan.h"
#include "./SessionServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CSessionServer::MsgHeartbeatServerAns(DWORD dwClient, MSG_LIST* pMsg)
{
#ifdef HEARTBEAT_LOG
	sc::writeLogInfo(sc::string::format("[HEARTBEAT] Recv Heartbeat Response from ( %1% ) Svr", dwClient));
#endif
	m_pClientManager->SetHeartBeat(dwClient);
}

void CSessionServer::MsgServerHeartbeatReq(DWORD dwClient, MSG_LIST* pMsg)
{
#ifdef HEARTBEAT_LOG
	sc::writeLogInfo(sc::string::format("[HEARTBEAT] Recv Heartbeat Request from ( %1% ) Svr", dwClient));
#endif
	NET_HEARTBEAT_SERVER_ANS msg;
	SendClient(dwClient, &msg);
}

int CSessionServer::FindServer(CString strIP, int nPort)
{
	int nMaxClient = m_pClientManager->GetMaxClient();

	for (int i=0; i<nMaxClient; ++i)
	{		
		if ((m_pClientManager->IsOnline((DWORD) i) == true) && 
		    (m_pClientManager->GetClientIP((DWORD)i) == strIP.GetString()) &&
			(nPort == m_pClientManager->GetSvrServicePort((DWORD)i)))
		{					
			return i;
		}
	}
	return NET_ERROR;
}

int CSessionServer::FindServer(int nSvrType, int nSvrGrp, int nSvrNum, int nFieldNum)
{
	int nMaxClient = m_pClientManager->GetMaxClient();

	for (int i=0; i<nMaxClient; ++i)
	{
		if (nSvrType == net::SERVER_FIELD)
		{
			if ((m_pClientManager->IsOnline((DWORD) i) == true) && 
				(m_pClientManager->GetSvrGrp((DWORD) i) == nSvrGrp) &&
				(m_pClientManager->GetSvrNum((DWORD) i) == nSvrNum) &&
				(m_pClientManager->GetSvrField((DWORD) i) == nFieldNum))
			{				
				sc::writeLogInfo( sc::string::format("Find Server %d", i) );
				return i;
			}
		}
		else
		{
			if ((m_pClientManager->IsOnline((DWORD) i) == true) && 
				(m_pClientManager->GetSvrGrp((DWORD) i) == nSvrGrp) &&
				(m_pClientManager->GetSvrNum((DWORD) i) == nSvrNum) &&
				(m_pClientManager->GetSvrType((DWORD) i) == nSvrType))
			{	
				sc::writeLogInfo( sc::string::format("Find Server %d", i) );
				return i;
			}
		}
	}
	sc::writeLogError(std::string("Can't Find Server"));
	return NET_ERROR;
}

int CSessionServer::MsgSndServerStateChange(int nClient, int nState)
{
	NET_SVR_CMD_CHANGE_STATE sTemp;
	sTemp.nCmdType = nState;
	return SendClient(static_cast<DWORD> (nClient), &sTemp);
}

void CSessionServer::MsgTracingChar(DWORD dwClient, MSG_LIST* pMsg)
{
	//
	//mjeon.STracingData�� std::string�� ���ԵǾ� �־ Release ������ Debug ���� ���̿��� ������ �ȴ�. �� ��, �� �޽������� std::string�� �����Ѵ�.
	//
#ifdef _DEBUG
	return;
#endif

	if (!pMsg)
		return;

	GLMSG::NET_UPDATE_TRACINGCHAR* pPacket = reinterpret_cast <GLMSG::NET_UPDATE_TRACINGCHAR*> (pMsg->Buffer);
	
	if ( static_cast< BYTE >( m_vecTracingData.size() ) > pPacket->updateNum )
	{
		m_vecTracingData[pPacket->updateNum] = pPacket->tracingData;

		// �� ������ ���� �߰��� ĳ���͵鿡 ���� ������ ������.
		int nMaxClient = m_pClientManager->GetMaxClient();
		for (int i=0; i<nMaxClient; ++i)
		{
			// �¶����̰� ���Ӽ������ ä�ø޽����� ������.
			if (m_pClientManager->IsOnline((DWORD) i) == true &&
				m_pClientManager->GetSvrType(i) != net::SERVER_AGENT &&
				m_pClientManager->GetSvrType(i) != net::SERVER_FIELD)
			{
				SendClient((DWORD) i, pPacket);
			}
		}
	}
}

void CSessionServer::MsgTracingCharAll(DWORD dwClient, MSG_LIST* pMsg)
{
	//
	//mjeon.STracingData�� std::string�� ���ԵǾ� �־ Release ������ Debug ���� ���̿��� ������ �ȴ�. �� ��, �� �޽������� std::string�� �����Ѵ�.
	//
#ifdef _DEBUG
	return;
#endif

	if( pMsg == NULL )
		return;
	
	m_vecTracingData.clear();
	GLMSG::NET_UPDATE_TRACINGCHAR_ALL* msg = reinterpret_cast <GLMSG::NET_UPDATE_TRACINGCHAR_ALL*> (pMsg->Buffer);
	
	for (int i=0; i<MAX_TRACING_NUM; ++i)
	{		
        std::string AccountName(msg->tracingData[i].m_szAccount);
		if (msg->tracingData[i].dwUserNum == 0 && AccountName.empty() )
			break;

		m_vecTracingData.push_back(msg->tracingData[i]);
	}	

	// �� ������ ���� �߰��� ĳ���͵鿡 ���� ������ ������.
	int nMaxClient = m_pClientManager->GetMaxClient();
	for (int i=0; i<nMaxClient; ++i)
	{
		//�ش� �޽����� ���� Ŭ���̾�Ʈ �ڽ��� ����.
		if( i == pMsg->dwClient ) 
			continue;

		// �¶����̰� ���Ӽ������ �ش� �޽����� ������.
		if( m_pClientManager->IsOnline((DWORD) i) == true  )
		{
			SendClient((DWORD) i, msg);			
		}
	}
}

void CSessionServer::MsgChatProcess(DWORD dwClient, MSG_LIST* pMsg)
{
    if (!pMsg)
        return;

	NET_CHAT_CTRL* pPacket = (NET_CHAT_CTRL*) pMsg->Buffer;
	
	sc::writeLogError(std::string("Chat message"));

	switch (pPacket->emType) 
	{
	// �������α׷����� ���� ��ü�޽���
	case CHAT_TYPE_CTRL_GLOBAL2: 
		//
		//mjeon.ToDo
		//MsgSndChatCtrlGlobal2() sends specific message to AgentServer with server type flag.
		//The function name and emType(CHAT_TYPE_CTRL_GLOBAL2) need to be renamed to be more clear.
		//
		MsgSndChatCtrlGlobal2(pMsg);
		break;
	case CHAT_TYPE_GLOBAL:
	case CHAT_TYPE_CTRL_GLOBAL:		
		//
		//mjeon
		//MsgSndChatCtrlGlobal() sends specific message to the server specified with IP adddress in pMsg.		
		//
		MsgSndChatCtrlGlobal(pMsg);
		break;
	default:
		break;
	}	

	//
	//mjeon.ToDo
	//Following code looks like a debugging code unnecessary.
	//Block the codes with comment.
	//

	/*int test;
	if( nc->emType == CHAT_TYPE_GLOBAL  )
	{
		test = 0;
	}*/
}

// �������α׷����� ���� ��ü ���� ä�� �޽����� �ش缭���� �����Ѵ�
void CSessionServer::MsgSndChatCtrlGlobal(MSG_LIST* pMsg)
{
    if (pMsg == NULL) return;

	sc::writeLogInfo(std::string("Notice Message"));

	NET_CHAT_CTRL* ncc = reinterpret_cast<NET_CHAT_CTRL*> (pMsg->Buffer);    

	// Find Server	
	DWORD dwClient = m_pClientManager->FindServer(ncc->szServerIP, ncc->nServicePort);
	if (dwClient == NET_ERROR) // �ش� ������ ã�� ����
	{
        sc::writeLogError(std::string("Can't Find Server"));
		return;
	}

	// Send Global message
	NET_CHAT_CTRL nc;
	nc.nType  = NET_MSG_CHAT_CTRL;
	nc.emType = CHAT_TYPE_GLOBAL;
	::StringCchCopy(nc.szChatMsg, GLOBAL_CHAT_MSG_SIZE+1, ncc->szChatMsg);
	SendClient(dwClient, &nc);

	sc::writeLogInfo(std::string(nc.szChatMsg));
}

// �������α׷����� ���� ��ü ���� ä�� �޽����� �ش缭���� �����Ѵ�
void CSessionServer::MsgSndChatCtrlGlobal2(MSG_LIST* pMsg)
{
    if (pMsg == NULL) return;

    sc::writeLogInfo(std::string("Notice Message"));

	NET_CHAT_CTRL2* ncc = reinterpret_cast <NET_CHAT_CTRL2*> (pMsg->Buffer);
	
    // Find AgentServer	
	DWORD dwClient = m_pClientManager->FindServer(net::SERVER_AGENT, ncc->nSvrGrp, 0, 0);
	if (dwClient == NET_ERROR) // �ش� ������ ã�� ����
	{
		sc::writeLogInfo(std::string("INFO:Can't Find Server"));
		return;
	}
	
	// Send Global message
	NET_CHAT_CTRL nc;
	nc.nType  = NET_MSG_CHAT_CTRL;
	nc.emType = CHAT_TYPE_GLOBAL;
	nc.SetMsg(ncc->szChatMsg);
	SendClient(dwClient, &nc);

	sc::writeLogInfo(std::string(nc.szChatMsg));
}

void CSessionServer::MsgSndSvrInfoReset(DWORD dwClient)
{		
	NET_MSG_GENERIC	nmg;	
	nmg.SetSize( sizeof(NET_MSG_GENERIC) );
	nmg.nType = NET_MSG_SND_SVR_INFO_RESET;
	SendClient(dwClient, &nmg);
}

/// ���Ǽ���->�α��μ��� : ���Ӽ����� ���������� �����Ѵ�.
void CSessionServer::MsgSndCurSvrInfo(DWORD dwClient, MSG_LIST* pMsg)
{
	// server type [type]
	// [type]
	// 1 : login server
	// 2 : session server
	// 3 : field server
	// 4 : agent server

	// LoginServer�� ClientID�� �����Ѵ�.
	m_dwLoginServerID = dwClient;

	NET_CUR_INFO_LOGIN ncil;
	ncil.SetType(NET_MSG_SND_SVR_INFO);

	for (int nGroup=0; nGroup < MAX_SERVER_GROUP; ++nGroup)
	{
		for (int nChannel=0; nChannel < MAX_CHANNEL_NUMBER; ++nChannel)
		{
			if (m_sServerChannel[nGroup][nChannel].nServerMaxClient > 0)
			{
				ncil.gscil               = m_sServerChannel[nGroup][nChannel];
				ncil.gscil.nServerNumber = nChannel; // Channel number

				SendClient(dwClient, &ncil);
				sc::writeLogInfo(
                    sc::string::format("MsgSndCurSvrInfo %1%",
                        m_sServerChannel[nGroup][nChannel].szServerIP ));
			}
		}
	}
}

//	Agent������ ������.
void CSessionServer::MsgSndChatGlobal(char* szChatMsg)
{
    if (szChatMsg == NULL)
        return;

	NET_CHAT_CTRL nc;	
	nc.nType  = NET_MSG_CHAT_CTRL;
	nc.emType = CHAT_TYPE_GLOBAL;
	::StringCchCopy(nc.szChatMsg, GLOBAL_CHAT_MSG_SIZE+1, szChatMsg);

	int nMaxClient = m_pClientManager->GetMaxClient();

	for (int i=0; i<nMaxClient; ++i)
	{
		// �¶����̰� ���Ӽ������ ä�ø޽����� ������.
		if ((m_pClientManager->IsOnline((DWORD) i) == true) && 
			(m_pClientManager->IsGameServer((DWORD) i) == true)) 
		{
			SendClient((DWORD) i, &nc);
		}
	}
}

// Session->ServerManager:�¶��� ������ ��� ������ ���������� ������ �ش�
void CSessionServer::MsgSndAllSvrInfo(DWORD dwClient, MSG_LIST* pMsg)
{
    if (!pMsg)
        return;

	int nMax = m_pClientManager->GetMaxClient();

	// Start of server info
	NET_MSG_GENERIC nmg;
	nmg.SetSize( sizeof(NET_MSG_GENERIC) );
	nmg.nType = NET_MSG_SND_ALL_SVR_INFO_S;	//< ���Ǽ���->�������α׷� : ������������ ����
	SendClient(dwClient, &nmg);
	
	// server info
	for (DWORD dwSvr=0; dwSvr<(DWORD) nMax; ++dwSvr)
	{
		if (m_pClientManager->IsOnline(dwSvr) == true)
		{
            NET_SERVER_INFO nsi(m_pClientManager->GetSvrInfo(dwSvr));            
			nsi.SetType(NET_MSG_SND_ALL_SVR_INFO);
			SendClient(dwClient, &nsi);

			sc::writeLogInfo(
                sc::string::format(
                    "MsgSndAllSvrInfo %s %d %d",
                    nsi.gsi.szServerIP,
                    nsi.gsi.nControlPort,
                    nsi.gsi.ServerType));
		}
	}

	// End of server info	
    NET_MSG_GENERIC MsgSC;
	MsgSC.SetSize( sizeof(NET_MSG_GENERIC) );
	MsgSC.nType = NET_MSG_SND_ALL_SVR_INFO_E;
	SendClient(dwClient, &MsgSC);


//#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
	//if (m_ServiceProvider == SP_OFFICE_TEST || m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_HONGKONG || m_ServiceProvider == SP_THAILAND || m_ServiceProvider == SP_KOREA || m_ServiceProvider == SP_KOREA_TEST)
	//{
		// ������ ������ ���� TracingUser ������ �����Ѵ�.
		if( m_pClientManager->IsOnline(dwClient) == true  )
		{
			GLMSG::NET_UPDATE_TRACINGCHAR_ALL msg;	
			
			for(size_t i=0; i<m_vecTracingData.size(); ++i)
			{
                if (i < MAX_TRACING_NUM)
				    msg.tracingData[i] = m_vecTracingData[i];
			}
			SendClient(dwClient, &msg);
		}
	//}
//#endif
}

// Agent, Field, Login -> Session
// Heartbeat
/*
void CSessionServer::MsgHeartBeat(MSG_LIST* pMsg)
{    
    if (pMsg == NULL) return;

    m_pClientManager->SetHeartBeat(pMsg->dwClient);
}
*/



void CSessionServer::SetSvrInfo(DWORD dwClient, G_SERVER_INFO *gsi)
{
	if (gsi == NULL) return;

	int nServerGroup         = gsi->nServerGroup;
	int nServerChannelNumber = gsi->nServerChannelNumber;
	int nPort                = gsi->nServicePort;

	if (gsi->ServerType == net::SERVER_AGENT)
	{
		//
		//mjeon
		//ServerGroup seems to mean a group of servers that consists of
		//1 AgentServer and several FieldServers.
		//And in this case, nServerNumber is used for channel number in the server group.
		//

		for (int nChannel=0; nChannel < nServerChannelNumber; ++nChannel)
		{
			::StringCchCopy(m_sServerChannel[nServerGroup][nChannel].szServerIP, MAX_IP_LENGTH+1, gsi->szServerIP);			
			m_sServerChannel[nServerGroup][nChannel].nServicePort         = nPort;
			m_sServerChannel[nServerGroup][nChannel].nServerGroup         = nServerGroup;
			m_sServerChannel[nServerGroup][nChannel].nServerNumber        = nChannel;
			m_sServerChannel[nServerGroup][nChannel].nServerCurrentClient = gsi->nServerCurrentClient;
			m_sServerChannel[nServerGroup][nChannel].nServerMaxClient     = gsi->nServerChannellMaxClient;
			m_sServerChannel[nServerGroup][nChannel].bPK                  = gsi->bPk;
            m_sServerChannel[nServerGroup][nChannel].bNewServer           = gsi->bNewServer;
		}
	}

	m_pClientManager->SetSvrInfo(dwClient, gsi);

	sc::writeLogInfo(
        sc::string::format(
            "SetSvrInfo %s %d %d %d %d",
            gsi->szServerIP,
            gsi->nServicePort,
            gsi->nServerChannellMaxClient,
            gsi->nServerCurrentClient,
            gsi->ServerType));
}


// ����->���Ǽ���
// �ش� ������ Full ����
void CSessionServer::MsgServerInfo(DWORD dwClient, MSG_LIST* pMsg)
{
    if (!pMsg)
		return;

	NET_SERVER_INFO* nsi = reinterpret_cast <NET_SERVER_INFO*> (pMsg->Buffer);

	SetSvrInfo(dwClient, &(nsi->gsi));
    
	// ���� ������ �α��� ������ �����Ѵ�.
	MsgSndCurSvrInfoToAll();
	
	if (nsi->gsi.ServerType == net::SERVER_FIELD)
	{
		for (int i=0; i < m_pClientManager->GetMaxClient(); ++i)
		{
			// �ʵ弭�� �ٿ�� �翬���� �̷�� ���� Agent ���� �ٽ� Field �� �����϶�� �˷��� �Ѵ�.
			if (m_pClientManager->IsOnline(i)   == true &&
				m_pClientManager->GetSvrType(i) == net::SERVER_AGENT &&
				m_pClientManager->GetSvrGrp(i)  == nsi->gsi.nServerGroup &&
				m_pClientManager->GetSvrNum(i)  == nsi->gsi.nServerNumber)
			{
				// Agent �� �ڽſ��� �����϶�� �˸���.
				NET_RECONNECT_FILED	nrf;
				SendClient(i, &nrf);
				sc::writeLogInfo(std::string("INFO:Inform Agent, Reconnect To Field"));
			}
		}
	}
	else if (nsi->gsi.ServerType == net::SERVER_MATCH)
	{
		for (int i=0; i < m_pClientManager->GetMaxClient(); ++i)
		{
			//��� Agent���� Match���� �������� ��û
			if (m_pClientManager->IsOnline(i) == true)
			{
				if (m_pClientManager->GetSvrType(i) == net::SERVER_AGENT || m_pClientManager->GetSvrType(i) == net::SERVER_INSTANCE)
				{
					// Agent/Instance �� �ڽſ��� �����϶�� �˸���.
					NET_RECONNECT_MATCH nrm;
					SendClient(i, &nrm);
					sc::writeLogInfo(std::string("INFO:Let Agent&Instance Reconnect To Match"));
				}
			}
		}
	}

	// Agent���� ���...
	if (nsi->gsi.ServerType == net::SERVER_AGENT)
	{
		if ( m_pClientManager->IsOnline(dwClient) == true )
		{
			MsgSndEncryptKey(dwClient);
		}
	}

//#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
	//if (m_ServiceProvider == SP_OFFICE_TEST || m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_HONGKONG || m_ServiceProvider == SP_THAILAND || m_ServiceProvider == SP_KOREA || m_ServiceProvider == SP_KOREA_TEST)
	//{
		// ������ ������ ���� TracingUser ������ �����Ѵ�.
		if( m_pClientManager->IsOnline(dwClient) == true  )
		{
			GLMSG::NET_UPDATE_TRACINGCHAR_ALL msg;	
			
			for (size_t i=0; i<m_vecTracingData.size(); ++i)
			{
                if (i < MAX_TRACING_NUM)
				    msg.tracingData[i] = m_vecTracingData[i];
			}
			SendClient(dwClient, &msg);
		}
	//}
//#endif
}

void CSessionServer::MsgEncryptKey(DWORD dwClient, MSG_LIST* pMsg)
{
	if (pMsg == NULL) return;

	NET_ENCRYPT_KEY* nmg = reinterpret_cast <NET_ENCRYPT_KEY*> (pMsg->Buffer);

	::StringCchCopy( m_szEncrypt, ENCRYPT_KEY+1, nmg->szEncryptKey );


	for (DWORD loof=0; loof < (DWORD) m_pClientManager->GetMaxClient(); ++loof)
	{
		if (m_pClientManager->IsOnline(loof) == true &&
			m_pClientManager->GetSvrType(loof) == net::SERVER_AGENT)
		{
			MsgSndEncryptKey( loof );
		}
	}
}

void CSessionServer::MsgSndEncryptKey( DWORD dwClient )
{
	NET_ENCRYPT_KEY nmg;
	::StringCchCopy(nmg.szEncryptKey, ENCRYPT_KEY+1, m_szEncrypt );
	SendClient(dwClient, &nmg);
}

// ��� ���ῡ ���Ӽ����� ���������� �����Ѵ�.
// SERVER_FIELD	3
// SERVER_AGENT	4
// LoginServer + ServerManager�� �޽����� ������.
void CSessionServer::MsgSndCurSvrInfoToAll()
{
	DWORD dwClientMax = static_cast< DWORD >( m_pClientManager->GetMaxClient() );
	for ( DWORD dwClient=0; dwClient < dwClientMax ; ++dwClient )
	{
		if ( 
			m_pClientManager->GetSocket( dwClient ) != INVALID_SOCKET &&
			m_pClientManager->GetSvrType( dwClient ) != net::SERVER_FIELD &&
			m_pClientManager->GetSvrType( dwClient ) != net::SERVER_AGENT &&
			m_pClientManager->GetSvrType( dwClient ) != net::SERVER_CACHE &&
			m_pClientManager->GetSvrType( dwClient ) != net::SERVER_MATCH &&	//mjeon.instance.match
			m_pClientManager->GetSvrType( dwClient ) != net::SERVER_INSTANCE && 
			m_pClientManager->GetSvrType( dwClient ) != net::SERVER_INTEGRATION_CACHE )
		{
			// ������������ ������ �������� ����
			if ( dwClient != m_nAuthServerSlot )
			{
				MsgSndSvrInfoReset( dwClient );
				MsgSndCurSvrInfo( dwClient, NULL );
			}
		}
	}
}

// ���Ӽ���->���Ǽ��� 
// ���� ���� ���� ����ó��
void CSessionServer::MsgServerCurInfo(DWORD dwClient, MSG_LIST* pMsg)
{
    if (pMsg == NULL) return;

	NET_SERVER_CUR_INFO* nsci = reinterpret_cast <NET_SERVER_CUR_INFO*> (pMsg->Buffer);
	m_pClientManager->SetSvrCurInfo(dwClient, &nsci->gsci);
}

// Agent->Session
// ���� ä�� ��������
void CSessionServer::MsgServerChannelInfo(DWORD dwClient, MSG_LIST* pMsg)
{
    if (pMsg == NULL) return;

	NET_SERVER_CHANNEL_INFO* pNsci = reinterpret_cast <NET_SERVER_CHANNEL_INFO*> (pMsg->Buffer);
	SetChannelInfo(pNsci);
}

void CSessionServer::SetChannelInfo(NET_SERVER_CHANNEL_INFO* pMsg)
{
	if (pMsg == NULL) return;

	int nServerGroup        = pMsg->nServerGroup;
	int nChannel            = pMsg->nChannel;	
	int nChannelCurrentUser = pMsg->nChannelCurrentUser;
	int nChannelMaxUser     = pMsg->nChannelMaxUser;
	bool bPk                = pMsg->bPk;
	
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER || nServerGroup < 0 || nServerGroup >= MAX_SERVER_GROUP)
	{
		return;
	}
	else
	{
		m_sServerChannel[nServerGroup][nChannel].nServerCurrentClient = nChannelCurrentUser;
		m_sServerChannel[nServerGroup][nChannel].nServerMaxClient     =	nChannelMaxUser;
		m_sServerChannel[nServerGroup][nChannel].bPK                  = bPk;

		if( m_dwLoginServerID != -1 )
		{
			//
			//mjeon
			//If the current user number exceeds 95% of max user number,
			//then signal server-full msg to LoginServer to prevent additioanl logging in.
			//
			if( (float)nChannelMaxUser * 0.95f < (float)nChannelCurrentUser )
			{
				if( m_bServerChannelFull[nServerGroup][nChannel] == FALSE )
				{
					m_bServerChannelFull[nServerGroup][nChannel] = TRUE;
					// LoginServer�� �޽����� ����
					NET_SERVER_CHANNEL_FULL_INFO nmg;
					nmg.nServerGroup = nServerGroup;
					nmg.nChannel	 = nChannel;
					nmg.bChannelFull = TRUE;
					nmg.nChannelCurrentUser = nChannelCurrentUser;

					SendClient(m_dwLoginServerID, &nmg);
				}
			}
            else 
			{
				if( m_bServerChannelFull[nServerGroup][nChannel] == TRUE )
				{
					m_bServerChannelFull[nServerGroup][nChannel] = FALSE;
					// LoginServer�� �޽����� ����
					NET_SERVER_CHANNEL_FULL_INFO nmg;
					nmg.nServerGroup = nServerGroup;
					nmg.nChannel	 = nChannel;
					nmg.bChannelFull = FALSE;
					nmg.nChannelCurrentUser = nChannelCurrentUser;

					SendClient(m_dwLoginServerID, &nmg);
				}			
			}
		}
	}
}

void CSessionServer::MsgReqAllSvrFullInfo(void)
{
	for (int i=0; i<m_pClientManager->GetMaxClient(); ++i)
	{
		if (m_pClientManager->IsOnline(i) == true)
		{
			MsgReqSvrFullInfo(i);
		}
	}
}

void CSessionServer::MsgReqSvrFullInfo(DWORD dwClient)
{
	NET_MSG_GENERIC		nmg;
	nmg.SetSize( sizeof(NET_MSG_GENERIC) );
	nmg.nType = NET_MSG_REQ_FULL_SVR_INFO;
	SendClient(dwClient, &nmg);
}

void CSessionServer::MsgReqAllSvrCurInfo(void)
{	
	for (int i=0; i<m_pClientManager->GetMaxClient(); ++i) 
	{
		if (m_pClientManager->IsOnline(i) == true)
		{
			MsgReqSvrCurInfo(i);
		}
	}
}

void CSessionServer::MsgReqSvrCurInfo(DWORD dwClient)
{
	NET_MSG_GENERIC		nmg;
	nmg.SetSize( sizeof(NET_MSG_GENERIC) );
	nmg.nType = NET_MSG_REQ_CUR_STATE;
	SendClient(dwClient, &nmg);
}

// ���Ӽ��� -> ���Ǽ��� : ĳ���Ͱ� ���ӿ� �������� 
void CSessionServer::MsgGameJoinOK(DWORD dwClient, MSG_LIST* pMsg)
{	
	/*
	if (pMsg == NULL) return;

	DWORD dwClient = pMsg->dwClient;
	NET_GAME_JOIN_OK* ngjo = (NET_GAME_JOIN_OK*) pMsg->Buffer;

	// sc::writeLogError(C_MSG_CONSOLE, "���Ӽ���->���Ǽ���:(%s)ĳ���Ͱ� ���ӿ� ��������", ngjo->szChaName);
	m_pClientManager->UpdateChaJoinOK(ngjo->szUserID,
									ngjo->nUsrNum,
									ngjo->nChaNum,
									ngjo->dwGaeaID,
									ngjo->szChaName);

	// DB Update	
	CUserUpdateCha* pAction = new CUserUpdateCha(ngjo->nUsrNum, ngjo->szChaName);
	m_pDbManager->AddJob((CDbAction*) pAction);
	*/
}

// Agent->Session : ĳ���� ����, ���� ����
int CSessionServer::MsgChaDecrease(MSG_LIST* pMsg) 
{
	/*
	if (pMsg == NULL) return NET_ERROR;

	NET_CHARACTER_INC_DEC* pTemp = reinterpret_cast <NET_CHARACTER_INC_DEC*> (pMsg->Buffer);
	
	CUserChaNumDecrease* pAction = new CUserChaNumDecrease(pTemp->nUserNum);
	m_pDbManager->AddJob(pAction);
	*/
	return NET_OK;
}

// Agent->Session :TEST ����  ĳ���� ����, ���� ����
int CSessionServer::MsgTestChaDecrease(MSG_LIST* pMsg)
{
	/*
	if (pMsg == NULL) return NET_ERROR;

	NET_CHARACTER_INC_DEC* pTemp = reinterpret_cast <NET_CHARACTER_INC_DEC*> (pMsg->Buffer);

	CUserTestChaNumDecrease* pAction = new CUserTestChaNumDecrease(pTemp->nUserNum);
	m_pDbManager->AddJob((CDbAction*) pAction);
	*/
	return NET_OK;
}

// DAUM:Agent->Session : ĳ���� ����, ���� ����
int CSessionServer::DaumMsgChaDecrease(MSG_LIST* pMsg) 
{
	/*
	if (pMsg == NULL) return NET_ERROR;

	NET_CHARACTER_INC_DEC* pTemp = reinterpret_cast <NET_CHARACTER_INC_DEC*> (pMsg->Buffer);
	
	CUserChaNumDecrease* pAction = new CUserChaNumDecrease(pTemp->nUserNum);
	m_pDbManager->AddJob(pAction);
	*/
	return NET_OK;
}

// DAUM:Agent->Session :TEST ����  ĳ���� ����, ���� ����
int CSessionServer::DaumMsgTestChaDecrease(MSG_LIST* pMsg)
{
	/*
	if (pMsg == NULL) return NET_ERROR;
	NET_CHARACTER_INC_DEC* pTemp = reinterpret_cast <NET_CHARACTER_INC_DEC*> (pMsg->Buffer);
	CUserTestChaNumDecrease* pAction = new CUserTestChaNumDecrease(pTemp->nUserNum);
	m_pDbManager->AddJob((CDbAction*) pAction);
	*/
	return NET_OK;
}

// Agent->Session:ĳ���� ����, ���� ����
int CSessionServer::MsgChaIncrease(MSG_LIST* pMsg)
{
	/*
	if (pMsg == NULL) return NET_ERROR;
	NET_CHARACTER_INC_DEC* pTemp = reinterpret_cast <NET_CHARACTER_INC_DEC*> (pMsg->Buffer);	
	CUserChaNumIncrease* pAction = new CUserChaNumIncrease(pTemp->nUserNum);
	m_pDbManager->AddJob((CDbAction*) pAction);
	*/
	return NET_OK;
}

// Agent->Session:TEST ���� ĳ���� ����, ���� ����
int	CSessionServer::MsgTestChaIncrease      (MSG_LIST* pMsg)
{
	/*
	if (pMsg == NULL) return NET_ERROR;
	NET_CHARACTER_INC_DEC* pTemp = reinterpret_cast <NET_CHARACTER_INC_DEC*> (pMsg->Buffer);	
	CUserTestChaNumIncrease* pAction = new CUserTestChaNumIncrease(pTemp->nUserNum);
	m_pDbManager->AddJob((CDbAction*) pAction);
	*/
	return NET_OK;
}

// DAUM:Agent->Session : ĳ���� ����, ���� ����
int CSessionServer::DaumMsgChaIncrease      (MSG_LIST* pMsg)
{
	/*
	if (pMsg == NULL) return NET_ERROR;
	NET_CHARACTER_INC_DEC* pTemp = reinterpret_cast <NET_CHARACTER_INC_DEC*> (pMsg->Buffer);	
	CUserChaNumIncrease* pAction = new CUserChaNumIncrease(pTemp->nUserNum);
	m_pDbManager->AddJob((CDbAction*) pAction);
	*/
	return NET_OK;
}

// DAUM:Agent->Session :TEST ����  ĳ���� ����, ���� ����
int CSessionServer::DaumMsgTestChaIncrease  (MSG_LIST* pMsg)
{
	/*
	if (pMsg == NULL) return NET_ERROR;
	NET_CHARACTER_INC_DEC* pTemp = reinterpret_cast <NET_CHARACTER_INC_DEC*> (pMsg->Buffer);	
	CUserTestChaNumIncrease* pAction = new CUserTestChaNumIncrease(pTemp->nUserNum);
	m_pDbManager->AddJob((CDbAction*) pAction);
	*/
	return NET_OK;
}

// Control program->Session server : command message
// NET_MSG_SVR_PAUSE	������ ��� �����.
// NET_MSG_SVR_RESUME	���߾��� ������ �ٽ� ������Ų��.	
// NET_MSG_SVR_RESTART	������ ������ ���߰� ���Ӱ� ������Ų��.
// NET_MSG_SVR_STOP		������ ������ �����.
void CSessionServer::MsgSvrCmdProcess(DWORD dwClient, MSG_LIST* pMsg)
{
    if (!pMsg)
        return;

    NET_SVR_CMD_CHANGE_STATE* pTemp = (NET_SVR_CMD_CHANGE_STATE*) pMsg->Buffer;
	int nClient = FindServer(pTemp->nSvrType, pTemp->nSvrGrp, pTemp->nSvrNum, pTemp->nSvrField);
		
	sc::writeLogInfo(sc::string::format("Server state change command %1%", nClient));

	if (nClient != NET_ERROR)
	{
		switch(pTemp->nCmdType)
		{
			/*
		case NET_MSG_SVR_PAUSE : // ���� ��� �ߴ�			
			sc::writeLogInfo(sc::string::format("Server state change PAUSE command %1%", nClient));
            MsgSndServerStateChange(nClient, NET_MSG_SVR_PAUSE);
			break;
		case NET_MSG_SVR_RESUME : // ���� ����			
			sc::writeLogInfo(sc::string::format("Server state change RESUME command %1%", nClient));
			MsgSndServerStateChange(nClient, NET_MSG_SVR_RESUME);
			break;
			*/
		case NET_MSG_SVR_RESTART : // ���� �����			
			sc::writeLogInfo(sc::string::format("Server state change RESTART command %1%", nClient));
			MsgSndServerStateChange(nClient, NET_MSG_SVR_RESTART);
			break;
		case NET_MSG_SVR_STOP : // ���� ���� �ߴ�			
			sc::writeLogInfo(sc::string::format("Server state change STOP command %1%", nClient));
			MsgSndServerStateChange(nClient, NET_MSG_SVR_STOP);
			break;
		default:
			break;
		}
	}
}

//! Login->Session : Version information
void CSessionServer::MsgVersionLS(DWORD dwClient, MSG_LIST* pMsg)
{
    NET_VERSION_LS* pPacket = (NET_VERSION_LS*) pMsg->Buffer;
    m_LauncherVersion = pPacket->m_LauncherVersion;
    m_PatchVersion    = pPacket->m_PatchVersion;
    sc::writeLogInfo(sc::string::format("Launcher Version %1%", pPacket->m_LauncherVersion));
    sc::writeLogInfo(sc::string::format("Patch Version %1%", pPacket->m_PatchVersion));
    
    SendVersionInfo();    
}

void CSessionServer::SendVersionInfo()
{
    NET_VERSION_SS MsgSS(m_LauncherVersion, m_PatchVersion);
    SendAllClient(&MsgSS);
}

void CSessionServer::MsgGmLogItemReloadAS(DWORD ClientSlot, MSG_LIST* pMsg)
{
    // GLMSG::NET_GM_LOG_ITEM_RELOAD_AS
    GLMSG::NET_GM_LOG_ITEM_RELOAD_SS Msg;
    SendAllClient(&Msg);
}

void CSessionServer::MsgGmGameDataUpdateAS(DWORD ClientSlot, MSG_LIST* pMsg)
{
    // GLMSG::NET_GM_GAME_DATA_UPDATE_AS
	 GLMSG::NET_GM_GAME_DATA_UPDATE_AS* pPacket = (GLMSG::NET_GM_GAME_DATA_UPDATE_AS*) pMsg->Buffer;
    GLMSG::NET_GM_GAME_DATA_UPDATE_SS Msg;
	Msg.nDataType = pPacket->nDataType;
    SendAllClient(&Msg);
}

void CSessionServer::MsgEventExpMS(DWORD ClientSlot, MSG_LIST* pMsg)
{
    GLMSG::NET_EVENT_EXP_MS* pPacket = (GLMSG::NET_EVENT_EXP_MS*) pMsg->Buffer;
    if (!pPacket)
        return;

    std::string Msg(
        sc::string::format(
            "MsgEventExpMS %1% %2%",
            pPacket->m_ServerGroup,
            pPacket->m_Scale));
    sc::writeLogInfo(Msg);

    DWORD AgentSlot = m_pClientManager->FindServer(net::SERVER_AGENT, pPacket->m_ServerGroup, 0, 0);
    if (AgentSlot == NET_ERROR) 
    {
        // �ش� ������ ã�� ����
        sc::writeLogError(
            sc::string::format(
                "MsgEventExpMS. Can't Find Server. %1%",
                pPacket->m_ServerGroup));
        return;
    }

    // Agent �� ���
    GLMSG::NET_EVENT_EXP_SA MsgSA(pPacket->m_Scale);
    SendClient(AgentSlot, &MsgSA);
}

void CSessionServer::MsgEventExpEndMS(DWORD ClientSlot, MSG_LIST* pMsg)
{
    GLMSG::NET_EVENT_EXP_END_MS* pPacket = (GLMSG::NET_EVENT_EXP_END_MS*) pMsg->Buffer;
    if (!pPacket)
        return;

    std::string Msg(
        sc::string::format(
            "MsgEventExpEndMS %1%",
            pPacket->m_ServerGroup));
    sc::writeLogInfo(Msg);

    DWORD AgentSlot = m_pClientManager->FindServer(net::SERVER_AGENT, pPacket->m_ServerGroup, 0, 0);
    if (AgentSlot == NET_ERROR) 
    {
        // �ش� ������ ã�� ����
        sc::writeLogError(
            sc::string::format(
                "MsgEventExpEndMS. Can't Find Server. %1%",
                pPacket->m_ServerGroup));
        return;
    }

    // Agent �� ���
    GLMSG::NET_EVENT_EXP_END_SA MsgSA;
    SendClient(AgentSlot, &MsgSA);
}

void CSessionServer::MsgSendAuthServerInfo(DWORD dwClient, MSG_LIST* pMsg)
{
    if (!pMsg)
        return;

    NET_AUTH_SERVER_CONNECTION_STATE nsi;       

	// ������ �õ����� ���� ���� ������ ����ȵ� ���
	// ó���� FALSE������ ���������� ������ �ȵǸ� ������ ������ �� �������� �������
	nsi.m_bDisconnection = m_bReconnect;

	SendClient(dwClient, &nsi);	
}

int CSessionServer::FindServer(int nSvrType, int nIndex)
{
	int nCount = 0;
	int nMaxClient = m_pClientManager->GetMaxClient();

	for (int i=0; i<nMaxClient; ++i)
	{
		if (m_pClientManager->GetSvrType(i) == nSvrType)
		{
			if (nCount == nIndex )
			{
				sc::writeLogInfo( sc::string::format("Find Server ( Type : %d) - %d", nSvrType, i) );
				return i;
			}		
			else
			{
				nCount++;
			}
		}
	}
	sc::writeLogError(std::string("Can't Find Server"));
	return NET_ERROR;
}

int CSessionServer::GetServerTypeCount(int nSvrType)
{
	int nCount = 0;
	int nMaxClient = m_pClientManager->GetMaxClient();

	for (int i=0; i<nMaxClient; ++i)
	{
		if (m_pClientManager->GetSvrType(i) == nSvrType)
		{
			nCount++;
		}
	}

	return nCount;
}