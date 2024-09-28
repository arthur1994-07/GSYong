#include "stdafx.h"
#include "../EngineLib/DxResponseMan.h"
#include "../EngineLib/GUInterface/Cursor.h"
#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/DxTools/DxPostProcess.h"
#include "../EngineLib/Common/CommonWeb.h"
#include "../EngineLib/Common/SUBPATH.h"
#include "../EngineLib/Common/profile.h"
#include "../EngineLib/DxTools/D3DFont.h"
#include "../EngineLib/DxTools/DxFontMan.h"
#include "../EngineLib/DxTools/DxGlowMan.h"
#include "../EngineLib/DxTools/DxShadowMap.h"
#include "../EngineLib/DxTools/DxViewPort.h"
#include "../EngineLib/DxTools/Light/DxLightMan.h"
#include "../EngineLib/DxTools/DxEnvironment.h"
#include "../EngineLib/DxTools/DxInputDevice.h"
#include "../EngineLib/DxTools/DebugSet.h"
#include "../EngineLib/DxEffect/Frame/DxEffectMan.h"
#include "../EngineLib/DxEffect/NSEffectThread.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"
#include "../EngineLib/DxMeshs/SkinMesh/NSSkinAniThread.h"
#include "../EngineLib/PhysX/NSPhysX.h"
#include "../EngineLib/GlobalParam.h"

#include "../RanLogic/Util/s_CLogSystem.h"
#include "../RanLogic/RANPARAM.h"
#include "../RanLogic/GLogicDataMan.h"

#include "../RanLogicClient/Char/NSGradeScriptMan.h"
#include "../RanLogicClient/Stage/DxGlobalStage.h"
#include "../RanLogicClient/GLGaeaClient.h"
#include "../RanLogicClient/Command/dxincommand.h"
#include "../RanLogicClient/Tool/DxParamSet.h"
#include "../RanLogicClient/LoadingThread/LoadingDirectClient.h"

#include "../RanLogicServer/FieldServer/GLGaeaServer.h"
#include "../RanLogicServer/DxServerInstance.h"

#include "Emulator.h"
#include "EmulatorDoc.h"
#include "EmulatorView.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

class DxSetConsoleMsg : public DxConsoleMsg
{
public:
	virtual void Write ( int nType, const char* szFormat, ... )
	{
		va_list argList;
		char szBuffer[1024];

		va_start(argList, szFormat);
		StringCbVPrintf(szBuffer, 1024, szFormat, argList);
		va_end(argList);

		CDebugSet::ToListView ( szBuffer );
	}

	virtual void Write( const TCHAR* msg, ... )
	{
		if (msg == NULL) return;

		TCHAR szBuf[C_BUFFER_SIZE+1] = {0};	

		va_list ap;
		va_start(ap, msg);
		StringCchVPrintf(szBuf, C_BUFFER_SIZE+1, msg, ap);
		va_end(ap);	

		CDebugSet::ToListView ( szBuf );
	}

	static DxSetConsoleMsg& GetInstance();
};

DxSetConsoleMsg& DxSetConsoleMsg::GetInstance()
{
	static DxSetConsoleMsg Instance;
	return Instance;
}

void CEmulatorView::OnInitialUpdate()
{
	__super::OnInitialUpdate();

	m_strCharSetFile = "class1.charset";
    m_strMapList = GLogicData::GetInstance().GetMapListFileName().c_str(); //"mapslist.mst";
}

void CEmulatorView::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CD3DApplication::Cleanup3DEnvironment();

	::SetCursor ( NULL );
	::DestroyCursor ( m_hCursorDefault );

	__super::PostNcDestroy();
}

BOOL CEmulatorView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	HCURSOR hCursl = CCursor::GetInstance().GetCurCursor();
	if ( !hCursl )
	{
		if ( !m_hCursorDefault )
			m_hCursorDefault = LoadCursor ( NULL, IDC_ARROW );
		hCursl = m_hCursorDefault;
	}

	if ( hCursl )
		::SetCursor ( hCursl );

	return TRUE;
}

HRESULT CEmulatorView::ConfirmDevice ( D3DCAPSQ* pCaps, DWORD dwBehavior, D3DFORMAT Format )
{
	if ( dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING )
		return E_FAIL;

	return S_OK;
}

