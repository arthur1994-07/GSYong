#include "../pch.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../enginelib/DxTools/DebugSet.h"

#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/Msg/GLContrlBaseMsg.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "../Club/ClubLogMan.h"
#include "../Cache/CacheUserLobby.h"
#include "../DxServerInstance.h"
#include "../Database/Ado/AdoManager.h"
#include "../Database/DBAction/DbActionShop.h"
#include "../PointShop/PointShopCache.h"
#include "../PrivateMarket/GLPrivateMarketSearchMan.h"
#include "../PrivateMarket/GLConsignmentSaleMan.h"
#include "../PostRenew/PostRenewCache.h"

#include "../Cache/CacheRandomboxChance.h"

#include "../Money/MoneyManagerCache.h"

#include "./CacheServerConfigXml.h"
#include "./NetCacheClientMan.h"
#include "../../RanLogic/Util/GlobalAuthManager.h"
#include "./CacheServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CacheServer::CacheServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo, SUBPATH* pPath, EMSERVICE_PROVIDER ServiceProvider)
    : NetServer(hWnd, hEditBox, hEditBoxInfo, pPath, ServiceProvider)
    , m_pClientManager(NULL)
    , m_pDbManager(NULL)
    , m_bHeartBeat(true)
    , m_SessionSlot(NET_INVALID_SLOT)
	, m_pMoneyMan ( NULL )
{
	MessageEnumMixer::GetInstance();

	sc::LogMan::getInstance()->attachEditControl(hEditBox, hEditBoxInfo);
    InitPacketFunc();
    m_HeartbeatTimer = ::timeGetTime();
    m_SessionCheckTimer = ::timeGetTime();
    m_pConfig = new CacheServerConfigXml;
    m_pPointShop = new pointshop::PointShopCache;
    m_pUserLobbyMan = new CacheUserLobbyMan;
    m_pClubLog = new club::LogManServer;
	m_pPrivateMarketSearch = new private_market::GLPrivateMarketSearchMan;
    m_pConsignmentSale = new private_market::GLConsignmentSaleMan(this);
	m_pAuthManager = new sc::CGlobalAuthClientLogic;

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.14, 신규등록
     * modified : 
     */
    m_pRandomboxMan = new CacheRandomboxChance;


	if( GLUseFeatures::GetInstance().IsUsingPostRenew() )
		m_spPostCache = spPostRenewCache( new PostRenewCache(this) );

	m_pMoneyMan = new Money::CMoneyManagerCache( this, 86400.f );
}

CacheServer::~CacheServer()
{
    if (IsRunning())
        Stop();
    SAFE_DELETE(m_pClubLog)
    SAFE_DELETE(m_pUserLobbyMan);
    SAFE_DELETE(m_pPointShop);
    SAFE_DELETE(m_pConfig);
	SAFE_DELETE( m_pPrivateMarketSearch );
    SAFE_DELETE( m_pConsignmentSale );
    GLogicData::GetInstance().ClearData();
	SAFE_DELETE(m_pAuthManager);

	SAFE_DELETE(m_pMoneyMan);
}

