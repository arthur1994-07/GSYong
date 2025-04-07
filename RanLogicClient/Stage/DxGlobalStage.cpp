#include "pch.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Net/NetUtil.h"
#include "../../SigmaCore/Math/Random.h"
#include "../../SigmaCore/File/StringFile.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Loading/NSLoadingDirect.h"
#include "../../SigmaCore/Compress/Unzipper.h"

#include "../../EngineLib/Common/DXInputString.h"
#include "../../EngineLib/Common/SUBPATH.h"
#include "../../EngineLib/Common/profile.h"
#include "../../EngineLib/GUInterface/Cursor.h"
#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../enginelib/GUInterface/UIConfigMgr.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/interfacecfg.h"
#include "../../EngineLib/DxTools/DxInputDevice.h"
#include "../../EngineLib/DxTools/DxFontMan.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/DxTools/DxBackUpRendTarget.h"
#include "../../EngineLib/DxTools/DxWeatherMan.h"
#include "../../EngineLib/DxTools/DxSurfaceTex.h"
#include "../../EngineLib/DxTools/RENDERPARAM.h"
#include "../../EngineLib/DxTools/DebugSet.h"
#include "../../EngineLib/DxTools/d3dfont.h"
#include "../../EngineLib/DxMeshs/FrameMesh/DxReplaceContainer.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkeletonManager.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinAnimationManager.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinMeshManager.h"
#include "../../EngineLib/DxEffect/Frame/DxEffectMan.h"
#include "../../EngineLib/DxResponseMan.h"
#include "../../EngineLib/LoadingTimeCheck.h"

#include "../../RanLogic/Crow/GLMobSchedule.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/Quest/GLQuestMan.h"
#include "../../RanLogic/Transport/GLBusStation.h"
#include "../../RanLogic/Transport/GLTaxiStation.h"
//#include "../../RanLogic/Item/GLItemMixMan.h"
#include "../../RanLogic/Product/GLProductRecipeMan.h"
#include "../../RanLogic/NpcTalk/NpcDialogueSet.h"
#include "../../RanLogic/GLStringTable.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Activity/ActivityBase.h"
#include "../../RanLogic/Msg/ApexMsg.h"
#include "../../RanLogic/Msg/GLContrlCountry.h"
#include "../../RanLogic/Msg/GLContrlMsg.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/GLLevelFile.h"
#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogicClient/GroupChat/GroupChatClient.h"
#include "../../RanLogicClient/RnAttendance/RnAttendanceClient.h"
#include "../../RanLogicClient/Country/GLCountryManClient.h"
#include "../../RanLogicClient/InstanceSystem/GLCTFlagEffectMan.h"
#include "../../RanLogicClient/LottoSystem/GLLottoSystemManClient.h"

#include "../../MfcExLib/RanFilter.h"

#include "../../RanUI/OuterInterface.h"

#include "../Attendance/GLAttendanceClient.h"
#include "../LoadingThread/LoadingThread.h"
#include "../LoadingThread/LoadingDirectClient.h"
#include "../Party/GLPartyClient.h"
#include "../Friend/GLFriendClient.h"
#include "../CapturedTheField/GLCaptureTheFieldClient.h"
#include "../Init/NSInitClientLua.h"
#include "../Tool/DxParamSet.h"
#include "../Emoticon/Emoticon.h"
#include "../TexasHoldem/GLTexasHoldemClient.h"
#include "../GLGaeaClient.h"
#include "./DxGlobalStage.h"

#include "../../RanGFxUI/GFxInterface.h"
#include "../Widget/GLWidgetScript.h"
#include "../Widget/GLWidgetTexture.h"
#include "../Widget/GLWidgetSaveProp.h"
#include "../Login/GLLoginClient.h"
#include "../RanMobile/GLRanMobileClient.h"

//#if defined( CH_PARAM ) || defined ( HK_PARAM ) || defined ( TW_PARAM ) //|| defined(_RELEASED) // Apex 적용
#include "../Apex/ClientApex.h"
//#endif

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//#ifdef TEST_INTRO_VIDEO
#include "../../DirectShowLib/DShow/NSDShow.h"
#define MSG_DSHOW_PLAYER 0xAF0AF0A
//#endif

/*
namespace
{
	struct CEMULMSG
	{
		float	fDelay;

		DWORD	dwSize;
		PBYTE	pBuffer;

		CEMULMSG () :
			pBuffer(NULL),
			
			dwSize(0),
			fDelay(0.0f)
		{
		}

		CEMULMSG ( float _fdelay, DWORD _dwsize, PBYTE _pbuffer )   
		{
			fDelay = _fdelay;
			dwSize = _dwsize;
			if ( _dwsize==0 )	return;

			pBuffer = new BYTE [ _dwsize ];
			memcpy ( pBuffer, _pbuffer, _dwsize );
		}

		~CEMULMSG()
		{
			SAFE_DELETE_ARRAY(pBuffer);
		}

		CEMULMSG& operator= ( const CEMULMSG &cMsg )
		{
			SAFE_DELETE_ARRAY(pBuffer);
			
			fDelay = cMsg.fDelay;
			dwSize = cMsg.dwSize;
			if ( cMsg.dwSize==0 )	return *this;

			pBuffer = new BYTE [ cMsg.dwSize ];
			memcpy ( pBuffer, cMsg.pBuffer, cMsg.dwSize );

			return *this;
		}
	};

	std::deque<CEMULMSG*>	g_vecMsg;

	void _insert_emul_msg ( NET_MSG_GENERIC *nmg )
	{
		CEMULMSG* pMsg = new CEMULMSG;
		pMsg->fDelay = 0.1f;
		pMsg->dwSize = nmg->dwSize;
		pMsg->pBuffer = new BYTE [ pMsg->dwSize ];
		memcpy ( pMsg->pBuffer, nmg, pMsg->dwSize );

		g_vecMsg.push_back ( pMsg );
	}

	void _clear_emul_msg ()
	{
		std::for_each ( g_vecMsg.begin(), g_vecMsg.end(), std_afunc::DeleteObject() );
		g_vecMsg.clear();
	}

	void _update_emul_msg ( float fElapsTime )
	{
		while(1)
		{
			if ( g_vecMsg.empty() )		return;

			CEMULMSG &sMsg = *(*g_vecMsg.begin());
			
			sMsg.fDelay -= fElapsTime;
			if ( sMsg.fDelay > 0.0f )	return;
    
//			GLGaeaServer::GetInstance().MsgProcess ( (NET_MSG_GENERIC*)sMsg.pBuffer, GETMYCLIENTID(), GETMYGAEAID() );

			delete (*g_vecMsg.begin());
			g_vecMsg.pop_front();
		}
	}
};
*/



void DxGlobalStage::SetEmulMsgField(EmulBase* pEmulMsg)
{
	m_pEmulMsgField = pEmulMsg;
}

void DxGlobalStage::NetSend(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, Buffer, UseCrc))
        NetSend(&MsgPack);
}

void DxGlobalStage::NetSend(NET_MSG_GENERIC* nmg)
{
	if (!m_bEmulate )
	{
        m_NetClient.Send((char*) nmg, nmg->Size());
	}
	else
        m_pEmulMsgField->_insert_emul_msg(nmg);
}

void DxGlobalStage::NetSendToField(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, Buffer, UseCrc))
        NetSendToField(&MsgPack);
}

void DxGlobalStage::NetSendToField(NET_MSG_GENERIC* nmg)
{
	if (!m_bEmulate)
        m_NetClient.SendToField((char*) nmg, nmg->Size());
	else
        m_pEmulMsgField->_insert_emul_msg(nmg);
}
/*
void DxGlobalStage::NetSendEventLottery(const char* szLotteryName)
{
	if (!m_bEmulate)
		m_NetClient.SndEventLottery(szLotteryName);
}
*/
CRITICAL_SECTION DxGlobalStage::m_CSMsgProcLock;

DxGlobalStage::DxGlobalStage(EngineDeviceMan* pEngineDevice, SUBPATH* pPath, bool bToolMode, bool bLoginProcessLog)
    : m_pEngineDevice(pEngineDevice)
    , m_pPath(pPath)
    , m_hWnd(NULL)
    , m_dwState(NULL)
    , m_bEmulate(FALSE)
    , m_dwBLOCK_PROG_COUNT(0)
    , m_dwBLOCK_PROG_IMAGE(0)
    , m_bBLOCK_PROG_DETECTED(false)
    , m_bDETECTED_REPORT(false)
    , m_fBLOCK_PROG_TIMER(0.0f)
    , m_emThisStage(EM_STAGE_NULL)
    , m_emChangeStage(EM_STAGE_NULL)
    , m_pD3DApp(NULL)
    , m_pd3dDevice(NULL)
    , m_pActiveStage(NULL)
    , m_pMsgActive(NULL)
    , m_pMsgReceive(NULL)
    , m_nChannel(-1) // 서버의 채널은 0부터 시작한다.
    , m_pEmulMsgField(NULL)
	, m_pGfxInterface( NULL )
	, m_bLoginProcessLog(bLoginProcessLog)
{
	InitializeCriticalSection(&m_CSMsgProcLock);

    m_ServiceProvider = m_pEngineDevice->GetServiceProvider();

	m_MsgBufferA.reserve ( 1000 );
	m_MsgBufferB.reserve ( 1000 );

	m_pMsgActive = &m_MsgBufferA;
	m_pMsgReceive = &m_MsgBufferB;

    m_pGaeaClient = new GLGaeaClient(this, m_pEngineDevice, m_ServiceProvider, m_pPath, bToolMode);
    m_pLobyStage = new DxLobyStage(this, m_pGaeaClient);
    m_pGameStage = new DxGameStage(this, m_pGaeaClient, m_bLoginProcessLog);
	m_pWBStage = new DxWBStage(this, m_pGaeaClient);

    initPacketFunc();

	m_dwNProtect30ServerNumber = 0;
	InitializeCriticalSection( &m_CS );
}