HRESULT CEmulatorView::OneTimeSceneInit()
{    
	HRESULT hr;

	CEmulatorApp* pApp = (CEmulatorApp*) AfxGetApp();

    COMMON_WEB::Create((CWnd*) this, TSTRING(pApp->m_szAppPath));

	// 각종 장치들
	hr = m_pEngineDevice->OneTimeSceneInit(
        pApp->m_szAppPath,
        this,
		RANPARAM::strFontType,
        true,
        RENDERPARAM::emLangSet,
        RANPARAM::strGDIFont );

	if( FAILED( hr ) )
	{
		MessageBox( _T( "m_pEngineDevice->OneTimeSceneInit" ), _T( "Error" ), MB_OK );
		return E_FAIL;
	}

    // Note : 인터페이스 스킬 텍스트 로딩
	TCHAR szFullPath[MAX_PATH] = {0};
    const SUBPATH* pPath = pApp->GetSubPath();
	StringCchCopy( szFullPath, MAX_PATH, pApp->m_szAppPath );
	StringCchCat( szFullPath, MAX_PATH, pPath->GuiFileRoot().c_str()); //SUBPATH::GUI_FILE_ROOT);
	CGameTextMan::GetInstance().SetPath(szFullPath);

	// [2014.08.26][shhan] DxServerInstance::FieldCreate 안에서 호출되므로 주석처리한다.
	//
	//CGameTextMan::GetInstance().LoadText(RANPARAM::strGameWord.GetString(),   CGameTextMan::EM_GAME_WORD, TRUE, RANPARAM::strCountry);
	//CGameTextMan::GetInstance().LoadText(RANPARAM::strGameInText.GetString(), CGameTextMan::EM_GAME_IN_TEXT, TRUE , RANPARAM::strCountry);
	//CGameTextMan::GetInstance().LoadText(RANPARAM::strGameExText.GetString(), CGameTextMan::EM_GAME_EX_TEXT, TRUE , RANPARAM::strCountry);

	// 가이아 서버 생성.
	RANPARAM::SETUSERID("userid");

	hr = DxServerInstance::FieldCreate(
        m_pGaeaServer,
        pApp->m_szAppPath,
        (DxMsgServer*) m_pMsgServer,        
        NULL,
		NULL,
        SP_OFFICE_TEST,
        pApp->GetSubPath(),
        m_strMapList.GetString(),
        FIELDSERVER_MAX,
        1000,
        true,
        0,
        FALSE,        
        0,
		RENDERPARAM::emLangSet,
        // language::KOREAN,
        false,
        false,
		NULL);	

	if( FAILED( hr ) )
	{
		MessageBox( _T( "DxServerInstance::FieldCreate" ), _T( "Error" ), MB_OK );
        return E_FAIL;
	}
    
    m_pEmulServerField = new EmulServerField(m_pGaeaServer);
	m_pGlobalStage->SetEmulMsgField(m_pEmulServerField);

	// 게임 스테이지
	m_pGlobalStage->SetD3DApp(this);
	m_pGlobalStage->OneTimeSceneInit(
        pApp->m_szAppPath,
        m_hWndApp,
        TRUE,
		static_cast<WORD>(RANPARAM::dwScrWidth),
        static_cast<WORD>(RANPARAM::dwScrHeight),
		m_strMapList.GetString(),
        false);

	return S_OK;
}

HRESULT CEmulatorView::CreateObjects()
{
	{ // Note : 로딩시 텍스트를 출력하기 위해서 폰트와 텍스트 부분을 먼저 초기화 했다.

		// 폰트 메니져 초기화.
		DxFontMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
		CD3DFontPar* pD3dFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		CD3DFontPar* pD3dFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW|D3DFONT_ASCII );

		//	Note	:	디버그셋 경로 설정 및 초기화
		CDebugSet::InitDeviceObjects( pD3dFont9 );
	}

	HRESULT hr = S_OK;

    LoadingDirectClient::s_pd3dDevice = m_pd3dDevice;
    LoadingDirectClient::s_hWnd = m_hWnd;
    LoadingDirectClient::s_pGaeaClient = m_pGlobalStage->GetGaeaClient();

	hr = InitDeviceObjects();
    if( FAILED(hr) )
		return hr;

	hr = RestoreDeviceObjects();	
    if( FAILED(hr) )
		return hr;

	return hr;
}

