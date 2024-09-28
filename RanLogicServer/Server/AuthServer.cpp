#include "pch.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "../../EngineLib/Common/profile.h"
#include "../../RanLogic/Msg/TraceMsg.h"
#include "../../RanLogic/Msg/ServerMsg.h"
#include "../Client/NetAuthClientMan.h"
#include "../../RanLogic/Util/GlobalAuthManager.h"
#include "../Database/DbAction/DbActionUser.h"
#include "../Database/ODBC/s_COdbcManager.h"
#include "../Database/ADO/ADOManager.h"
#include "./AuthServerConfigXml.h"
#include "./AuthServer.h"
#include "../../SigmaCore/DebugInclude.h"

CAuthServer::CAuthServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo, SUBPATH* pPath, EMSERVICE_PROVIDER ServiceProvider)
	: NetServer(hWnd, hEditBox, hEditBoxInfo, pPath, ServiceProvider) , m_pClientManager(NULL)
{
	sc::LogMan::getInstance()->attachEditControl(hEditBox, hEditBoxInfo);
	initPacketFunc();

	m_pAuthManager = new sc::CGlobalAuthServerLogic;
    m_pConfig = new AuthServerConfigXml;
}

CAuthServer::~CAuthServer()
{
	SAFE_DELETE(m_pClientManager);
	SAFE_DELETE(m_pAuthManager); 
    SAFE_DELETE(m_pConfig);
}

// 클라이언트 정보저장 클래스 생성
int	CAuthServer::StartClientManager()
{	
	SAFE_DELETE(m_pClientManager);	

	m_pClientManager = new NetAuthClientMan(m_nMaxClient, m_hIOServer, m_pConfig->GetReferenceCountMax());

	if ( !m_pClientManager ) 
	{
		sc::writeLogError(std::string("StartClientManager m_pClientManager NULL"));
		m_bIsRunning = false;
		return NET_ERROR;
	}

	return NET_OK;
}

int	CAuthServer::StartDbManager()
{
	int	nRetCode = 0;

	SAFE_DELETE(m_pDbManager);	

/*		
    m_pDbManager = new COdbcManager(this, db::AUTH_SERVER, m_ServiceProvider, m_pConfig->UseCustomDbPool());

	if ( !m_pDbManager )
	{
		sc::writeLogError(std::string("DB:Couldn't get an OdbcManager Instance."));
		return NET_ERROR;
	}

	nRetCode = m_pDbManager->OdbcOpenUserDB(m_pConfig->GetUserOdbcName(), m_pConfig->GetUserOdbcUser(), m_pConfig->GetUserOdbcPass(), m_pConfig->GetUserOdbcDatabase(), m_pConfig->UseCustomDbPool()); 

	if (nRetCode == sc::db::DB_ERROR)
	{
        sc::writeLogError( sc::string::format("ODBC:User DB Open Error. %1%, %2%, %3%, %4%, %5%", m_pConfig->GetUserOdbcName(), m_pConfig->GetUserOdbcUser(), m_pConfig->GetUserOdbcPass(), m_pConfig->GetUserOdbcDatabase(), m_pConfig->UseCustomDbPool() ) );
		m_bIsRunning = false;
		return NET_ERROR;
	}
	else
	{
		sc::writeLogInfo(std::string("ODBC:User DB Open OK"));
	}

	nRetCode = m_pDbManager->OdbcOpenLogDB(m_pConfig->GetLogOdbcName(), m_pConfig->GetLogOdbcUser(), m_pConfig->GetLogOdbcPass(), m_pConfig->GetLogOdbcDatabase(), m_pConfig->UseCustomDbPool());

	if (nRetCode == sc::db::DB_ERROR)
	{
        sc::writeLogError( sc::string::format("ODBC:Log DB Open Error. %1%, %2%, %3%, %4%, %5%", m_pConfig->GetUserOdbcName(), m_pConfig->GetUserOdbcUser(), m_pConfig->GetUserOdbcPass(), m_pConfig->GetUserOdbcDatabase(), m_pConfig->UseCustomDbPool() ) );
		m_bIsRunning = false;
		return NET_ERROR;
	}
	else
	{
		sc::writeLogInfo(std::string("ODBC:Log DB Open OK"));
	}*/

	SAFE_DELETE(m_pAdoManager);

	m_pAdoManager = new db::AdoManager(this, db::AUTH_SERVER, m_ServiceProvider);

	m_pAdoManager->SetConnectionString( db::AUTH_DB, m_pConfig->AuthDbSource(), m_pConfig->AuthDbDatabaseName(), m_pConfig->AuthDbUser(), m_pConfig->AuthDbPassword(), m_pConfig->GetAdoMaxPoolSize(), m_pConfig->GetDatabaseTcpPacketSize() ); 
	
	return NET_OK;
}