int	CacheServer::Stop()
{
	// Stop All Thread and exit
	DWORD dwExitCode = 0;

	sc::writeLogInfo(std::string("Please wait until server stops"));
	
	m_bIsRunning = false;

	if ( m_pMoneyMan )
		m_pMoneyMan->OnStop();
	
	::SetEvent(m_hUpdateQuitEvent);
	::WaitForSingleObject(m_hUpdateThread, INFINITE);
	SAFE_CLOSE_HANDLE(m_hUpdateQuitEvent);

	sc::writeLogInfo(std::string("== Update Thread ends"));

	StopListenThread();
	StopWorkThread();
	StopIOCP();
	
	SessionCloseConnect();

	SAFE_DELETE(m_pClientManager);
	sc::writeLogInfo(std::string("== Client Manager closed"));
	
	SAFE_DELETE(m_pRecvMsgManager);
	sc::writeLogInfo(std::string("== Message Manager closed."));

	SAFE_DELETE(m_pDbManager);
	sc::writeLogInfo(std::string("== Database Manager closed OK"));

	SAFE_DELETE(m_pConfig);
	sc::writeLogInfo(std::string("== Config Manager closed OK"));

    // Logic data clear
    GLogicData::GetInstance().ClearData();

	CDebugSet::FinalCleanup();

	sc::writeLogInfo(std::string("Server Stop OK"));
	sc::writeLogInfo(std::string("======================================================="));
	
	m_nStatus = S_SERVER_STOP;

	return NET_OK;
	/*
    m_bIsRunning = false;

    ::SetEvent(m_hUpdateQuitEvent);	
    ::WaitForSingleObject(m_hUpdateThread, INFINITE);
    SAFE_CLOSE_HANDLE(m_hUpdateQuitEvent);
        
    SAFE_DELETE(m_pClientManager);

	// Message Queue 종료
	SAFE_DELETE(m_pRecvMsgManager);
	
	SAFE_DELETE(m_pDbManager)

    SAFE_DELETE(m_pConfig);

    // Logic data clear
    GLogicData::GetInstance().ClearData();

    sc::writeLogInfo(std::string("-------------------------------------------------------------"));
    sc::writeLogInfo(std::string("Server stopped"));
    sc::writeLogInfo(std::string("-------------------------------------------------------------"));
    return NET_OK;
	*/
}

void CacheServer::ForceStop( DWORD Reason )
{
	sc::writeLogInfo( std::string( "== Please wait until server stop(Force)" ) );

	if ( m_pMoneyMan )
		m_pMoneyMan->OnStop();
}

int	CacheServer::Start()
{
    m_bIsRunning = true;
    if (!ReadConfig())
    {
        m_bIsRunning = false;
        return NET_ERROR;
    }

	sc::writeLogInfo(std::string(">Start Configuring the Server..."));
    
    if (StartMsgManager(m_nMaxClient) == NET_ERROR)
    {
        m_bIsRunning = false;
        return NET_ERROR;
    }

    if (StartIOCP() == NET_ERROR)
    {
        m_bIsRunning = false;
        return NET_ERROR;
    }

    if (StartClientManager() == NET_ERROR)
    {
        m_bIsRunning = false;
        return NET_ERROR;
    }

    if (!StartDbManager())
    {
        m_bIsRunning = false;
        return NET_ERROR;
    }

    if (StartWorkThread(m_pConfig) == NET_ERROR)
    {
        m_bIsRunning = false;
        return NET_ERROR;
    }

    // Logic Load -------------------------------------------------------------
    std::string AppPath(sc::getAppPath());
    HRESULT hr = DxServerInstance::CreateBefore(
        AppPath.c_str(),
        m_pPath,
        m_pConfig->GetLangSet(),
        m_ServiceProvider,
        m_pConfig->IsPKServer(),
        m_pConfig->IsPKLessServer(),
        false);
    if (FAILED(hr))
    {
        sc::writeLogFatal(std::string("DxServerInstance::CreateBefore"));
        Stop();
        return NET_ERROR;
    }
    DxServerInstance::CreateAfter();
    // ------------------------------------------------------------------------

	// 포인트샵, 개인상점검색시스템 설정 load
	PointShopDirectLoad();
	PrivateMarketDirectLoad();

	if ( StartUpdateThread() == NET_ERROR )
	{
		m_bIsRunning = false;
		return NET_ERROR;
	}

	if ( StartListenThread() == NET_ERROR )
	{
		m_bIsRunning = false;
		return NET_ERROR;	
	}

	SessionConnect();

//#ifdef _USE_AUTH
//	// 세션서버로 해당 서버의 정보를 보낸 다음에 인증처리를 진행해야함.
//	if ( SendAuthInfoToSessionServer() == NET_ERROR )
//	{
//		m_bIsRunning = false;
//		return NET_ERROR;
//	}
//#endif

    m_HeartbeatTimer = ::timeGetTime();
    m_SessionCheckTimer = ::timeGetTime();
    m_bHeartBeat = true;
    
	sc::writeLogInfo(
		sc::string::format(
		"FD_SETSIZE %1%",
		FD_SETSIZE));

    sc::writeLogInfo(std::string(">Server Start OK"));
	sc::writeLogInfo(std::string("======================================================="));

    return NET_OK;
}