HRESULT CEmulatorView::InitDeviceObjects()
{
    {
		SCHARDATA2 CharData2;
		CharData2.Assign ( GLCONST_CHAR::GET_CHAR_DATA2(1,GLCI_ARMS_M) );

		if (!m_strCharSetFile.IsEmpty())
		{
            std::string CharSetFile(m_strCharSetFile.GetString());
            if (FALSE == CharData2.LOADFILE(CharSetFile))
			{
				MessageBox( _T( "!CharData2.LOADFILE" ), _T( "Error" ), MB_OK );
				return E_FAIL;
			}
		}

		CharData2.m_cInventory.SetItemGenTime();

		if( m_bEnterGmLevel )
			CharData2.SetUserLevel(USER_MASTER);

		CharData2.SetName("KALUNG GSOnline ");

		m_pServerChar = m_pGaeaServer->CreateCharacter(&CharData2, 0, 0, GLPartyID(), wb::UNKNOWN_COUNTRY, 0, FALSE);
		if ( m_pServerChar )
		{
			// 클라이언트에 전송 : 캐릭터정보 + 스킬 + 아이템
			m_pServerChar->MsgGameJoin ();            
			m_pGlobalStage->MsgProcessFrame ();
		}
		else
		{
			MessageBox ( "Server Activate Charactor Create fail.", "ERROR", MB_OK|MB_ICONEXCLAMATION );
		}
	}

	// 각종 장치들.
	//
	m_pEngineDevice->InitDeviceObjects ( m_pd3dDevice );

	DXLIGHT sDirectional;
	sDirectional.SetDefault();
	sDirectional.m_Light.Diffuse = D3DXCOLOR(0,0,0,0);
	sDirectional.m_Light.Ambient = D3DXCOLOR(0,0,0,0);
	DxLightMan::SetDefDirect ( sDirectional );

	TextureManager::GetInstance().EnableTextureAlphaTypeDebug();

	// 게임 스테이지.
	//
	m_pGlobalStage->InitDeviceObjects ( m_pd3dDevice );

	// 키보드 마우스.
	//
	CCursor::GetInstance().InitDeviceObjects ();

    // Loading Script
    {
        CEmulatorApp* pApp = (CEmulatorApp*) AfxGetApp();
        NSGradeScriptMan::LoadScript( pApp->GetSubPath() );
    }

	return S_OK;
}

HRESULT CEmulatorView::RestoreDeviceObjects()
{
	HRESULT hr=S_OK;

	// 각종 장치들.
	//
	m_pEngineDevice->RestoreDeviceObjects ();

	// 게임 스테이지.
	//
	m_pGlobalStage->RestoreDeviceObjects();
	DXPARAMSET::INIT ();

	// 글로우 처리를 하기위해 사용 한다.
	DxGlowMan::GetInstance().SetProjectActiveON();
	DxPostProcess::GetInstance().SetProjectActiveON();

	//	DEBUG : 프로파일링 초기화.
	PROFILE_INIT();

	return S_OK;
}

HRESULT CEmulatorView::FrameMove3DEnvironment ()
{
	HRESULT hr=S_OK;

	PROFILE_BLOCKSTART();

	if ( m_pd3dDevice )
	{
		hr = CD3DApplication::FrameMove3DEnvironment ();
		if ( FAILED(hr) )	return hr;
	}

	PROFILE_BLOCKEND();
	PROFILE_DUMPOUTPUT();

	return S_OK;
}

HRESULT CEmulatorView::Render3DEnvironment ()
{
	HRESULT hr=S_OK;

	PROFILE_BLOCKSTART();

	if ( m_pd3dDevice )
	{
		hr = CD3DApplication::Render3DEnvironment ();
		if ( FAILED(hr) )	return hr;
	}

	PROFILE_BLOCKEND();
	PROFILE_DUMPOUTPUT();

	return S_OK;
}