DxGlobalStage::~DxGlobalStage(void)
{
	DeleteCriticalSection(&m_CSMsgProcLock);


/*
    //  이미 삭제된 상황 Point, 내부에서 지우므로 이 코드는 삭제하자
    if ( m_bEmulate && m_pEmulMsgField )
	{
	    m_pEmulMsgField->_clear_emul_msg();
    }
*/

	{

		EnterCriticalSection(&m_CS);

		int nSize = (int) m_GGAuthBuffer.size();
		for( int i=0; i<nSize; ++i)
		{
			P_AUTH_DATA pData = m_GGAuthBuffer.front();
			SAFE_DELETE( pData );
			m_GGAuthBuffer.pop();
		}

		LeaveCriticalSection(&m_CS);
	}

    SAFE_DELETE(m_pLobyStage);
    SAFE_DELETE(m_pGameStage);
    SAFE_DELETE(m_pGaeaClient);
	SAFE_DELETE(m_pWBStage);
	DeleteCriticalSection( &m_CS );
}

// Use Features 동기화 후 새로운 초기화;
// Use Features 와 상관있는 경우 이용;
void DxGlobalStage::OnInitialize()
{
	// SelectCountry가 없으면 SelectCountry 를 끈다;
	//!! SF_TODO
	/*if ( m_pOutInterface )
	{
		if ( false == GLUseFeatures::GetInstance().IsUsingSelectCountry() )
		{
			m_pOutInterface->ToggleUISelectCountry( false );
		}
	}*/

	// Load Map List
	LoadMapList();
}

void DxGlobalStage::UpdateJoinWorldBattle(bool bJoinWorldBattle)
{
	//m_pOutInterface->ToggleUIJoinWorldBattle( bJoinWorldBattle );
	m_pGaeaClient->ToggleJoinWorldBattleButton(bJoinWorldBattle);
}

// Login 후 새로운 초기화;
// Use Features 와 상관없는 경우 이용;
void DxGlobalStage::OnInitLogin()
{
	m_pLobyStage->OnInitLogin ();
	m_pGameStage->OnInitLogin ();

	m_pGaeaClient->GetLottoSystemMan()->Initialize();
	m_pGaeaClient->GetCTFlagEffectMan()->OnReset();
	m_pGaeaClient->GetCharacter()->ResetData();
    m_pGaeaClient->GetAttendance()->ResetData();
	m_pGaeaClient->GetMyPartyClient()->ResetParty();
    m_pGaeaClient->GetCaptureTheField()->Clear();
	m_pGaeaClient->GetChatGroup()->ChatRoomClear();
	m_pGaeaClient->GetRnAttendanceClient()->ResetData();
	m_pGaeaClient->GetTexasHoldemManager()->ResetData();

//#if defined ( CH_PARAM ) || defined ( HK_PARAM ) || defined ( TW_PARAM ) //|| defined(_RELEASED) // Apex 적용
    int nReturn = 0;
	// APEX 버젼 변경
	if (m_ServiceProvider == SP_CHINA || m_ServiceProvider == SP_HONGKONG || m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_THAILAND)
		    nReturn = APEX_CLIENT::StartApexClient(this);
//#endif

//#if defined ( HK_PARAM ) || defined ( TW_PARAM ) // Apex 적용( 홍콩 )
	// APEX 버젼 변경
    if (m_ServiceProvider == SP_HONGKONG || m_ServiceProvider == SP_TAIWAN ||  m_ServiceProvider == SP_THAILAND)
    {
        NET_APEX_RETURN nmg;
	    nmg.nReturn = nReturn;
	    NetSend( (NET_MSG_GENERIC*) &nmg );
    }
//#endif
}

HRESULT DxGlobalStage::ReSizeWindow ( WORD wWidth, WORD wHeight )
{
	HRESULT hr;
	if (m_pActiveStage)
	{
		hr = m_pActiveStage->ReSizeWindow ( wWidth, wHeight );
		if (FAILED(hr))
            return E_FAIL;
	}
	return S_OK;
}

void DxGlobalStage::SetProgDetected ( DWORD dwID )
{
	if ( dwID!=UINT_MAX )
	{
		m_dwBLOCK_PROG_IMAGE = dwID;
		m_bBLOCK_PROG_DETECTED = true;
	}
}

#ifdef LOADING_TIME_CHECK
#include <boost/timer.hpp>
/*
                                 2011-08-16
    -------------------------------------------------------------------
    DxGlobalStage Path           2.353  2.353 sec 
    DxGlobalStage GLogicData     7.143  9.534 sec
    DxGlobalStage PointShop      0.15   9.684 sec
    DxGlobalStage Attendance     0.031  9.715 sec
    DxGlobalStage Maplist        0.006  9.721 sec
    DxGlobalStage Emoticon       0.006  9.727 sec
    DxGlobalStage Interface Text 3.607 13.334 sec
    DxGlobalStage RanFilter      3.808 17.142 sec
    DxGlobalStage ActiveStage    0     17.182 sec
*/
#endif

bool DxGlobalStage::LoadMapList ()
{
	char szFullPath[ MAX_PATH ] = { 0 };
	StringCchCopy( szFullPath, MAX_PATH, m_szFullPath );
	StringCchCat( szFullPath, MAX_PATH, m_pPath->MapFile().c_str() );
	GLMapAxisInfo::SetPath( szFullPath );
	if ( FAILED( m_pGaeaClient->LoadMapList() ) )
	{
		GASSERT( 0 && "[ERROR] : LoadMapList." );
		return false;
	}

	return true;
}

HRESULT DxGlobalStage::OneTimeSceneInitPrev( const char* szAppPath, HWND hWnd, BOOL bEmulate, LPDIRECT3DDEVICEQ pd3dDevice )
{
    StringCchCopy(m_szFullPath,MAX_PATH,szAppPath);
    m_hWnd = hWnd;
    m_bEmulate = bEmulate;

    if ( !bEmulate )
	{
		// GLLandMan 경로 지정
        char szFullPath[MAX_PATH] = {0};
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, m_pPath->GLMapFile().c_str()); //SUBPATH::GLMAP_FILE );
		GLLevelFile::SetPath ( szFullPath );

		// Npc 대화파일
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, m_pPath->NtkFileRoot().c_str()); //SUBPATH::NTK_FILE_ROOT );
		CNpcDialogueSet::SetPath ( szFullPath );

		// Quest 파일
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, m_pPath->QuestFileRoot().c_str()); //SUBPATH::QST_FILE_ROOT );
		GLQuestMan::GetInstance().SetPath ( szFullPath );

		// 버스 파일. 택시 파일
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, m_pPath->GLogicPath().c_str()); //SUBPATH::GLOGIC_FILE );
		GLBusStation::GetInstance().SetPath ( szFullPath );
		GLTaxiStation::GetInstance().SetPath ( szFullPath );
		GLProductRecipeMan::GetInstance().SetPath ( szFullPath );

		// Note : GLogic zip 파일 패스 셋팅
		// zip 파일 패스는 계속 추가
		GLogicData::GetInstance().SetLogicFileFullPath(GLOGIC::UseLogicPackFile());
		GLOGIC::SetEngFullPath();
		GLOGIC::OpenPackFile(szAppPath , RANPARAM::RPFDataPath);
//#ifdef TEST_INTRO_VIDEO
		// TEST (인트로 영상)

		/*if( NULL != pd3dDevice )
		{
			NSDSHOW::InitPlayer( m_hWnd, MSG_DSHOW_PLAYER, pd3dDevice );
			NSDSHOW::DShowTextureMan::GetInstance().OnCreateDevice( pd3dDevice );
			NSDSHOW::g_strVideoPath = szAppPath;
			NSDSHOW::g_strVideoPath.append( "/data/video/" );

			NSDSHOW::LoadAndPlay( "Mincomunication.wmv" );
		}*/
//#endif
	}
	else
	{
		LoadMapList();
	}

	char szFullPath[MAX_PATH] = {0};
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, m_pPath->GfxPath().c_str() );
	NSGFXFILE::g_strGfxPath = szFullPath;

	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, m_pPath->TextureFileRoot().c_str() );
	NSGFXFILE::g_strTexturePath = szFullPath;
	NSGFXFILE::g_strTexturePath.append( "\\gui\\" );

    return S_OK;
}