//! Close Client Connection
void CacheServer::CloseClient( DWORD dwClient )
{
    if ( dwClient == NET_INVALID_SLOT )
        return;

	int nResult = m_pClientManager->CloseClient( dwClient );

    if ( dwClient == m_SessionSlot && m_bIsRunning )
    {
        // 서버가 가동중인데 연결이 끊어졌다.
        // SessionServerConnectionCheck 함수에서 재연결을 시도할 것이다.
        sc::writeLogError( std::string( "Session connection was closed. will try to reconnect" ) );
        m_SessionSlot = NET_INVALID_SLOT;
    }

	// 필드서버가 다운되었을 경우 처리한다.
	if ( m_pPrivateMarketSearch )
	{
		m_pPrivateMarketSearch->DeleteSeller( dwClient );
	}
}

int CacheServer::SendClient(DWORD ClientSlot, EMNET_MSG Type, const msgpack::sbuffer& SendBuffer)
{
    NET_MSG_PACK SendData;
    if (SendData.SetData(Type, SendBuffer))
        return SendClient(ClientSlot, &SendData);
    else
        return NET_ERROR;
}

int CacheServer::SendClient(DWORD ClientSlot, NET_MSG_GENERIC* pMsg)
{
    if (m_pClientManager->IsOnline(ClientSlot))
        return m_pClientManager->SendClient(ClientSlot, pMsg);
    else
        return NET_ERROR;
}

void CacheServer::SendAllClient(EMNET_MSG Type, const msgpack::sbuffer& SendBuffer)
{
    NET_MSG_PACK SendData;
    if (SendData.SetData(Type, SendBuffer))
        SendAllClient(&SendData);
}

void CacheServer::SendAllClient(NET_MSG_GENERIC* pMsg)
{    
    for (int i=0; i<m_nMaxClient; ++i)
        SendClient(i, pMsg);
}

void CacheServer::SendAllClientWithoutSession(NET_MSG_GENERIC* pMsg)
{    
    for (int i=0; i<m_nMaxClient; ++i)
	{
		if ( i != m_SessionSlot )
			SendClient(i, pMsg);
	}
}

void CacheServer::SendToSession( NET_MSG_GENERIC* pMsg )
{
	SendClient(m_SessionSlot, pMsg);
}

void CacheServer::MsgPmarketRegitemFH( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNETPC_PMARKET_REGITEM_FH *pNetNsg = (GLMSG::SNETPC_PMARKET_REGITEM_FH *)pMsg;
	DWORD dwnum = pNetNsg->dwCharDbNum;
	SSALEITEM sItemdata = pNetNsg->sItemdata;

	PRIVAIETEMDATA sPrivaeItemData;
	sPrivaeItemData.dwCharDbNum = dwnum;
	sPrivaeItemData.SetName(pNetNsg->Itemname);
	sPrivaeItemData.sItemdata = sItemdata;

	m_vecPrivateItemData.push_back(sPrivaeItemData);
}

void CacheServer::MsgPmarketSearchReqAH( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNETPC_PMARKET_SEARCH_REQ_CAH *pNetNsg = (GLMSG::SNETPC_PMARKET_SEARCH_REQ_CAH *)pMsg;
	GLMSG::SNETPC_PMARKET_SEARCH_RESULT_HAC pnmg;
	pnmg.dwcharDbnum = pNetNsg->dwcharDbnum;
	pnmg.dwGaeaID = pNetNsg->dwGaeaID;
	SendClient(dwClient, &pnmg);
}