HRESULT CEmulatorView::FrameMove( BOOL bNotRendering )
{
	PROFILE_BEGIN("FrameMove");

	//	float 정확도 향상
	CheckControlfp();

	GLPeriod::GetInstance().UpdateWeather ( m_fElapsedTime );
	m_pGaeaServer->FrameMove(m_fTime, m_fElapsedTime);

	// 클라이언트 위치와 서버 위치 동기화 테스트
	{
		GLGaeaClient* pGaeaClient = m_pGlobalStage->GetGaeaClient();
		if( pGaeaClient && m_pServerChar )
		{
			GLCharacter* pClientChar = pGaeaClient->GetCharacter();
			if( pClientChar )
				pClientChar->SetServerPos( m_pServerChar->GetPosition() );
		}
	}

	// 뷰포트
	//
	DxViewPort::GetInstance().FrameMove ( m_fTime, m_fElapsedTime );

	// 키보드 조작으로 RenderState 변경해볼때 사용하는 코드
	GLOBAL_PARAM::ChangeState( &DxInputDevice::GetInstance() );

	// 각종 장치들
	m_pEngineDevice->FrameMoveFirst(m_fTime, m_fElapsedTime);

	// 게임 스테이지.
	//
	m_pGlobalStage->FrameMove(m_fTime, m_fElapsedTime, bNotRendering);

	// 각종 장치들
	m_pEngineDevice->FrameMoveSecond(m_fTime, m_fElapsedTime, m_bDefWin, TRUE);

	//////////////////////////////////////////////////////////////////////////
	// NSSkinAniThread
	// TAG_ProcessAnimationThread_3
	// 모든 정보가 COMPLETE_DATA 될 때 까지 기다리도록 한다.
	NSSkinAniThread::Wait_CompleteSkinAniControl( m_fElapsedTime );

	// NSEffectThread 대기
	NSEffectThread::Wait_FrameMove( m_pd3dDevice, m_fElapsedTime );

	PROFILE_END("FrameMove");

	return S_OK;
}

HRESULT CEmulatorView::Render()
{
	PROFILE_BEGIN("Render");

	//	Fog 셋팅
	D3DCOLOR colorClear = D3DCOLOR_XRGB(89,135,179);
	DxFogMan::GetInstance().RenderFogSB ( m_pd3dDevice, FALSE );
	colorClear = DxFogMan::GetInstance().GetFogColor();

	//// Begin the scene
	if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
	{
			// Glow 
		DxGlowMan::GetInstance().RenderTex ( m_pd3dDevice );

		// Light 설정.
		//
		DxLightMan::GetInstance()->Render ( m_pd3dDevice, FALSE, m_pGlobalStage->GetUsedMaterialSystem() );

		// 버텍스 쉐이더 콘스탄트 설정.
		//
		DXLIGHT &Light = *DxLightMan::GetInstance()->GetDirectLight ();
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTDIRECT, (float*)&Light.m_Light.Direction, 1 );
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTDIFFUSE, (float*)&Light.m_Light.Diffuse, 1 );
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTAMBIENT, (float*)&Light.m_Light.Ambient, 1 );

		D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt ();
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_CAMERAPOSITION, (float*)&vFromPt, 1 );
		
		// 포인트 라이트 VertexShaderConstant
		//
		D3DLIGHTQ	pLight;
		D3DXVECTOR4	vPointPos;
		D3DXVECTOR3	vPointDiff;
		for ( int i=0; i<7; i++ )
		{
			if ( DxLightMan::GetInstance()->GetClosedLight(i+1) )
			{
				pLight = DxLightMan::GetInstance()->GetClosedLight(i+1)->m_Light;
				vPointDiff = D3DXVECTOR3 ( pLight.Diffuse.r, pLight.Diffuse.g, pLight.Diffuse.b );
				vPointPos.x = pLight.Position.x;
				vPointPos.y = pLight.Position.y;
				vPointPos.z = pLight.Position.z;
				vPointPos.w = pLight.Range;
			}
			else
			{
				vPointPos = D3DXVECTOR4 ( 0.f, 0.f, 0.f, 0.1f );
				vPointDiff = D3DXVECTOR3 ( 0.f, 0.f, 0.f );
			}
			m_pd3dDevice->SetVertexShaderConstantF (i*2+VSC_PLIGHTPOS01, (float*)&vPointPos, 1);
			m_pd3dDevice->SetVertexShaderConstantF (i*2+VSC_PLIGHTDIFF01, (float*)&vPointDiff, 1);
		}

		D3DXMATRIX matView = DxViewPort::GetInstance().GetMatView();
		D3DXMATRIX matProj = DxViewPort::GetInstance().GetMatProj();

		m_pd3dDevice->SetVertexShaderConstantF ( VSC_SKIN_DEFAULT, (float*)&D3DXVECTOR4 (1.f, 0.5f, 0.f, 765.01f), 1 );

		D3DXVECTOR3	vLightVector = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction;
		D3DXVec3TransformNormal ( &vLightVector, &vLightVector, &matView );
		D3DXVec3Normalize ( &vLightVector, &vLightVector);
		vLightVector = -vLightVector;
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTDIRECT_VIEW, (float*)&vLightVector, 1 );

		D3DXMatrixTranspose( &matView, &matView );
		D3DXMatrixTranspose( &matProj, &matProj );

		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );		// 강제 설정

		// PhysX 물리엔진 적용
		NSPhysX::Render( m_fElapsedTime );

		// 게임 스테이지.
		m_pGlobalStage->Render( colorClear );

        // 웹 페이지 렌더링
		COMMON_WEB::Render();

		RenderText();

		// 커서 그리기.
		//
		CCursor::GetInstance().Render ( m_pd3dDevice, DxInputDevice::GetInstance().GetMouseLocateX(), DxInputDevice::GetInstance().GetMouseLocateY() );

		// End the scene.
		m_pd3dDevice->EndScene();
	}

	PROFILE_END("Render");
	return S_OK;
}