HRESULT DxGlobalStage::OneTimeSceneInitNext(WORD wWidth, WORD wHeight, const char* szMapList, bool bToolMode)
{

#ifdef LOADING_TIME_CHECK
    boost::timer LoadingTotalTimer;
    LoadingTotalTimer.restart();

    boost::timer LoadingSectionTimer;
    LoadingSectionTimer.restart();
#endif

    if ( wWidth==0 || wHeight==0 )
	{
		RECT rtWin;
		GetClientRect ( m_hWnd, &rtWin );

		wWidth = static_cast<WORD> ( rtWin.right - rtWin.left );
		wHeight = static_cast<WORD> ( rtWin.bottom - rtWin.top );
	}

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

    if ( !m_bEmulate )
	{

#ifdef LOADING_TIME_CHECK
        sc::writeLogInfo(sc::string::format("Loading DxGlobalStage::OneTimeSceneInit Path %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
        LoadingSectionTimer.restart();
#endif
		// Note : 로직 data 초기화.
		HRESULT hr = GLogicData::GetInstance().LoadData(FALSE, m_pPath, bToolMode, NULL);
		if (FAILED(hr))
            return hr;

#ifdef LOADING_TIME_CHECK
        sc::writeLogInfo(sc::string::format("Loading DxGlobalStage::OneTimeSceneInit GLogicData %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
        LoadingSectionTimer.restart();
#endif

	}

	//---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

    // Point Shop cache load
    m_pGaeaClient->PointShopCacheLoad();

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading DxGlobalStage::OneTimeSceneInit PointShop %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

    // Attendance Cfg load
    m_pGaeaClient->AttendanceUILoad();

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

#ifdef LOADING_TIME_CHECK
	sc::writeLogInfo(sc::string::format("Loading DxGlobalStage::OneTimeSceneInit Attendance %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
	LoadingSectionTimer.restart();
#endif

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	// ControlType*.nut (bjju.ControlType)
	m_pGaeaClient->KeySettingLoad();

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

#ifdef LOADING_TIME_CHECK
	sc::writeLogInfo(sc::string::format("Loading GlobalStage::OneTimeSceneInit ControlType*.nut %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
	LoadingSectionTimer.restart();
#endif

/*
    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//
*/

	/*if ( false == LoadMapList() )
		return E_FAIL;*/

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading DxGlobalStage::OneTimeSceneInit Maplist %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

	if ( !m_bEmulate )
	{
		// Initilaize Winsock2
		//
		if (sc::net::initializeNetwork() != NET_OK)
		{
            sc::writeLogError(std::string("NET_InitializeSocket NET_ERROR"));
			return E_FAIL;	
		}
		
		m_NetClient.SetWndHandle ( m_hWnd, this );
	}

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	// 이모티콘 설정 데이터 파일 로딩
	CEmoticon::GetInstance().LOAD(bToolMode);

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading DxGlobalStage::OneTimeSceneInit Emoticon %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	// 인터페이스 설정 파일 로딩
	char szFullPath[ MAX_PATH ] = { 0 };

	StringCchCopy ( szFullPath, MAX_PATH, m_szFullPath );
	StringCchCat ( szFullPath, MAX_PATH, m_pPath->GuiFileRoot().c_str()); //SUBPATH::GUI_FILE_ROOT);
	CInterfaceCfg::GetInstance().SetPath(szFullPath);
	CInterfaceCfg::GetInstance().LoadText(RANPARAM::strUIOuterCfg.GetString(),RANPARAM::strCountry);

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

    CInterfaceCfg::GetInstance().LoadText(RANPARAM::strUIInnerCfg01.GetString(),RANPARAM::strCountry);

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

    CInterfaceCfg::GetInstance().LoadText(RANPARAM::strUIInnerCfg02.GetString(),RANPARAM::strCountry);

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

    CInterfaceCfg::GetInstance().LoadText(RANPARAM::strUIInnerCfg03.GetString(),RANPARAM::strCountry);

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

    CInterfaceCfg::GetInstance().LoadText(RANPARAM::strUIExtCfg.GetString(),RANPARAM::strCountry);

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

    CUIConfigMgr::GetInstance().SetPath(szFullPath);
    CUIConfigMgr::GetInstance().LoadTextureList( RANPARAM::strUITextureList.GetString() );
    CUIConfigMgr::GetInstance().LoadControlList( RANPARAM::strUIXmlList.GetString(), RANPARAM::strCountry );

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading DxGlobalStage::OneTimeSceneInit Interface Text %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	if (!CRanFilter::GetInstance().Init(GLOGIC::UseLogicZipFile(),
										GLOGIC::UseLogicPackFile(),
										GLogicData::GetInstance().GetGlogicZipFile().c_str(), 
										GLOGIC::GetPath())) 
	{
		GASSERT( 0 && "[ERROR] : Ran Filter Initialize." );
		return E_FAIL;
	}

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading DxGlobalStage::OneTimeSceneInit RanFilter %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	// 활성화된 stage 초기화.
	if ( !m_bEmulate )
	{
		m_emThisStage = EM_STAGE_LOBY;
		m_pActiveStage = m_pLobyStage;
		if ( m_pActiveStage)
		{
			HRESULT hr = m_pActiveStage->OneTimeSceneInit ( m_hWnd, wWidth, wHeight, m_szFullPath );
			if (FAILED(hr))
                return E_FAIL;

			m_pLobyStage->SetChangedStage( true );
		}
	}
	else
	{
		m_emThisStage = EM_STAGE_GAME;
		m_pActiveStage = m_pGameStage;
		if ( m_pActiveStage)
		{
			HRESULT hr = m_pActiveStage->OneTimeSceneInit ( m_hWnd, wWidth, wHeight, m_szFullPath );
			if (FAILED(hr))
                return E_FAIL;
		}
	}

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading DxGlobalStage::OneTimeSceneInit ActiveStage %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	// 게임 커서 사용 여부 결정.
	CCursor::GetInstance().SetGameCursor ( RANPARAM::bGameCursor );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	m_pGaeaClient->PrivateMarketDelayLoad();

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	if( m_pGaeaClient->LoadCountryFile() == false )
		return E_FAIL;

	//GfxInterfaceBase::GetInstance()->InitOldInterface();

    return S_OK;
}

HRESULT DxGlobalStage::OneTimeSceneInit(
    const char* szAppPath,
    HWND hWnd,
    BOOL bEmulate,
    WORD wWidth,
    WORD wHeight,
    const char* szMapList,
    bool bToolMode)
{
    if (FAILED(OneTimeSceneInitPrev(szAppPath, hWnd, bEmulate)))
        return E_FAIL;

    if (FAILED(OneTimeSceneInitNext(wWidth, wHeight, szMapList, bToolMode)))
        return E_FAIL;

    return S_OK;
}

/*
                                                                                     2011-08-16
    --------------------------------------------------------------------------------------------------
    DxGlobalStage::InitDeviceObjects m_pd3dDevice->GetDeviceCaps                     0.012   0.012 sec
    DxGlobalStage::InitDeviceObjects GLogicData::GetInstance().InitDeviceObjects     0.517   0.529 sec
    DxGlobalStage::InitDeviceObjects DxSkeletonManager::GetInstance().PreLoad          0.362   0.96 sec
    DxGlobalStage::InitDeviceObjects DxSkinAnimationManager::GetInstance().PreLoad             1.587   2.547 sec
    DxGlobalStage::InitDeviceObjects DxSkinMeshManager::GetInstance().PreLoad            27.03  29.577 sec
    DxGlobalStage::InitDeviceObjects DxSkinCharDataContainer::GetInstance().LoadData 11.774 41.351 sec
    DxGlobalStage::InitDeviceObjects DxFontMan::GetInstance().FindFont               0      41.351 sec
    DxGlobalStage::InitDeviceObjects m_pActiveStage->InitDeviceObjects               8.742  50.093 sec
    DxGlobalStage::InitDeviceObjects m_NetClient.ConnectLoginServer                  0.035  50.128 sec
*/
HRESULT DxGlobalStage::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
#ifdef LOADING_TIME_CHECK
    boost::timer LoadingTotalTimer;
    LoadingTotalTimer.restart();

    boost::timer LoadingSectionTimer;
    LoadingSectionTimer.restart();
#endif

	GASSERT(pd3dDevice);

	HRESULT hr=S_OK;
	m_pd3dDevice = pd3dDevice;

	hr = m_pd3dDevice->GetDeviceCaps ( &m_d3dCaps );
	if (FAILED(hr))
        return hr;

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading DxGlobalStage::InitDeviceObjects m_pd3dDevice->GetDeviceCaps %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	// 로직 data 초기화.
	hr = GLogicData::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	if (FAILED(hr))
        return hr;

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading DxGlobalStage::InitDeviceObjects GLogicData::GetInstance().InitDeviceObjects %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

//    //---------------------------------------------------//
//    NSLoadingDirect::FrameMoveRender( FALSE );
//    //---------------------------------------------------//
//
//	DxSkeletonManager::GetInstance().PreLoad ( _PRELOAD_BONE, m_pd3dDevice );
//
//#ifdef LOADING_TIME_CHECK
//    sc::writeLogInfo(sc::string::format("Loading DxGlobalStage::InitDeviceObjects DxSkeletonManager::GetInstance().PreLoad %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
//    LoadingSectionTimer.restart();
//#endif

//    //---------------------------------------------------//
//    NSLoadingDirect::FrameMoveRender( FALSE );
//    //---------------------------------------------------//
//
//	//DxSkinAnimationManager::GetInstance().BackUpList( _PRELOAD_ANI );
//
//#ifdef LOADING_TIME_CHECK
//    sc::writeLogInfo(sc::string::format("Loading DxGlobalStage::InitDeviceObjects DxSkinAnimationManager::GetInstance().PreLoad %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
//    LoadingSectionTimer.restart();
//#endif

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//
//
//	DxSkinMeshManager::GetInstance().PreLoad( _PRELOAD_SKIN, m_pd3dDevice, FALSE );
//
//#ifdef LOADING_TIME_CHECK
//    sc::writeLogInfo(sc::string::format("Loading DxGlobalStage::InitDeviceObjects DxSkinMeshManager::GetInstance().PreLoad %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
//    LoadingSectionTimer.restart();
//#endif

	for ( int i=0; i<GLCI_NUM_ACTOR; ++i )
		DxSkinCharDataContainer::GetInstance().LoadData( GLCONST_CHAR::szCharSkin[i], m_pd3dDevice );

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading DxGlobalStage::InitDeviceObjects DxSkinCharDataContainer::GetInstance().LoadData %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

	DxFontMan::GetInstance().FindFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW|D3DFONT_ASCII );
	DxFontMan::GetInstance().FindFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading DxGlobalStage::InitDeviceObjects DxFontMan::GetInstance().FindFont %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	m_pGfxInterface = GfxInterfaceBase::GetInstance();

	hr = GLWidgetScript::GetInstance().Init( m_pGaeaClient, m_pGfxInterface, m_pPath );
	
	// 에뮬레이터에선 초기화 작업 안되어있다. 여기서 해주자.
	if( TRUE == m_bEmulate )
	{
		m_pGfxInterface->SetGaeaClient( m_pGaeaClient );
		m_pGfxInterface->StorePresentParameters( m_pD3DApp->GetPresentParams() );
		m_pGfxInterface->SetFontLanguage( GetTextLanguage() );
		m_pGfxInterface->OnCreateDevice( m_hWnd, m_pEngineDevice->GetDevice() );
		m_pGfxInterface->OnInitLoading();
	}

	m_pGfxInterface->OnInitGame();
	m_pGfxInterface->InitOldInterface();

	GLWidgetTextureMan::GetInstance().OnCreateDevice( m_pEngineDevice->GetDevice() );

	if ( m_pActiveStage )
	{
		hr = m_pActiveStage->InitDeviceObjects ( m_pd3dDevice );
		if (FAILED(hr))
            return E_FAIL;
	}

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading DxGlobalStage::InitDeviceObjects m_pActiveStage->InitDeviceObjects %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

	// Connect Login Server
	//
	if (!m_bEmulate)
	{
        if (m_NetClient.ConnectLoginServer(RANPARAM::LoginAddress, RANPARAM::LoginServerPort) <= NET_ERROR)
			CDebugSet::ToListView("DxGlobalStage::InitDeviceObjects ConnectLoginServer NET_ERROR");
		else
			CDebugSet::ToListView("DxGlobalStage::InitDeviceObjects ConnectLoginServer NET_OK");

		// Send version information
		//
		m_NetClient.SndVersion ();
		m_NetClient.SndReqServerInfo ();
	}

	//if( NULL != m_pGfxInterface )
	//	m_pGfxInterface->OnEndLoading();

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading DxGlobalStage::InitDeviceObjects m_NetClient.ConnectLoginServer %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif
	return S_OK;
}

HRESULT DxGlobalStage::RestoreDeviceObjects ()
{
	HRESULT hr=S_OK;
	if ( !m_pd3dDevice )	return S_FALSE;

	if ( m_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGVERTEX )
	{
		m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,		TRUE );
	}

	float fFogStart=720.0f, fFogEnd=790.0f, fFongDensity=0.0f;

	m_pd3dDevice->SetRenderState ( D3DRS_FOGSTART,		*((DWORD *)(&fFogStart)) );
	m_pd3dDevice->SetRenderState ( D3DRS_FOGEND,		*((DWORD *)(&fFogEnd)) );
	m_pd3dDevice->SetRenderState ( D3DRS_FOGDENSITY,	*((DWORD *)(&fFongDensity)) );

	m_pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE,	D3DFOG_LINEAR );
	m_pd3dDevice->SetRenderState ( D3DRS_FOGTABLEMODE,	D3DFOG_NONE );

	if ( m_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGRANGE )
	{
		m_pd3dDevice->SetRenderState ( D3DRS_RANGEFOGENABLE,	TRUE );
	}

	if ( m_pActiveStage)
	{
		hr = m_pActiveStage->RestoreDeviceObjects ();
		if ( FAILED(hr) )		
			return E_FAIL;
	}

//#ifdef TEST_INTRO_VIDEO
	NSDSHOW::OnResetDevicePlayer();
	NSDSHOW::DShowTextureMan::GetInstance().OnResetDevice();
//#endif

	GLWidgetTextureMan::GetInstance().OnResetDevice();

	if( NULL != m_pGfxInterface )
	{
		m_pGfxInterface->StorePresentParameters( m_pD3DApp->GetPresentParams() );
		m_pGfxInterface->OnResetDevice( m_pd3dDevice );
	}

	return S_OK;
}

HRESULT DxGlobalStage::ChangeStage ( WORD wWidth, WORD wHeight )
{
	std::string strLoadingImageName;
	BOOL bLobbyLoadingImageFullSize(FALSE);

	SNATIVEID sNativeID;
	ELoadingSceneType eLoadingType;
	switch ( m_emChangeStage )
	{
	case EM_STAGE_LOBY:
	case EM_STAGE_LOBY_WORLDBATTLE:
		strLoadingImageName = NSInitClientLua::GetRandomLobbyLoadingImage();
		bLobbyLoadingImageFullSize = NSInitClientLua::GetLobbyLoadingImageFullSize();
		sNativeID = NATIVEID_NULL();
		eLoadingType = ELoadingSceneType::LogOut;
		break;

	case EM_STAGE_GAME:
		{
			DxLobyStage * pLobbyStage = GetLobyStage();
			if (!pLobbyStage)
                break;
			
			SCHARINFO_LOBBY * pCharInfo_Lobby = pLobbyStage->GetSelectCharInfo();
			if (!pCharInfo_Lobby) 
                break;

            const DxCharJoinData* pJoinData = m_pGameStage->GetCharJoinData();
			const SMAPNODE* pMapNode = NULL;
			if (pCharInfo_Lobby->m_sSaveMapID != NATIVEID_NULL())
				pMapNode = m_pGaeaClient->FindMapNode(pCharInfo_Lobby->m_sSaveMapID.dwID);
			else
				pMapNode = m_pGaeaClient->FindMapNode(pJoinData->m_CharData2.m_sStartMapID); // by luxes.

			if (!pMapNode)
                break;

			strLoadingImageName = pMapNode->m_LoadingImageName;

			if (pCharInfo_Lobby->m_sSaveMapID != NATIVEID_NULL())
				sNativeID = pCharInfo_Lobby->m_sSaveMapID;
			else
				sNativeID = pJoinData->m_CharData2.m_sStartMapID;

			eLoadingType = ELoadingSceneType::MoveMap;
		}
		break;
	};

	//m_pGfxInterface->SetLoadingData( sNativeID, strLoadingImageName, false );
	//m_pGfxInterface->OnBeginLoading( eLoadingType );

	LoadingThread::SetData(
        strLoadingImageName.c_str(),
        sNativeID,
		false,
		false,
		bLobbyLoadingImageFullSize );

    LoadingDirectClient sLodingDirect( FALSE );
	NSLoadingDirect::Set( &sLodingDirect, EM_LT_NORMAL, TRUE );		// [shhan][2015.09.18] 기획팀의 요청으로 좌측상단에 WB 승리국기 안 나오는 방향으로 됨.
																	// [shhan][2016.03.11] rm #2858 - [GS] 난사군도 우승자 국기 + 문구 추가 요청 ( 재변경 )

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( TRUE );
    //---------------------------------------------------//


	m_pEngineDevice->DoInterimClean_Prev();

	// 통합전장 stage는 Lobby stage를 포함하고 있기때문에 Lobby로 전송되는 패킷에 영향을 받게 된다. 
	// Lobby에 추가될때마다 통합전장 stage에 중복으로 추가할 수 없기때문에 통합전장일때 Lobby stage를 DeActive 하게 한다.
	switch ( m_emThisStage )
	{
	case EM_STAGE_LOBY_WORLDBATTLE:
		// 액티브 stage 비활성화.
		//
		if (m_pLobyStage)
		{
			if( m_pLobyStage->DeActive () != S_OK )
			{
				sc::writeLogError(std::string("LobyStage DeActive ERROR"));
			}
		}
		break;
	default:
		if (m_pActiveStage)
		{
			if( m_pActiveStage->DeActive () != S_OK )
			{
				sc::writeLogError(std::string("DeActive ERROR"));
			}
		}
		break;
		
	}

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	GLWidgetTextureMan::GetInstance().CleanUp();

	if( m_pd3dDevice != NULL )
		m_pEngineDevice->DoInterimClean(m_pd3dDevice);
	else
		sc::writeLogError(std::string("d3dDevice ERROR"));

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	// 케릭터 기본 스킨 다시 로드.
	for ( int i=0; i<GLCI_NUM_ACTOR; ++i )
	{
		DxSkinCharDataContainer::GetInstance().LoadData( GLCONST_CHAR::szCharSkin[i], m_pd3dDevice );
	}

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	// 활성화 stage 변경.
	//
	switch (m_emChangeStage)
	{
	case EM_STAGE_LOBY_WORLDBATTLE:
		m_pActiveStage = m_pWBStage;
		// 현제 stage 활성화.
		if( m_pActiveStage )
			m_pActiveStage->SetActive(m_pd3dDevice, m_hWnd, wWidth, wHeight, true);
		break;
	case EM_STAGE_LOBY:
        m_pActiveStage = m_pLobyStage;

		m_pLobyStage->SetChangedStage( true );

		// 현제 stage 활성화.
		if( m_pActiveStage )
			m_pActiveStage->SetActive(m_pd3dDevice, m_hWnd, wWidth, wHeight);
        break;
	case EM_STAGE_GAME:
        m_pActiveStage = m_pGameStage;
	// 현제 stage 활성화.
		if( m_pActiveStage )
			m_pActiveStage->SetActive(m_pd3dDevice, m_hWnd, wWidth, wHeight);
        break;
	};

	//// 현제 stage 활성화.
	////
	//if (m_pActiveStage && bWB == false )
	//	m_pActiveStage->SetActive(m_pd3dDevice, m_hWnd, wWidth, wHeight);

	ReSetSTATE(EM_CHANGE);
	m_emThisStage = m_emChangeStage;

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( TRUE );
    //---------------------------------------------------//

	NSLoadingDirect::Reset();

	m_pEngineDevice->SetRenderState();

	//m_pGfxInterface->OnEndLoading();

	return S_OK;
}

HRESULT DxGlobalStage::GameToLobbyStage (bool bWorldBattle)
{
	GLWidgetSaveProp::GetInstance().SaveXML();

	if (m_bEmulate)
        return S_FALSE;

//#if defined ( CH_PARAM ) || defined ( HK_PARAM ) || defined ( TW_PARAM ) // Apex 적용
	// APEX 버젼 변경
    if (m_ServiceProvider == SP_CHINA || m_ServiceProvider == SP_HONGKONG || m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_THAILAND)
	    APEX_CLIENT::StopApexClient();
//#endif

	// 네트웍 연결 종료.
	if ( m_NetClient.IsOnline() == TRUE )
		m_NetClient.CloseConnect ();

#ifdef _NP30
	m_dwNProtect30ServerNumber++;
	//sc::writeLogDebug(sc::string::format("DxGlobalStage::GameToLobbyStage - m_dwNProtect30ServerNumber : %1%", m_dwNProtect30ServerNumber));
#endif
	// 메세지 버퍼 초기화.
	//
	EnterCriticalSection(&m_CSMsgProcLock);
	{
		std::for_each ( m_MsgBufferA.begin(), m_MsgBufferA.end(), std_afunc::DeleteArray() );
		m_MsgBufferA.clear ();

		std::for_each ( m_MsgBufferB.begin(), m_MsgBufferB.end(), std_afunc::DeleteArray() );
		m_MsgBufferB.clear ();
	}
	LeaveCriticalSection(&m_CSMsgProcLock);

	m_pGameStage->ResetCharJoinData();

	// 스테이지 변경 요청.
	//
	if( bWorldBattle )
	{
		ToChangeStage ( EM_STAGE_LOBY_WORLDBATTLE );
		SetSTATE(EM_REQ_CONNECT_WORLDBATTLE_LOGIN_SVR);
	}
	else
	{
		ToChangeStage ( EM_STAGE_LOBY );
		SetSTATE(EM_REQCONNECT_LOGINSVR);
	}

	return S_OK;
}

HRESULT DxGlobalStage::ToChangeStage ( EMSTAGE emChangeStage )
{
	if ( m_emThisStage==emChangeStage )	return S_FALSE;

	SetSTATE(EM_CHANGE);
	m_emChangeStage = emChangeStage;

	return S_OK;
}

void DxGlobalStage::CloseGame ( LPCTSTR lpszMsg )
{
	SetSTATE(EM_CLOSE);

	if (lpszMsg)
	{
        sc::writeLogError(std::string(lpszMsg));
		MessageBox( m_hWnd, lpszMsg, "Error Message", MB_OK ); // or use game’s UI
	}

    ::PostMessage ( m_hWnd, WM_CLOSE, 0, 0 );	
}

HRESULT DxGlobalStage::FrameMove ( float m_fTime, float m_fElapsedTime, BOOL bNotRendering )
{
	HRESULT hr=S_OK;
	
	if (IsSTATE(EM_CLOSE))
	{
        return S_FALSE;
	}

	// 에뮬레이터에서 메세지 지연을 강제로 만들기 위해서.
	//
	if ( m_bEmulate )
	{
		m_pEmulMsgField->_update_emul_msg(
            GetGameStage()->GetClientID(),
            GetGameStage()->GetGaeaID(),
            m_fElapsedTime);
	}   

	// 수신된 메시지처리.
	//
	PROFILE_BEGIN("net msg");
	MsgProcessFrame ();
	PROFILE_END("net msg");

	// 스테이지변경시에 처리.
	//
	if ( IsSTATE(EM_CHANGE) )
	{
		hr = ChangeStage ( m_wWidth, m_wHeight );
		if ( FAILED(hr) )	return hr;

		//if ( m_emChangeStage == EM_STAGE_GAME )
		//{
		//	m_pEngineDevice->DoInterimClean_Prev();

		//	m_pActiveStage->Test ();

		//	m_pEngineDevice->DoInterimClean(m_pd3dDevice);

		//	// 케릭터 기본 스킨 다시 로드.
		//	for ( int i=0; i<GLCI_NUM_ETC; ++i )
		//	{
		//		DxSkinCharDataContainer::GetInstance().LoadData( GLCONST_CHAR::szCharSkin[i], m_pd3dDevice );
		//	}

		//	m_pActiveStage->Test2 ();
		//}
	}
    
	// Connect Login Server
	//
	if (!m_bEmulate && IsSTATE(EM_REQCONNECT_LOGINSVR))
	{
        if (m_NetClient.ConnectLoginServer(RANPARAM::LoginAddress, RANPARAM::LoginServerPort) <= NET_ERROR)
		{
			CloseGame();
			CDebugSet::ToListView("DxGlobalStage::FrameMove ConnectLoginServer NET_ERROR");
		}
		else
		{
			CDebugSet::ToListView("DxGlobalStage::FrameMove ConnectLoginServer NET_OK");
			// Send version information
			m_NetClient.SndVersion();
			m_NetClient.SndReqServerInfo();
		}

		ReSetSTATE(EM_REQCONNECT_LOGINSVR);
	}
	else if (!m_bEmulate && IsSTATE(EM_REQ_CONNECT_WORLDBATTLE_LOGIN_SVR))
	{
        if (m_NetClient.ConnectLoginServer(RANPARAM::WB_LoginAddress, RANPARAM::WB_LoginServerPort) <= NET_ERROR)
		{
			m_NetClient.SetWorldBattle(FALSE);
			CloseGame();
			CDebugSet::ToListView("DxGlobalStage::FrameMove ConnectWorldBattleLoginServer NET_ERROR");
		}
		else
		{
			CDebugSet::ToListView("DxGlobalStage::FrameMove ConnectWorldBattleLoginServer NET_OK");
			// Send version information
			m_NetClient.SndVersion();
			m_NetClient.SndReqServerInfo();			

			m_NetClient.SetWorldBattle(TRUE);
			m_NetClient.SetWorldBattleCharSelectEnd(FALSE);
		}

		SetSTATE(EM_REQ_CONNECT_WORLDBATTLE_GAME_SVR);
		ReSetSTATE(EM_REQ_CONNECT_WORLDBATTLE_LOGIN_SVR);
	}
	else if (!m_bEmulate && IsSTATE(EM_REQ_CONNECT_WORLDBATTLE_GAME_SVR))
	{
		if( m_NetClient.IsWorldBattleSvrInfoEnd() )
		{
			// 통합전장에 로그인 시도는 한번만 가능하다. 만약 여러 이유로 로그인 실패한다면 다시 라이브서버에 접속해서 통합전장으로 이동 필요
			m_NetClient.SetWorldBattleSvrInfoEnd(FALSE);

			if( GLLoginClient::GetInstance().LoginWorldBattle() == TRUE )
			{
				SetSTATE(EM_REQ_CONNECT_WORLDBATTLE_CHA_SELECT);
				ReSetSTATE(EM_REQ_CONNECT_WORLDBATTLE_GAME_SVR);
			}
			else
			{
				return E_FAIL;
			}
		}
	}
	else if (!m_bEmulate && IsSTATE(EM_REQ_CONNECT_WORLDBATTLE_CHA_SELECT))
	{
		int nServerChannel = 0;

		// GS는 접속전에 WB 접속 여부를 확인하지만 전장서버에서는 확인할 필요없이 바로 접속함.
		if (m_ServiceProvider == SP_GS )
			m_NetClient.GsSndLogin( GetWorldBattleID(), "1111", nServerChannel );
		else
			m_NetClient.SndLogin( GetWorldBattleID(), "1111", "", nServerChannel );

		m_NetClient.SetWorldBattleCharSelectEnd(TRUE);

		SetSTATE(EM_REQ_CONNECT_WORLDBATTLE_JOIN_GAME);
		ReSetSTATE(EM_REQ_CONNECT_WORLDBATTLE_CHA_SELECT);
	}
	else if (!m_bEmulate && IsSTATE(EM_REQ_CONNECT_WORLDBATTLE_JOIN_GAME))
	{	
		ReSetSTATE(EM_REQ_CONNECT_WORLDBATTLE_JOIN_GAME);
	}

	if( NULL != m_pGfxInterface && TRUE == m_pGameStage->m_bUseInterface )
	{
		PROFILE_BEGIN( "GfxInterface::OnFrameMove" );

		GLWidgetScript::GetInstance().OnFrameMove( m_fElapsedTime );
		GLWidgetTextureMan::GetInstance().OnFrameMove( m_fElapsedTime );
		m_pGfxInterface->OnFrameMove( m_fElapsedTime );

		PROFILE_END( "GfxInterface::OnFrameMove" );
	}

	if (m_pActiveStage)
	{
		hr = m_pActiveStage->FrameMove(m_fTime, m_fElapsedTime, bNotRendering);
		if (FAILED(hr))
            return E_FAIL;

	}

	return S_OK;
}

HRESULT DxGlobalStage::Render ( const D3DXCOLOR& cClearColor )
{
	HRESULT hr=S_OK;
	if ( IsSTATE(EM_CLOSE) )
	{
		return S_FALSE;
	}

	if ( m_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGVERTEX )		
		m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, FALSE );

	if ( m_pActiveStage)
	{
		HRESULT hr(S_OK);

		if ( DxFogMan::GetInstance().GetFogRange() == EMFR_OFF )
		{
			hr = m_pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pColorBuffer );
			hr = m_pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pZBuffer );
		}
		else
		{
			if ( RENDERPARAM::g_bEnable_HDR )
			{
				hr = m_pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR );
			}
			else
			{
				hr = m_pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pColorBuffer );

			}

			//hr = m_pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pZBuffer );
			if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_NM )
			{
				hr = m_pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pSufFourCC_INTZ );
			}
			else
			{
				hr = m_pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pZBuffer );
			}
		}

		if( m_pActiveStage->IsWBStage() == false)
		{
			hr = m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, cClearColor, 1.0f, 0L );
			if ( FAILED(hr) )		
			{
				sc::writeLogError(std::string(" m_pActiveStage->Clear ERROR"));
				return E_FAIL;
			}
		}


		//if ( DxSurfaceTex::GetInstance().m_pColorSuf_1st && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_NM && m_pActiveStage->GetUsedMaterialSystem() )
		//{
		//	hr = m_pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pColorSuf_1st );
		//}

		//DxBackupTarget sBackupTargetGfx;
		//sBackupTargetGfx.Backup( m_pd3dDevice );

		if( m_pActiveStage )
		{
			hr = m_pActiveStage->Render();
			if ( FAILED(hr) )		
				return E_FAIL;
		}

		if( NULL != m_pGfxInterface && TRUE == m_pGameStage->m_bUseInterface )
		{
			PROFILE_BEGIN2( "GfxInterface::OnFrameRender" );

			GLWidgetTextureMan::GetInstance().OnFrameRender();
			m_pGfxInterface->OnFrameRender( m_pD3DApp->GetElapsedTime() );

			PROFILE_END2( "GfxInterface::OnFrameRender" );
		}

		// Interface에서 DxSurfaceTex::GetInstance().m_pGlowSuf_SRC,
		//				DxSurfaceTex::GetInstance().m_pBurnNewSuf 값을 변화시킬 수 있으므로 정리를 해준다.
		{
			DxBackupTarget sBackupTarget;
			sBackupTarget.Backup( m_pd3dDevice );

			if ( DxSurfaceTex::GetInstance().m_pGlowSuf_SRC )
			{
				m_pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pGlowSuf_SRC );
				m_pd3dDevice->SetDepthStencilSurface ( NULL );
				m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);
			}

			if ( DxSurfaceTex::GetInstance().m_pBurnNewSuf )
			{
				m_pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pBurnNewSuf );
				m_pd3dDevice->SetDepthStencilSurface ( NULL );
				m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);
			}
		}
	}

	return S_OK;
}