int CAuthServer::Start()
{	
	m_bIsRunning = true;

    const std::string XmlFileName("AuthServer.xml");    

    if ( !m_pConfig->Load(XmlFileName) )
    {
        sc::writeLogError(sc::string::format("%1% file load error", XmlFileName));
        m_bIsRunning = false;
        return NET_ERROR;
    }

    if ( StartCfg(m_pConfig) == NET_ERROR )
        return NET_ERROR;

	if ( StartMsgManager(m_nMaxClient) == NET_ERROR )
        return NET_ERROR;

	if ( StartIOCP() == NET_ERROR )
        return NET_ERROR;

	if ( StartClientManager() == NET_ERROR )
        return NET_ERROR;
	
	if ( StartDbManager() == NET_ERROR )
        return NET_ERROR;	

	if ( StartWorkThread(m_pConfig) == NET_ERROR )
        return NET_ERROR;

	if ( StartUpdateThread() == NET_ERROR )
        return NET_ERROR;

	if ( StartListenThread() == NET_ERROR )
        return NET_ERROR;

	sc::writeLogInfo(sc::string::format("FD_SETSIZE %1%", FD_SETSIZE));
	sc::writeLogInfo(std::string(">Server Start OK"));
	sc::writeLogInfo(std::string("======================================================="));

	m_nStatus = S_SERVER_RUNING;

	return NET_OK;
}

int CAuthServer::Stop()
{
	// Stop All Thread and exit
	sc::writeLogInfo(std::string("Please wait until server stop "));

	DWORD dwExitCode = 0;	
	
	lockOn();	
	m_bIsRunning = false;
	lockOff();
	
	if (m_bUseEventThread)	
	{
		::SetEvent(m_hUpdateQuitEvent);
		::WaitForSingleObject(m_hUpdateThread, INFINITE);
		SAFE_CLOSE_HANDLE(m_hUpdateQuitEvent);
	}
    else
    {
		Sleep(1000);
	}

	StopListenThread();	
	StopWorkThread();
	StopIOCP();

	CloseAllClient(); 	// close all client connections

	// 클라이언트 저장 클래스 종료
	SAFE_DELETE(m_pClientManager);
	// Message Queue 종료	
	SAFE_DELETE(m_pRecvMsgManager);
	sc::writeLogInfo(std::string("Stop Message Queue"));
	
    // DB 연결종료	
    SAFE_DELETE(m_pDbManager);
	
	sc::writeLogInfo(std::string("Stop DataBase"));
	// CFG class 종료
	//m_pConfig->ReleaseInstance();
	sc::writeLogInfo(std::string("Stop CFG"));
	// Put message to console
	sc::writeLogInfo(std::string("Server Stop OK"));
	sc::writeLogInfo(std::string("======================================================="));
	
	m_nStatus = S_SERVER_STOP;

	return NET_OK;
}

void CAuthServer::ForceStop( DWORD Reason )
{
	sc::writeLogInfo( std::string( "== Please wait until server stop(Force)" ) );
}

void CAuthServer::CloseAllClient()
{
	for ( DWORD dwNum=0; dwNum<(DWORD) m_nMaxClient; ++dwNum )
	{
		CloseClient(dwNum);
	}
}

int CAuthServer::InsertMsg(int nClient, void* pMsg)
{
    NET_MSG_GENERIC* pPacket = (NET_MSG_GENERIC*) pMsg;

    m_pRecvMsgManager->MsgQueueInsert(nClient, pPacket, pPacket->Size());

    return NET_OK;
}

// 클라이언트의 연결을 해제한다.
void CAuthServer::CloseClient(DWORD dwClient)
{
    NET_CLOSE_CLIENT_AUTH InnerMsg(dwClient);

    InsertMsg(dwClient, &InnerMsg);
}

void CAuthServer::Send(DWORD dwClient, PER_IO_OPERATION_DATA* PerIoData, DWORD dwSize)
{
}

int CAuthServer::SendClient( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
    if ( pMsg )
	{
		PROFILE_MSG_COUNT_SEND( pMsg );
		return m_pClientManager->SendClient2( dwClient, pMsg );
	}
    else
    {
        return NET_ERROR;
    }
}

int CAuthServer::SendClient( DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc/*=false*/ )
{
	NET_MSG_PACK MsgPack;
	if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
		return SendClient(dwClient, &MsgPack);
	else
		return NET_ERROR;
}

void CAuthServer::SendAllClient(NET_MSG_GENERIC* pMsg)
{
    if ( pMsg )
    {        
        int MaxClient = m_pClientManager->GetMaxClient();

        for ( int i=0; i<MaxClient; ++i )
        {        
            if ( m_pClientManager->IsOnline(i) )
			{
                SendClient(i, pMsg);
			}
        }
    }
}

void CAuthServer::SetSvrInfo(DWORD dwClient, G_AUTH_INFO *gsi)
{
	if ( gsi == NULL ) 
		return;

	m_pClientManager->SetSvrInfo(dwClient, gsi);

	sc::writeLogInfo(sc::string::format("SetSvrInfo IP[%s] Port[%d] Country[%d] Key[%s] SvrType[%d]]", gsi->szServerIP, gsi->nServicePort, gsi->nCounrtyType, gsi->szAuthData, gsi->ServerType));
}

int CAuthServer::FindServer(CString strIP, int nPort)
{
	int nMaxClient = m_pClientManager->GetMaxClient();

	for ( int i=0; i<nMaxClient; ++i )
	{		
		if ( (m_pClientManager->IsOnline((DWORD) i) == true) && (m_pClientManager->GetClientIP((DWORD)i) == strIP.GetString()) && (nPort == m_pClientManager->GetSvrServicePort((DWORD)i)) ) 
		{					
			return i;
		}
	}
	return NET_ERROR;
}