HRESULT CEmulatorView::RenderText()
{
	if (!dxincommand::bDISP_CONSOLE && !dxincommand::bDISP_FPS)
        return S_OK;

	D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,255);
	D3DCOLOR fontWarningColor = D3DCOLOR_ARGB(255,0,255,255);
	TCHAR szMsg[MAX_PATH] = TEXT("");

	// Output display stats
	CD3DFontPar* pD3dFont = DxFontMan::GetInstance().FindFont(_DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG);
	if (!pD3dFont)
        return S_OK;

	if ( dxincommand::bDISP_FPS )
	{
		// Output display stats
		FLOAT fNextLine = 40.0f; 

		lstrcpy( szMsg, m_strDeviceStats );
		fNextLine -= 20.0f;
		pD3dFont->DrawText( 2, fNextLine, fontColor, szMsg );

		lstrcpy( szMsg, m_strFrameStats );
		fNextLine -= 20.0f;
		pD3dFont->DrawText( 2, fNextLine, fontColor, szMsg );
	}

	if ( dxincommand::bDISP_CONSOLE )
	{
		CDebugSet::Render ();
	}

	GLOBAL_PARAM::StateDisplay( pD3dFont );

    return S_OK;
}

HRESULT CEmulatorView::InvalidateDeviceObjects()
{
    // 각종 장치들.
	m_pEngineDevice->InvalidateDeviceObjects();
	
    // 게임 스테이지.
	m_pGlobalStage->InvalidateDeviceObjects();
	return S_OK;
}

HRESULT CEmulatorView::DeleteDeviceObjects()
{
#ifdef _DEBUG
	_CrtCheckMemory();
#endif //_DEBUG

    // 각종 장치들.
	m_pEngineDevice->DeleteDeviceObjects ();

	// 게임 스테이지.
	m_pGlobalStage->DeleteDeviceObjects();

	// 마우스 커서.
	CCursor::GetInstance().DeleteDeviceObjects ();

	return S_OK;
}

HRESULT CEmulatorView::FinalCleanup()
{
	m_pServerChar = NULL;

	DxServerInstance::FieldCleanUp( m_pGaeaServer );
	
    // 게임 스테이지.
	m_pGlobalStage->FinalCleanup();

	// 각종 장치들.
	m_pEngineDevice->FinalCleanup();

	return S_OK;
}

void CEmulatorView::SetActive ( BOOL bActive )
{
	DxInputDevice::GetInstance().OnActivate ( bActive );
}

void CEmulatorView::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);    

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if ( m_bCreated )
	{
		ReSizeWindow(cx,cy);

		if (m_pd3dDevice)
		{
			m_pGlobalStage->ReSizeWindow(cx, cy);
		}
	}
}