HRESULT DxGlobalStage::InvalidateDeviceObjects ()
{
	HRESULT hr=S_OK;

	if ( m_pActiveStage)
	{
		hr = m_pActiveStage->InvalidateDeviceObjects ();
		if ( FAILED(hr) )		
			return E_FAIL;
	}
	
//#ifdef TEST_INTRO_VIDEO
	NSDSHOW::OnLostDevicePlayer();
	NSDSHOW::DShowTextureMan::GetInstance().OnLosetDevice();
//#endif

	GLWidgetTextureMan::GetInstance().OnLostDevice();

	if( NULL != m_pGfxInterface )
		m_pGfxInterface->OnLostDevice();

	return S_OK;
}

HRESULT DxGlobalStage::DeleteDeviceObjects ()
{
	HRESULT hr=S_OK;

	// 로직 data 삭제
	//
	hr = GLogicData::GetInstance().DeleteDeviceObjects ();
	if ( FAILED(hr) )	return hr;

	if( NULL != m_pGfxInterface )
		m_pGfxInterface->OnDestroyDevice();

	GLWidgetTextureMan::GetInstance().OnDestroyDevice();

	// 저정된 UI 프로퍼티 XML 익스포트
	GLWidgetSaveProp::GetInstance().SaveXML();

	if( true == NSWIDGET_SCRIPT::g_bWidgetDebug )
		FreeConsole();

	if ( m_pActiveStage)
	{
		hr = m_pActiveStage->DeleteDeviceObjects ();
		if ( FAILED(hr) )		
			return E_FAIL;
	}
	return S_OK;	
}

