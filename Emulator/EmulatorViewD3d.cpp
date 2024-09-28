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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
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

	// ���� ��ġ��
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

    // Note : �������̽� ��ų �ؽ�Ʈ �ε�
	TCHAR szFullPath[MAX_PATH] = {0};
    const SUBPATH* pPath = pApp->GetSubPath();
	StringCchCopy( szFullPath, MAX_PATH, pApp->m_szAppPath );
	StringCchCat( szFullPath, MAX_PATH, pPath->GuiFileRoot().c_str()); //SUBPATH::GUI_FILE_ROOT);
	CGameTextMan::GetInstance().SetPath(szFullPath);

	// [2014.08.26][shhan] DxServerInstance::FieldCreate �ȿ��� ȣ��ǹǷ� �ּ�ó���Ѵ�.
	//
	//CGameTextMan::GetInstance().LoadText(RANPARAM::strGameWord.GetString(),   CGameTextMan::EM_GAME_WORD, TRUE, RANPARAM::strCountry);
	//CGameTextMan::GetInstance().LoadText(RANPARAM::strGameInText.GetString(), CGameTextMan::EM_GAME_IN_TEXT, TRUE , RANPARAM::strCountry);
	//CGameTextMan::GetInstance().LoadText(RANPARAM::strGameExText.GetString(), CGameTextMan::EM_GAME_EX_TEXT, TRUE , RANPARAM::strCountry);

	// ���̾� ���� ����.
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

	// ���� ��������
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
	{ // Note : �ε��� �ؽ�Ʈ�� ����ϱ� ���ؼ� ��Ʈ�� �ؽ�Ʈ �κ��� ���� �ʱ�ȭ �ߴ�.

		// ��Ʈ �޴��� �ʱ�ȭ.
		DxFontMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
		CD3DFontPar* pD3dFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		CD3DFontPar* pD3dFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW|D3DFONT_ASCII );

		//	Note	:	����׼� ��� ���� �� �ʱ�ȭ
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
			// Ŭ���̾�Ʈ�� ���� : ĳ�������� + ��ų + ������
			m_pServerChar->MsgGameJoin ();            
			m_pGlobalStage->MsgProcessFrame ();
		}
		else
		{
			MessageBox ( "Server Activate Charactor Create fail.", "ERROR", MB_OK|MB_ICONEXCLAMATION );
		}
	}

	// ���� ��ġ��.
	//
	m_pEngineDevice->InitDeviceObjects ( m_pd3dDevice );

	DXLIGHT sDirectional;
	sDirectional.SetDefault();
	sDirectional.m_Light.Diffuse = D3DXCOLOR(0,0,0,0);
	sDirectional.m_Light.Ambient = D3DXCOLOR(0,0,0,0);
	DxLightMan::SetDefDirect ( sDirectional );

	TextureManager::GetInstance().EnableTextureAlphaTypeDebug();

	// ���� ��������.
	//
	m_pGlobalStage->InitDeviceObjects ( m_pd3dDevice );

	// Ű���� ���콺.
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

	// ���� ��ġ��.
	//
	m_pEngineDevice->RestoreDeviceObjects ();

	// ���� ��������.
	//
	m_pGlobalStage->RestoreDeviceObjects();
	DXPARAMSET::INIT ();

	// �۷ο� ó���� �ϱ����� ��� �Ѵ�.
	DxGlowMan::GetInstance().SetProjectActiveON();
	DxPostProcess::GetInstance().SetProjectActiveON();

	//	DEBUG : �������ϸ� �ʱ�ȭ.
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

	//	float ��Ȯ�� ���
	CheckControlfp();

	GLPeriod::GetInstance().UpdateWeather ( m_fElapsedTime );
	m_pGaeaServer->FrameMove(m_fTime, m_fElapsedTime);

	// Ŭ���̾�Ʈ ��ġ�� ���� ��ġ ����ȭ �׽�Ʈ
	{
		GLGaeaClient* pGaeaClient = m_pGlobalStage->GetGaeaClient();
		if( pGaeaClient && m_pServerChar )
		{
			GLCharacter* pClientChar = pGaeaClient->GetCharacter();
			if( pClientChar )
				pClientChar->SetServerPos( m_pServerChar->GetPosition() );
		}
	}

	// ����Ʈ
	//
	DxViewPort::GetInstance().FrameMove ( m_fTime, m_fElapsedTime );

	// Ű���� �������� RenderState �����غ��� ����ϴ� �ڵ�
	GLOBAL_PARAM::ChangeState( &DxInputDevice::GetInstance() );

	// ���� ��ġ��
	m_pEngineDevice->FrameMoveFirst(m_fTime, m_fElapsedTime);

	// ���� ��������.
	//
	m_pGlobalStage->FrameMove(m_fTime, m_fElapsedTime, bNotRendering);

	// ���� ��ġ��
	m_pEngineDevice->FrameMoveSecond(m_fTime, m_fElapsedTime, m_bDefWin, TRUE);

	//////////////////////////////////////////////////////////////////////////
	// NSSkinAniThread
	// TAG_ProcessAnimationThread_3
	// ��� ������ COMPLETE_DATA �� �� ���� ��ٸ����� �Ѵ�.
	NSSkinAniThread::Wait_CompleteSkinAniControl( m_fElapsedTime );

	// NSEffectThread ���
	NSEffectThread::Wait_FrameMove( m_pd3dDevice, m_fElapsedTime );

	PROFILE_END("FrameMove");

	return S_OK;
}