void CEmulatorView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CEmulatorApp *pApp = (CEmulatorApp*) AfxGetApp();
	pApp->SetActive ( bActivate );
	CEmulatorView::SetActive ( bActivate );

	__super::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CEmulatorView::OnAppPlay()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CEmulatorApp *pApp = (CEmulatorApp *) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();
    std::string FullPath(pApp->m_szAppPath);
	FullPath.append(pPath->GLogicPath()); //SUBPATH::GLOGIC_FILE);
	GLOGIC::SetPath(FullPath.c_str());

    TCHAR szfileName[MAX_PATH] = {0};

    const TCHAR* CATAGORY_CHARSETFILE = _T("CharSetFile");
    const TCHAR* CATAGORY_MAPFILE = _T("MapFile");
    const TCHAR* CATAGORY_GM = _T("GM");

    if (m_bAuto && Load(CATAGORY_CHARSETFILE, _T("%s"), szfileName, MAX_PATH) > 0)
    {
        m_strCharSetFile = szfileName;
    }
    else
	{
		// 파일 필터를 정의한다.
		CString szFilter = "charset file  |*.charset|";

		// 파일 오픈 다이알로그를 만듬.
		CFileDialog dlg ( TRUE,".charset",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this );

		dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
		if ( dlg.DoModal() != IDOK )
			return;

		m_strCharSetFile = dlg.GetFileName();
        Save(CATAGORY_CHARSETFILE, _T("%s"), m_strCharSetFile.GetBuffer());
	}

    if (m_bAuto && Load(CATAGORY_MAPFILE, _T("%s"), szfileName, MAX_PATH) > 0)
    {
        m_strMapList = szfileName;
    }
    else
	{
		// 파일 필터를 정의한다.
		CString szFilter = "Maps List Setting File (*.luc)|*.luc|";

		// 파일 오픈 다이알로그를 만듬.
		CFileDialog dlg ( TRUE,".luc",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this );

		dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
		if ( dlg.DoModal() != IDOK )
			return;

		m_strMapList = dlg.GetFileName();
        Save(CATAGORY_MAPFILE, _T("%s"), m_strMapList.GetBuffer());
	}

    int nBool = 0;

    if (m_bAuto && Load(CATAGORY_GM, _T("%d"), &nBool) > 0)
    {
        nBool == 0 ? m_bEnterGmLevel = true : m_bEnterGmLevel = false;
    }
    else
	{
		if (IDYES == MessageBox("GM Level 로 게임에 접속하시겠습니까?", "주의", MB_YESNO))
			m_bEnterGmLevel = true;

        Save(CATAGORY_GM, _T("%d"), m_bEnterGmLevel ? 1 : 0);
	}

	RANPARAM::LOAD(pApp->m_szAppPath, pApp->GetServiceProvider(), false, false);
	DXPARAMSET::INIT();

	CD3DApplication::SetScreen(
        RANPARAM::dwScrWidth,
        RANPARAM::dwScrHeight,
        RANPARAM::emScrFormat, 
		RANPARAM::uScrRefreshHz,
        TRUE);

	if (FAILED(CD3DApplication::Create(m_hWnd, m_hWnd, AfxGetInstanceHandle())))
	{
		MessageBox( _T( "CD3DApplication::Create" ), _T( "Error" ), MB_OK );
        return;
	}
	
	m_bCreated = TRUE;
}

BOOL CEmulatorView::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	MSG msg;
	msg.hwnd = m_hWnd;
	msg.message = message;
	msg.wParam = wParam;
	msg.lParam = lParam;

	m_pGlobalStage->MsgProc(&msg);

	if( message == WM_SYSKEYDOWN && wParam == VK_F4 )
	{
		return TRUE;
	}

	return __super::OnWndMsg(message, wParam, lParam, pResult);
}

void CEmulatorView::OnUpdateMenuAuto(CCmdUI *pCmdUI)
{
    if (pCmdUI)
    {
        pCmdUI->SetCheck(m_bAuto ? 1 : 0);
    }
}

void CEmulatorView::OnMenuAuto()
{
    m_bAuto = !m_bAuto;
    Save(_T("AutoPlay"), _T("%d"), m_bAuto ? 1 : 0);
}