HRESULT DxGlobalStage::FinalCleanup ()
{
	HRESULT hr=S_OK;

	if ( m_pActiveStage)
	{
		hr = m_pActiveStage->FinalCleanup ();
		if ( FAILED(hr) )		
			return E_FAIL;
	}

	hr = GLogicData::GetInstance().ClearData ();
	if ( FAILED(hr) )	return hr;

	// 에뮬레이터가 아니면 네트웍 연결 종료.
	if ( !m_bEmulate )
	{
		if ( m_NetClient.IsOnline() == true )
			m_NetClient.CloseConnect ();

		sc::net::closeNetwork();
	}

	// 메세지 버퍼 초기화.
	//
	EnterCriticalSection(&m_CSMsgProcLock);
	{
		std::for_each ( m_MsgBufferA.begin(), m_MsgBufferA.end(), std_afunc::DeleteArray() );
		m_MsgBufferA.clear ();

		std::for_each ( m_MsgBufferB.begin(), m_MsgBufferB.end(), std_afunc::DeleteArray() );
		m_MsgBufferB.clear ();
	}
	LeaveCriticalSection(&m_CSMsgProcLock);

//#ifdef TEST_INTRO_VIDEO
	NSDSHOW::ReleasePlayer();
	NSDSHOW::DShowTextureMan::GetInstance().OnDestroyDevice();
//#endif
	return S_OK;
}