HRESULT CEmulatorView::Render()
{
	PROFILE_BEGIN("Render");

	//	Fog ����
	D3DCOLOR colorClear = D3DCOLOR_XRGB(89,135,179);
	DxFogMan::GetInstance().RenderFogSB ( m_pd3dDevice, FALSE );
	colorClear = DxFogMan::GetInstance().GetFogColor();

	//// Begin the scene
	if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
	{
			// Glow 
		DxGlowMan::GetInstance().RenderTex ( m_pd3dDevice );

		// Light ����.
		//
		DxLightMan::GetInstance()->Render ( m_pd3dDevice, FALSE, m_pGlobalStage->GetUsedMaterialSystem() );

		// ���ؽ� ���̴� �ܽ�źƮ ����.
		//
		DXLIGHT &Light = *DxLightMan::GetInstance()->GetDirectLight ();
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTDIRECT, (float*)&Light.m_Light.Direction, 1 );
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTDIFFUSE, (float*)&Light.m_Light.Diffuse, 1 );
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTAMBIENT, (float*)&Light.m_Light.Ambient, 1 );

		D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt ();
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_CAMERAPOSITION, (float*)&vFromPt, 1 );
		
		// ����Ʈ ����Ʈ VertexShaderConstant
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

		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );		// ���� ����

		// PhysX �������� ����
		NSPhysX::Render( m_fElapsedTime );

		// ���� ��������.
		m_pGlobalStage->Render( colorClear );

        // �� ������ ������
		COMMON_WEB::Render();

		RenderText();

		// Ŀ�� �׸���.
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
    // ���� ��ġ��.
	m_pEngineDevice->InvalidateDeviceObjects();
	
    // ���� ��������.
	m_pGlobalStage->InvalidateDeviceObjects();
	return S_OK;
}

HRESULT CEmulatorView::DeleteDeviceObjects()
{
#ifdef _DEBUG
	_CrtCheckMemory();
#endif //_DEBUG

    // ���� ��ġ��.
	m_pEngineDevice->DeleteDeviceObjects ();

	// ���� ��������.
	m_pGlobalStage->DeleteDeviceObjects();

	// ���콺 Ŀ��.
	CCursor::GetInstance().DeleteDeviceObjects ();

	return S_OK;
}

HRESULT CEmulatorView::FinalCleanup()
{
	m_pServerChar = NULL;

	DxServerInstance::FieldCleanUp( m_pGaeaServer );
	
    // ���� ��������.
	m_pGlobalStage->FinalCleanup();

	// ���� ��ġ��.
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

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CEmulatorApp *pApp = (CEmulatorApp*) AfxGetApp();
	pApp->SetActive ( bActivate );
	CEmulatorView::SetActive ( bActivate );

	__super::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CEmulatorView::OnAppPlay()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
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
		// ���� ���͸� �����Ѵ�.
		CString szFilter = "charset file  |*.charset|";

		// ���� ���� ���̾˷α׸� ����.
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
		// ���� ���͸� �����Ѵ�.
		CString szFilter = "Maps List Setting File (*.luc)|*.luc|";

		// ���� ���� ���̾˷α׸� ����.
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
		if (IDYES == MessageBox("GM Level �� ���ӿ� �����Ͻðڽ��ϱ�?", "����", MB_YESNO))
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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
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