bool DxGlobalStage::MsgProc(MSG* pMsg)
{
	// gfx 메시지 처리
	if( NULL != m_pGfxInterface )
	{
		// 윈도우 기본 프록시저 스킵
		if( true == m_pGfxInterface->ProcessEvent( m_hWnd, pMsg->message, pMsg->wParam, pMsg->lParam ) )
			return false;
	}

	m_pEngineDevice->MsgProc(pMsg);

//#ifdef TEST_INTRO_VIDEO
	if( pMsg->message == MSG_DSHOW_PLAYER )
	{
		if( true == NSDSHOW::IsPlayerPlayEnd() )
			NSDSHOW::ReleasePlayer();
	}
//#endif

	return true;
}

LRESULT DxGlobalStage::OnNetNotify(WPARAM wParam, LPARAM lParam)
{
	m_NetClient.ClientProcess (wParam, lParam);

	return 0L;
}

void DxGlobalStage::MsgQueueFlip ()
{
	EnterCriticalSection(&m_CSMsgProcLock);
	{
		MSGBUFFERLIST* pMsgTemp = m_pMsgActive;

		m_pMsgActive = m_pMsgReceive;
		m_pMsgReceive = pMsgTemp;
	}
	LeaveCriticalSection(&m_CSMsgProcLock);
}

void DxGlobalStage::MsgProcess ( NET_MSG_GENERIC* nmg )
{
	EnterCriticalSection(&m_CSMsgProcLock);
	{
		MSGBUFFERLIST &MsgBufferList = GetActiveMsgList();

		PBYTE pBuffer = new BYTE[nmg->Size()];
		memcpy ( pBuffer, nmg, sizeof(BYTE)*nmg->Size() );

		MsgBufferList.push_back ( pBuffer );
	}
	LeaveCriticalSection(&m_CSMsgProcLock);
}

void DxGlobalStage::MsgProcessFrame ()
{
	// Active Msg Buffer 플립 시킴.
	//
	MsgQueueFlip ();

	// 현제 가져올 메세지 버퍼 리스트를 가져옴.
	MSGBUFFERLIST &MsgReceivedMsg = GetReceivedMsgList();

	MSGBUFFERLIST_ITER iter = MsgReceivedMsg.begin();
	MSGBUFFERLIST_ITER iter_end = MsgReceivedMsg.end();
	for ( ; iter!=iter_end; ++iter )
	{
		NET_MSG_GENERIC* nmg = (NET_MSG_GENERIC*) (*iter);

		if ( nmg->nType==NET_MSG_COMBINE )
		{
			GLMSG::SNET_COMBINE &sMSG_COMBINE = *((GLMSG::SNET_COMBINE*)nmg);
			GASSERT(sMSG_COMBINE.IsVALID());

			sMSG_COMBINE.RESET_CUR();

			WORD wNUM(0);
			NET_MSG_GENERIC* pMsg(NULL);
			while(sMSG_COMBINE.POPMSG(pMsg))
			{
				wNUM++;
				GASSERT(wNUM<NET_DATA_BUFSIZE&&"sMSG_COMBINE.POPMSG()");
				if (wNUM > NET_DATA_BUFSIZE)
                    continue;
				MsgProcessFrame ( pMsg );
			}
		}
		else
		{
			MsgProcessFrame ( nmg );
		}
	}

	std::for_each ( MsgReceivedMsg.begin(), MsgReceivedMsg.end(), std_afunc::DeleteArray() );
	MsgReceivedMsg.clear ();
}


void DxGlobalStage::MsgApexData(NET_MSG_GENERIC* pMsg)
{
//#if defined ( CH_PARAM ) || defined ( HK_PARAM ) || defined ( TW_PARAM ) // Apex 적용
	// APEX 버젼 변경
    if (m_ServiceProvider == SP_CHINA || m_ServiceProvider == SP_HONGKONG || m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_THAILAND)
    {

        const NET_APEX_DATA * pNAD;
        pNAD = reinterpret_cast<NET_APEX_DATA*> (pMsg);
        int nLength = pNAD->Size() - sizeof(NET_MSG_GENERIC);
        APEX_CLIENT::NoticeApex_UserData( pNAD->szData, nLength );
    }
//#endif
}

void DxGlobalStage::MsgPostRenew( NET_MSG_GENERIC* pMsg )
{
	m_pGaeaClient->MsgPostProcess( pMsg );
}


void DxGlobalStage::MsgUpdateUserCountryAC ( NET_MSG_GENERIC* pMsg )
{
	GLMSG::NET_COUNTRY_CHANGE_AFC* pCountryMsg =
		static_cast< GLMSG::NET_COUNTRY_CHANGE_AFC* >( pMsg );

	if ( pCountryMsg )
	{
		if ( m_pGaeaClient )
		{
			m_pGaeaClient->SetCountryInfo ( pCountryMsg->m_sCountryInfo );

			m_pGaeaClient->PrintConsoleText(
				ID2GAMEINTEXT( "COMPLETE_INITIALIZE_COUNTRY_TEXT" ) );

			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_UpdateSelectCountry,
				"-n, -n",
				pCountryMsg->m_sCountryInfo.wContinentID,
				pCountryMsg->m_sCountryInfo.wCountryID );
		}
	}
}

void DxGlobalStage::MsgSyncVictoriousCountry ( NET_MSG_GENERIC* pMsg )
{
	Country::GLVictoriousCountryManClient* pVCountryMan =
		Country::GLVictoriousCountryManClient::Instance();

	if ( pVCountryMan )
		pVCountryMan->MsgSyncVictoriousCountry( pMsg );
}

void DxGlobalStage::MsgSyncUseFeatures ( NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNET_SYNC_USE_FEATURES_AC* pSyncMsg =
		static_cast< GLMSG::SNET_SYNC_USE_FEATURES_AC* >( pMsg );
	if ( pSyncMsg )
	{
		for ( int i=0; i<GLUseFeatures::EMUSE_FEATURE_SIZE; ++i )
		{
			GLUseFeatures::GetInstance().SetUseFeature(
				i, pSyncMsg->bUseFeatures[ i ] );
		}
	}

	OnInitialize ();
}

void DxGlobalStage::MsgSyncUseFeaturesJoinWorldBattle ( NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNET_SYNC_USE_FEATURES_JOINWORLDBATTLE_AC* pSyncMsg = static_cast< GLMSG::SNET_SYNC_USE_FEATURES_JOINWORLDBATTLE_AC* >( pMsg );
	if ( pSyncMsg )
	{
		GLUseFeatures::GetInstance().SetUseFeature( pSyncMsg->emUserFeatures, pSyncMsg->bUseFeatures );
		UpdateJoinWorldBattle(pSyncMsg->bUseFeatures);
	}
}


void DxGlobalStage::MsgGameGuardAuth1(NET_MSG_GENERIC* pMsg)
{
    const NET_GAMEGUARD_AUTH_1* pNGA = reinterpret_cast<NET_GAMEGUARD_AUTH_1*> (pMsg);
    P_AUTH_DATA pAD = new AUTH_DATA;

	//sc::writeLogDebug(sc::string::format("NET_MSG_GAMEGUARD_AUTH_1 : Seq %1%, PacketSize %2%", pNGA->packetSeq, pNGA->packetSize));

#ifndef _NP30
    memmove( &(pAD->gg_Auth_Data), &(pNGA->ggad), sizeof(GG_AUTH_DATA) );
#else
	memcpy(pAD->packet, pNGA->packet, sizeof(BYTE)*512);
	pAD->bPass = pNGA->bPass;
	pAD->packetSize = pNGA->packetSize;
	pAD->packetSeq = pNGA->packetSeq;
#endif
	pAD->Auth_Msg = NET_MSG_GAMEGUARD_AUTH_1;
	
	EnterCriticalSection(&m_CS);
    m_GGAuthBuffer.push( pAD );
	LeaveCriticalSection(&m_CS);
}

void DxGlobalStage::MsgGameGuardAuth2(NET_MSG_GENERIC* pMsg)
{
    const NET_GAMEGUARD_AUTH_2* pNGA = reinterpret_cast<NET_GAMEGUARD_AUTH_2*>(pMsg);

	//sc::writeLogDebug(sc::string::format("NET_MSG_GAMEGUARD_AUTH_2 : Seq %1%, PacketSize %2%", pNGA->packetSeq, pNGA->packetSize));

    P_AUTH_DATA pAD = new AUTH_DATA;
#ifndef _NP30
    memmove( &(pAD->gg_Auth_Data), &(pNGA->ggad), sizeof(GG_AUTH_DATA) );
#else
	memcpy(pAD->packet, pNGA->packet, sizeof(BYTE)*512);
	pAD->bPass = pNGA->bPass;
	pAD->packetSize = pNGA->packetSize;
	pAD->packetSeq = pNGA->packetSeq;
#endif
	pAD->Auth_Msg = NET_MSG_GAMEGUARD_AUTH_2;

	EnterCriticalSection(&m_CS);
    m_GGAuthBuffer.push( pAD );
	LeaveCriticalSection(&m_CS);
}

void DxGlobalStage::MsgGameGuardAuth(NET_MSG_GENERIC* pMsg)
{
    const NET_GAMEGUARD_AUTH* pNGA = reinterpret_cast<NET_GAMEGUARD_AUTH*>(pMsg);

	//sc::writeLogDebug(sc::string::format("NET_MSG_GAMEGUARD_AUTH : Seq %1%, PacketSize %2%", pNGA->packetSeq, pNGA->packetSize));

    P_AUTH_DATA pAD = new AUTH_DATA;
#ifndef _NP30
    memmove( &(pAD->gg_Auth_Data), &(pNGA->ggad), sizeof(GG_AUTH_DATA) );
#else
	memcpy(pAD->packet, pNGA->packet, sizeof(BYTE)*512);
	pAD->packetSize = pNGA->packetSize;
	pAD->bPass = pNGA->bPass;
	pAD->packetSeq = pNGA->packetSeq;
#endif
	pAD->Auth_Msg = NET_MSG_GAMEGUARD_AUTH;

	EnterCriticalSection(&m_CS);
    m_GGAuthBuffer.push( pAD );
	LeaveCriticalSection(&m_CS);
}

void DxGlobalStage::MsgGmKickUserProc(NET_MSG_GENERIC* pMsg)
{
    m_NetClient.CloseConnect();
}

void DxGlobalStage::MsgRanMobileCommandResponse( NET_MSG_GENERIC* pMsg )
{
	GLRanMobileClient::GetInstance().MsgCommandResponse( pMsg );
}

UINT DxGlobalStage::GetGGAuthData(PGG_AUTH_DATA pGG)
{
	if (m_GGAuthBuffer.empty())
        return 0;

	EnterCriticalSection(&m_CS);

	P_AUTH_DATA pData = m_GGAuthBuffer.front();
#ifdef _NP30
	return 0;
#endif
	memmove(pGG, &(pData->gg_Auth_Data), sizeof(GG_AUTH_DATA));
	UINT nRet = pData->Auth_Msg;
	m_GGAuthBuffer.pop();

	LeaveCriticalSection(&m_CS);

	SAFE_DELETE(pData);
	return nRet;
}

UINT DxGlobalStage::GetGGAuth30Data(P_AUTH_DATA pAD)
{
	if (m_GGAuthBuffer.empty())
        return 0;

#ifndef _NP30
	return 0;
#endif

	//sc::writeLogDebug(sc::string::format("GetGGAuth30Data SIZE %1%", m_GGAuthBuffer.size()));

	if( m_GGAuthBuffer.size() > 8 )
	{
		//sc::writeLogDebug(sc::string::format("GetGGAuth30Data SIZE OVER THAN 8 - %1%",m_GGAuthBuffer.size()));
		bool bStop = false;

		EnterCriticalSection(&m_CS);

		for( int idx = 0; idx<8; idx++)
		{
			P_AUTH_DATA pTemp = m_GGAuthBuffer.front();
			
			if(pTemp->packetSeq == 8 )
				bStop = true;

			SAFE_DELETE(pTemp);
			m_GGAuthBuffer.pop();

			if(bStop)
				break;
		}

		LeaveCriticalSection(&m_CS);

		return 0;
	}
	else if( m_GGAuthBuffer.size() == 8 )
	{
		UINT nRet = 0;

		EnterCriticalSection(&m_CS);

		for( int i=0; i<8; i++ )
		{
			P_AUTH_DATA pData = m_GGAuthBuffer.front();

			if( pData )
			{
				if( i == 0 && pData->packetSeq != 1 )
				{
					//sc::writeLogDebug(sc::string::format("GetGGAuth30Data Start Seq ERROR - %1%", pData->packetSeq));
					break;
				}

				//sc::writeLogDebug(sc::string::format("GetGGAuth30Data : Seq %1%, PacketSize %2%", pData->packetSeq, pData->packetSize));

				if( pData->packetSeq != (i+1) )
				{
					 sc::writeLogError(sc::string::format("GetGGAuth30Data Start Seq ERROR %1%", pData->packetSeq));
					 nRet = 0;
					 break;
				}

				if( i+1 != pData->packetSeq )
				{
					sc::writeLogError(sc::string::format("GetGGAuth30Data Seq Match ERROR idx : %1%, packet seq %2%", i, pData->packetSeq));
					 nRet = 0;
					 break;
				}

				if( (pAD->packetSize != 0) && (pAD->packetSize != pData->packetSize) )
				{
					sc::writeLogError(sc::string::format("GetGGAuth30Data Packet Size ERROR idx : %1%, prePacketSize: %2%, curPacektSize %3%", i, pData->packetSize, pAD->packetSize));
					 nRet = 0;
					 break;
				}

				if( (nRet != 0) && (nRet != pData->Auth_Msg) )
				{
					sc::writeLogError(sc::string::format("GetGGAuth30Data Packet ERROR idx : %1%, nRet: %2%, Auth_Msg %3%", i, nRet, pData->Auth_Msg));
					 nRet = 0;
					 break;
				}

				memcpy(pAD->packet + (i*512), pData->packet, sizeof(BYTE)*512);

				pAD->bPass = pData->bPass;
				pAD->packetSize = pData->packetSize;
				pAD->packetSeq = pData->packetSeq;
				nRet = pData->Auth_Msg;
				SAFE_DELETE(pData);
				m_GGAuthBuffer.pop();
			}
			else
			{
				//sc::writeLogDebug(std::string("GetGGAuth30Data pDATA ERROR"));
			}
		}

		LeaveCriticalSection(&m_CS);

		return nRet;
	}
	else
	{
		//sc::writeLogDebug(sc::string::format("GetGGAuth30Data SIZE LESS THAN 8 - %1%",m_GGAuthBuffer.size()));
		return 0;
	}
}

HRESULT DxGlobalStage::ChangeWndMode ()
{
	if (RANPARAM::bScrWndFullSize)
        ToRestoreSize();
	else
        ToFullSize();
	return S_OK;
}

HRESULT DxGlobalStage::ToFullSize ()
{
	if ( !RANPARAM::bScrWindowed )	return S_FALSE;

	RANPARAM::bScrWndFullSize = TRUE;
	
	LONG nWindowStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	nWindowStyle = WS_POPUP | WS_VISIBLE;
	SetWindowLong ( m_hWnd, GWL_STYLE, nWindowStyle );

	int nScreenX = (int) GetSystemMetrics(SM_CXSCREEN);
	int nScreenY = (int) GetSystemMetrics(SM_CYSCREEN);

	// 와이드 스크린을 넘어서는 비율일 경우
	//			2 스크린으로 인식.
	float fScreen = nScreenX / float(nScreenY);
	if ( fScreen > (16.0f/9.0f) )
	{
		nScreenX = nScreenX/2;
	}

	MoveWindow ( m_hWnd, 0, 0, nScreenX, nScreenY, TRUE );

	return S_OK;
}

HRESULT DxGlobalStage::ToRestoreSize ()
{
	//if ( !RANPARAM::bScrWndHalfSize )	return S_FALSE;	// 필요없는 데이터
	if ( !RANPARAM::bScrWindowed )		return S_FALSE;

	RANPARAM::bScrWndFullSize = FALSE;

	LONG nWindowStyle = WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION |
						WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_CLIPCHILDREN;
	SetWindowLong ( m_hWnd, GWL_STYLE, nWindowStyle );

	int wndSizeX = (int) RANPARAM::dwScrWidth;
	int wndSizeY = (int) RANPARAM::dwScrHeight;

	int nScreenX = (int) GetSystemMetrics(SM_CXSCREEN);
	int nScreenY = (int) GetSystemMetrics(SM_CYSCREEN);

	if ( wndSizeX >= nScreenX || wndSizeY >= nScreenY )
	{
		wndSizeX = 1024;
		wndSizeY = 768;

		if ( wndSizeX >= nScreenX || wndSizeY >= nScreenY )
		{
			wndSizeX = nScreenX-40;
			wndSizeY = nScreenY-60;
		}
	}

	RECT rt = {0, 0, wndSizeX, wndSizeY};
	AdjustWindowRectEx(&rt, nWindowStyle, FALSE, WS_EX_APPWINDOW);

	wndSizeX = rt.right - rt.left;
	wndSizeY = rt.bottom - rt.top;
	INT nX = (nScreenX - wndSizeX) / 2;
	INT nY = (nScreenY - wndSizeY) / 2;

	MoveWindow ( m_hWnd, nX, nY, wndSizeX, wndSizeY, TRUE );
	
	return S_OK;
}

HRESULT DxGlobalStage::ToChangeWndStyle()
{
	int wndSizeX = (int) DXPARAMSET::GetInstance().m_dwScrWidth;
	int wndSizeY = (int) DXPARAMSET::GetInstance().m_dwScrHeight;

	if ( wndSizeX < 1024 || wndSizeY < 768 )
	{
		LONG nWindowStyle = WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION | 
			WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN;
		SetWindowLong( m_hWnd, GWL_STYLE, nWindowStyle );

	}
	else
	{
		LONG nWindowStyle = WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION | 
			WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_CLIPCHILDREN;
		SetWindowLong( m_hWnd, GWL_STYLE, nWindowStyle );
	}

	return S_OK;
}

void DxGlobalStage::SetD3DApp( CD3DApplication * pD3DApp )
{
	// 이 함수가 헤더에 존재하면 대입 연산이 이루어 지지 않는다
	// 갑자기 왜 그런걸까...? 싱글톤? 컴파일 옵션? 최적화?
	// 정확한 이유를 모르겠다.
	m_pD3DApp = pD3DApp;
}

CD3DApplication* DxGlobalStage::GetD3DApp ()
{
	return m_pD3DApp;
}

void DxGlobalStage::SetUserCountry(wb::EM_COUNTRY Country)
{
	//!! SF_TODO
    //if (m_pOutInterface)
    //    m_pOutInterface->SetCountry(Country);
}

void DxGlobalStage::ShowCountryClubCountInSelectCountryWindow()
{
	//!! SF_TODO
    //if ( m_pOutInterface )
    //    m_pOutInterface->ShowCountryClubCountInSelectCountryWindow();
}

void DxGlobalStage::ReloadClubListInSelectClubWindow()
{
	//!! SF_TODO
    //if ( m_pOutInterface )
    //    m_pOutInterface->ReloadClubList();    
}

EMGFX_FONT_LANG DxGlobalStage::GetTextLanguage()
{
	if( "kr" == RANPARAM::strCountry )
	{
		return EMGFX_FONT_KOREAN;
	}
	else if( "cn" == RANPARAM::strCountry ||
			 "ml" == RANPARAM::strCountry ||
			 "tw" == RANPARAM::strCountry )
	{
		return EMGFX_FONT_CHINESE;
	}
	else if( "th" == RANPARAM::strCountry )
	{
		return EMGFX_FONT_THAI;
	}
	else
	{
		return EMGFX_FONT_ENGLISH;
	}
}

void DxGlobalStage::DoModalOutInterface(
    const std::string& strText,
    int nModalTitle,
    int nModalType,
    UIGUID CallerID,
    BOOL bHide)
{
	//!! SF_TODO
    //if (m_pOutInterface)
    //    m_pOutInterface->DoModal(strText, nModalTitle, nModalType, CallerID, bHide);
}

BOOL DxGlobalStage::GetUsedMaterialSystem()
{
	if ( !m_pActiveStage )
		return FALSE;

	return m_pActiveStage->GetUsedMaterialSystem();
}
/*
DWORD DxGlobalStage::GetMouseState( DX_MOUSEKEYS MKey )
{
}*/
