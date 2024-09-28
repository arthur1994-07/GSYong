#include "stdafx.h"

#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../EngineLib/DxTools/DxViewPort.h"
#include "../EngineLib/DxTools/DxInputDevice.h"
#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../EngineLib/DxMeshs/SkinMesh/NSSkinAniThread.h"
#include "../EngineLib/Common/SUBPATH.h"
#include "../EngineLib/DxTools/PostProcessing/NSBloomHDR.h"
#include "../EngineLib/DxTools/DxFontMan.h"
#include "../EngineLib/DxEffect/DxLoadShader.h"
#include "../EngineLib/DxMeshs/FrameMesh/DxReplaceContainer.h"
#include "../EngineLib/DxTools/Light/DxLightMan.h"
#include "../EngineLib/DxTools/Material/NSMaterialManager.h"
#include "../EngineLib/DxTools/Deffered/NSDeffered.h"
#include "../EngineLib/DxTools/DxRenderStates.h"
#include "../EngineLib/Water/NSWaterMain.h"
#include "../EngineLib/DxEffect/Frame/DxEffectMan.h"
#include "../EngineLib/DxTools/EditMeshs.h"
#include "../EngineLib/DxTools/DxShadowMap.h"
#include "../EngineLib/DxTools/DxFogMan.h"
#include "../EngineLib/DxTools/DxSkyMan.h"
#include "../EngineLib/DxTools/DebugSet.h"
#include "../EngineLib/DxTools/DxEditMat.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"
#include "../EngineLib/DxTools/DxSurfaceTex.h"
#include "../EngineLib/DxResponseMan.h"

#include "../RanLogic/RANPARAM.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogic/GLogicData.h"
#include "../RanLogic/Quest/GLQuestMan.h"
#include "../RanLogic/Crow/GLCrowDataMan.h"

#include "../RanLogicClient/Collision/NSSimpleCollisionClient.h"

#include "../RanLogicServer/DxServerInstance.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"
#include "../MfcExLib/Splasher.h"   //스플래쉬

#include "./Edit/LevelSheetWithTab.h"
#include "./Edit/LevelSingleEffect.h"
#include "./LevelEdit.h"
#include "./LevelMainFrm.h"
#include "./Edit/MobSetPage.h"
#include "./LevelEditDoc.h"
#include "./LevelEditView.h"


// 스플래쉬 화면을 출력하기위해 추가되었음
#ifndef LWA_ALPHA
#define LWA_ALPHA 0x00000002
#endif

extern BOOL g_bWIREFRAME_VIEW;
extern BOOL g_bVIEW_NAVIGATION_MESH_FOR_LEVELEDIT;
typedef std::tr1::shared_ptr<GLMobScheduleManServer> SpMobScheduleManServer;

class CMySplashWnd : public CSplashWnd
{
public:
    //Constructors / Destructors
    CMySplashWnd()
    {
        //Lets use an almost opaque splash window with some "cut-out" sections in it
        SetDropShadow();
        SetTransparent(RGB(255, 0, 255), LWA_ALPHA, 255);                       
    }

    DECLARE_DYNCREATE(CMySplashWnd);
};
IMPLEMENT_DYNCREATE(CMySplashWnd, CSplashWnd);
//////////////////////////////////////////////////////////////////////////

extern BOOL g_bFRAME_LIMIT;

BOOL CLevelEditView::m_bRENDQUAD = TRUE;

//DxLandMan CLevelEditView::m_LandMan;

_CUR_TYPE CLevelEditView::m_CurType = _CUR_SELECT;

HRESULT CLevelEditView::ConfirmDevice ( D3DCAPSQ* pCaps, DWORD dwBehavior, D3DFORMAT Format )
{
	if( dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING )
		return E_FAIL;

	return S_OK;
}


HRESULT CLevelEditView::SaveCsvFile( const char* szFile )
{
    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
        
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	std::fstream SFile;
	SFile.open(szFile, std::ios_base::out);

	if (!pGLLandMan->SaveCsvFile(SFile, false))
        return E_FAIL;

	SFile.close();

	return S_OK;
}

HRESULT CLevelEditView::LoadCsvFile( const char* szFile )
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();

	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if (!pGLLandMan->LoadCsvFile(szFile))
        return E_FAIL;

	m_spDxLandMan->CleanUp ();
	BOOL bOk = m_spDxLandMan->LoadFile ( pGLLandMan->GetWldFileName(), GetD3dDevice () );
	if ( bOk )
	{
		pGaeaServer->ClearDropObj();
		m_spDxLandMan->ActiveMap_Tool ( GetD3dDevice (), m_spDxLandMan );
	}

	// 우회 Navigation 이 있을 때만 작업한다.
	if ( !pGLLandMan->IsEmptyNaviBypassFileName() )
	{
		m_spDxLandMan->LoadNaviBypassFile( pGLLandMan->GetNaviBypassFileName() );
	}

	pFrame->m_DlgLevelEdit.UpdatePage ( TRUE );

	return S_OK;
}

BOOL CLevelEditView::ImportGLLandMan ( const char* szFile, BOOL bExceptEffect )
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();

	GLLandMan* const pLand(pGaeaServer->GetRootMap());
	if ( pLand->IsInit() == FALSE )
	{
		AfxMessageBox(_T("first, it needs to loaded at least one Level"), MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	pLand->ImportLayer(szFile, NULL, bExceptEffect);

	pFrame->m_DlgLevelEdit.UpdatePage ( TRUE );

	return TRUE;
}

BOOL CLevelEditView::LoadGLLandMan ( const char* szFile )
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();

	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	
	pGLLandMan->ReSetMap();
	pGLLandMan->LoadFile(szFile, NULL);

	m_spDxLandMan->CleanUp ();
	BOOL bOk = m_spDxLandMan->LoadFile ( pGLLandMan->GetWldFileName(), GetD3dDevice () );
	if ( bOk )
	{
		pGaeaServer->ClearDropObj ();
		m_spDxLandMan->ActiveMap_Tool ( GetD3dDevice (), m_spDxLandMan );
	}

	// 우회 Navigation 이 있을 때만 작업한다.
	if ( !pGLLandMan->IsEmptyNaviBypassFileName() )
	{
		m_spDxLandMan->LoadNaviBypassFile( pGLLandMan->GetNaviBypassFileName() );
	}

	pFrame->m_DlgLevelEdit.UpdatePage ( TRUE );

	return TRUE;
}

BOOL CLevelEditView::SaveGLLandMan ( const char* szFile )
{
    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	pGLLandMan->SaveFile ( szFile );

	return TRUE;
}


HRESULT CLevelEditView::OneTimeSceneInit()
{
    // 스플래쉬 화면을 출력하기위해 추가되었음
    CSplashFactory factory;
    
    if (!factory.Create(RUNTIME_CLASS(CMySplashWnd)))
    {
        AfxMessageBox(_T("Failed to create splash screen"), MB_OK | MB_ICONSTOP);
        return FALSE;
    }
    //////////////////////////////////////////////////////////////////////////


	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
    std::string FullPath;

	// 각종 장치들
	m_pEngineDevice->OneTimeSceneInit(
        pFrame->m_szAppPath,
        this,
		RANPARAM::strFontType,
        false,
        RENDERPARAM::emLangSet,
        RANPARAM::strGDIFont);
    
    CLevelEditApp* pApp = (CLevelEditApp*) AfxGetApp();
    SUBPATH* pPath = pApp->GetSubPath();
	//GMTOOL::Create(pFrame->m_szAppPath, pPath, pApp->GetServiceProvider());
    // 스플래쉬 화면을 출력하기위해 추가되었음
    GMTOOL::Create(pFrame->m_szAppPath, pPath, pApp->GetServiceProvider(), NULL,
        boost::bind(&CSplashFactory::DisplayMessage, &factory, _1),
        boost::bind(&CSplashFactory::DisplayProgress, &factory, _1));

	// 에디트 메트릭스 생성.
	//
	m_pDxEditmat = new DxEditMat;

	FullPath = pFrame->m_szAppPath;
	FullPath.append(pPath->ObjFileEdit()); //SUBPATH::OBJ_FILE_EDIT);
	m_pDxEditmat->SetParam(FullPath, m_hWnd);

	// 위치 지정 구 초기화.
	m_p3dPosMesh = new DxFrameMesh( TRUE );

	FullPath = pFrame->m_szAppPath;
	FullPath.append(pPath->ObjFileEdit()); //SUBPATH::OBJ_FILE_EDIT);
	m_p3dPosMesh->SetFile( FullPath.c_str(), "3dPos.x" );

	FullPath = pFrame->m_szAppPath;
	FullPath.append(pPath->ObjFileEdit()); //SUBPATH::OBJ_FILE_EDIT);
	m_EditBox.SetPath(FullPath.c_str());


    factory.Close();
	return S_OK;
}

HRESULT CLevelEditView::InitDeviceObjects()
{
    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();

	CLevelEditApp* pApp = (CLevelEditApp*) AfxGetApp();
	SUBPATH* pPath = pApp->GetSubPath();

	DxFontMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	CD3DFontPar* pD3dFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	//	Note	:	디버그셋 경로 설정 및 초기화
	CDebugSet::InitDeviceObjects( pD3dFont9 );

	// 각종 장치들.
	//
	m_pEngineDevice->InitDeviceObjects ( m_pd3dDevice );

	// 뷰포트.
	//
	DxViewPort::GetInstance().SetToolCamera();

	D3DXVECTOR3 vFromPt		= D3DXVECTOR3( 0.0f, 80.0f, -80.0f );
	D3DXVECTOR3 vLookatPt	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	DxViewPort::GetInstance().SetViewTrans ( vFromPt, vLookatPt, vUpVec );
    
	pGaeaServer->Create4Level ( m_pd3dDevice );
	pGaeaServer->SetUpdate ( FALSE );
	SetCurType ( _CUR_SEL_MOBSCH );

	m_spDxLandMan->InitDeviceObjects ( m_pd3dDevice );

	pGaeaServer->GetRootMap()->InitDeviceObjects ( m_pd3dDevice );

	// 메트릭스 에디트의 초기화.
	//
	m_pDxEditmat->InitDeviceObjects ( m_pd3dDevice );
	m_p3dPosMesh->InitDeviceObjects ( m_pd3dDevice );
	m_EditBox.InitDeviceObjects ( m_pd3dDevice );

	{	// Note : 인터페이스 스킬 텍스트 로딩
		TCHAR szFullPath[MAX_PATH] = {0};

		StringCchCopy(szFullPath, MAX_PATH, pFrame->m_szAppPath);
		StringCchCat(szFullPath, MAX_PATH, pPath->GuiFileRoot().c_str()); //SUBPATH::GUI_FILE_ROOT);
		CGameTextMan::GetInstance().SetPath(szFullPath);

		//CGameTextMan::GetInstance().LoadText(RANPARAM::strGameWord.GetString(),   CGameTextMan::EM_GAME_WORD, TRUE ,RANPARAM::strCountry);
		CGameTextMan::GetInstance().LoadText(RANPARAM::strGameInText.GetString(), CGameTextMan::EM_GAME_IN_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT);
		//CGameTextMan::GetInstance().LoadText(RANPARAM::strGameExText.GetString(), CGameTextMan::EM_GAME_EX_TEXT, TRUE,RANPARAM::strCountry);
	}

	return S_OK;
}

HRESULT CLevelEditView::RestoreDeviceObjects()
{
    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();

    HRESULT hr = S_OK;

	// 각종 장치들.
	//
	m_pEngineDevice->RestoreDeviceObjects ();

	// 뷰포트
	//
	DxViewPort::GetInstance().SetProjection ( D3DX_PI/4, (float)m_d3dsdBackBuffer.Width, 
		(float)m_d3dsdBackBuffer.Height, 5.0f, 3000.0f );
	
	hr = DxViewPort::GetInstance().SetViewPort ();
	if ( FAILED(hr) )	return hr;

	m_spDxLandMan->RestoreDeviceObjects ( m_pd3dDevice );
	pGaeaServer->GetRootMap()->RestoreDeviceObjects ( m_pd3dDevice );

	m_p3dPosMesh->RestoreDeviceObjects ( m_pd3dDevice );
	m_sObjectMRS.OnResetDevice( m_pd3dDevice );

	return S_OK;
}

void CLevelEditView::SetCurEditType ( DWORD EditType )
{
	m_pDxEditmat->SetType ( (EDITMAT_TYPE)EditType );

	switch( EditType )
	{
	case TRANS_MAT:
        m_sObjectMRS.SetMode( OBECTMRS_M );
        break;
	case ROTATE_MAT:
        m_sObjectMRS.SetMode( OBECTMRS_R );
        break;
	case SCALE_MAT:
        m_sObjectMRS.SetMode( OBECTMRS_S );
        break;
	}
}

void CLevelEditView::CurSelect ()
{
	// Frame Picking
	//
	DWORD mKeyState = DxInputDevice::GetInstance().GetMouseState ( DXMOUSE_LEFT );

	if ( mKeyState&DXKEY_DOWNED )
	{
		D3DXVECTOR3 vTargetPt, vFromPt;
		vFromPt = DxViewPort::GetInstance().GetFromPt ();
		BOOL bTargetted = DxViewPort::GetInstance().GetMouseTargetPosWnd ( vTargetPt );

		if ( bTargetted && m_spDxLandMan->GetNaviMesh() )
		{
			LPDXFRAME pDxFrame = NULL;
			DWORD dwCollisionID;
			BOOL bCollision;
			D3DXVECTOR3 vCollision;
			m_spDxLandMan->GetNaviMesh()->IsCollision( vFromPt, vTargetPt, vCollision, dwCollisionID, bCollision );
			//m_spDxLandMan->IsCollision ( vFromPt, vTargetPt, vCollision, bCollision, pDxFrame );
					
			if ( bCollision )
			{
				m_vCollisionPos = vCollision;
			}
		}
	}

	m_p3dPosMesh->FrameMove( m_fTime, m_fElapsedTime );
}

void CLevelEditView::CurMove ()
{
	m_pDxEditmat->FrameMove ( m_fTime, m_fElapsedTime );
}

void CLevelEditView::SetCharMoveTo ()
{

}

HRESULT CLevelEditView::FrameMove( BOOL bNotRendering )
{
    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();

	//	float 정확도 향상
//	CheckControlfp();
		
	m_sObjectMRS.SetWVL( pFrame->m_wndToolBar.m_comboMatrix.GetCurSel() );
	if( m_sObjectMRS.IsCollMode() && m_CurType!=_CUR_SEL_MOBSCH && m_CurType!=ID_VIEW_BOXEDIT )
	{
		m_CurType = _CUR_SELECT;
	}

	// 각종 장치들.
	//
	m_pEngineDevice->FrameMove( m_fTime, m_fElapsedTime, m_bDefWin );

	// 뷰포트
	//DxViewPort::GetInstance().FrameMove( 0.f, m_fElapsedTime );
	//DxViewPort::GetInstance().SetCameraZoomInOut( 5.0f, 1800.0f );

	DxViewPort::GetInstance().FrameMoveMAX( m_fElapsedTime, m_vCollisionPos );
	
	if( pFrame->m_DlgLevelEdit.m_pSheetTab->GetActiveIndex() == ETCFUNCINDEX )
	{

		CDialog *pDialog = &pFrame->m_DlgLevelEdit.m_pSheetTab->m_EtcFuncPage;

		if( GetWin_Check( pDialog, IDC_CHECK_CONCAM ) && GetWin_Check( pDialog, IDC_CHECK_CONCAM2 ) )
		{

			CString strTemp;
			float fUp, fDown, fLeft, fRight, fFar, fNear;
			D3DXVECTOR3 vBaseCamPos, vFromPt, vLookatPt, vUpVec, vMaxPos, vMinPos;
				

			strTemp = GetWin_Text( pDialog, IDC_EDIT_CONCAM1 );
			fUp	  = (float)atof(strTemp.GetString());
			strTemp = GetWin_Text( pDialog, IDC_EDIT_CONCAM2 );
			fDown	  = (float)atof(strTemp.GetString());
			strTemp = GetWin_Text( pDialog, IDC_EDIT_CONCAM3 );
			fRight	  = (float)atof(strTemp.GetString());
			strTemp = GetWin_Text( pDialog, IDC_EDIT_CONCAM4 );
			fLeft	  = (float)atof(strTemp.GetString());
			strTemp = GetWin_Text( pDialog, IDC_EDIT_CONCAM8 );
			fFar	  = (float)atof(strTemp.GetString());
			strTemp = GetWin_Text( pDialog, IDC_EDIT_CONCAM9 );
			fNear	  = (float)atof(strTemp.GetString());


			strTemp = GetWin_Text( pDialog, IDC_EDIT_CONCAM5 );
			vBaseCamPos.x = (float)atof(strTemp.GetString());
			strTemp = GetWin_Text( pDialog, IDC_EDIT_CONCAM6 );
			vBaseCamPos.y = (float)atof(strTemp.GetString());
			strTemp = GetWin_Text( pDialog, IDC_EDIT_CONCAM7 );
			vBaseCamPos.z = (float)atof(strTemp.GetString());	
			


			vMaxPos = D3DXVECTOR3( vBaseCamPos.x + fRight, vBaseCamPos.y + fUp, vBaseCamPos.z + fFar );
			vMinPos = D3DXVECTOR3( vBaseCamPos.x + fLeft, vBaseCamPos.y + fDown, vBaseCamPos.z + fNear );			
			DxViewPort::GetInstance().GetViewTrans( vFromPt, vLookatPt, vUpVec );	

			CDebugSet::ToView( 7, _T("vFromPt %.2fx %.2fy %.2fz"), vFromPt.x, vFromPt.y, vFromPt.z );


			D3DXVECTOR3 vCamPos = vFromPt - vLookatPt;
			if( vCamPos.x > vMaxPos.x ) vCamPos.x = vMaxPos.x;
			if( vCamPos.y > vMaxPos.y ) vCamPos.y = vMaxPos.y;
			if( vCamPos.z > vMaxPos.z ) vCamPos.z = vMaxPos.z;

			if( vCamPos.x < vMinPos.x ) vCamPos.x = vMinPos.x;
			if( vCamPos.y < vMinPos.y ) vCamPos.y = vMinPos.y;
			if( vCamPos.z < vMinPos.z ) vCamPos.z = vMinPos.z;

			vFromPt = vCamPos + vLookatPt;

			CDebugSet::ToView( 8, _T("vNEwFromPt %.2fx %.2fy %.2fz"), vFromPt.x, vFromPt.y, vFromPt.z );

			//DxViewPort::GetInstance().SetViewTrans( vFromPt, vLookatPt, vUpVec );			

			DxViewPort::GetInstance().SetCameraControl( TRUE, vBaseCamPos, fUp, fDown, fLeft, fRight, fFar, fNear );
		}else{

			DxViewPort::GetInstance().SetCameraControl( FALSE );
		}
		
	}

	m_spDxLandMan->FrameMove ( m_fTime, m_fElapsedTime, DxViewPort::GetInstance().GetClipVolume(), bNotRendering );

	pGaeaServer->FrameMove ( m_fTime, m_fElapsedTime );

	switch ( m_CurType )
	{
	case _CUR_SELECT:
		CurSelect ();
		break;

	case _CUR_ROTATE:
	case _CUR_MOVE:
	case _CUR_SCALE:
		CurMove ();
		break;

	case _CUR_SEL_MOBSCH:
		{
			DWORD mKeyState = DxInputDevice::GetInstance().GetMouseState ( DXMOUSE_LEFT );

			if ( mKeyState&DXKEY_UP )
			{
				GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
				SpMobScheduleManServer spMobSchMan(pGLLandMan->GetMobSchMan ());
				if ( spMobSchMan == NULL )
					break;

				D3DXVECTOR3 vTargetPt, vFromPt;
				vFromPt = DxViewPort::GetInstance().GetFromPt ();
				BOOL bTargetted = DxViewPort::GetInstance().GetMouseTargetPosWnd ( vTargetPt );

				if ( bTargetted )
				{
					GLMobSchedule* pMobSch = spMobSchMan->GetCollisionMobSchedule ( vFromPt, vTargetPt );
					if ( pMobSch )
					{
						CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
						
						PCROWDATA pCrow = GLCrowDataMan::GetInstance().GetCrowData ( pMobSch->m_CrowID );
						if ( pCrow->m_emCrow != CROW_MATERIAL )	CMobSetPage::m_bMaterial = FALSE;
						else	CMobSetPage::m_bMaterial = TRUE;
						
						pFrame->m_DlgLevelEdit.m_pSheetTab->SetActivePage ( MOBSETPAGEINDEX );						
						pFrame->m_DlgLevelEdit.m_pSheetTab->m_MobSetPage.ActiveMobSetEdit();
						pFrame->m_DlgLevelEdit.m_pSheetTab->m_MobSetPage.SetSelectSchedule ( pMobSch );
					}
				}
			}
		}
		break;

	case ID_VIEW_BOXEDIT:
		m_EditBox.FrameMove ( m_fTime, m_fElapsedTime );
		break;
	};

	if( pFrame->m_DlgLevelEdit.m_pSheetTab->GetActiveIndex() == ETCFUNCINDEX )
	{
		CEtcFuncPage *pDialog = &pFrame->m_DlgLevelEdit.m_pSheetTab->m_EtcFuncPage;
		if( GetWin_Check( pDialog, IDC_CHECK_LANDEFF ) )
		{
			CurSelect ();
		}
	}

    // 마우스 커서 관련 단축키;
    if ( DxInputDevice::GetInstance().GetKeyState( DIK_F1 ) & DXKEY_DOWN )
    {
        pFrame->OnViewCurnone();
    }
    if ( DxInputDevice::GetInstance().GetKeyState( DIK_F2 ) & DXKEY_DOWN )
    {
        pFrame->OnViewSelect();
    }
    if ( DxInputDevice::GetInstance().GetKeyState( DIK_F3 ) & DXKEY_DOWN )
    {
        pFrame->OnViewMove();
    }
    if ( DxInputDevice::GetInstance().GetKeyState( DIK_F4 ) & DXKEY_DOWN )
    {
        pFrame->OnViewRotate();
    }
    if ( DxInputDevice::GetInstance().GetKeyState( DIK_F5 ) & DXKEY_DOWN )
    {
        pFrame->OnViewScale();
    }
    if ( DxInputDevice::GetInstance().GetKeyState( DIK_F6 ) & DXKEY_DOWN )
    {
        pFrame->OnViewSelectMobsch();
    }
    if ( DxInputDevice::GetInstance().GetKeyState( DIK_F7 ) & DXKEY_DOWN )
    {
        pFrame->OnViewCameraTo();
    }

	//////////////////////////////////////////////////////////////////////////
	// NSSkinAniThread
	// TAG_ProcessAnimationThread_3
	// 모든 정보가 COMPLETE_DATA 될 때 까지 기다리도록 한다.
	NSSkinAniThread::Wait_CompleteSkinAniControl( m_fElapsedTime );

	return S_OK;
}

VOID CLevelEditView::RenderLandEffectState()
{
    // version 0x0105 이하;
    // 지형효과의 영역정보는 x와 z값만을 가지고 y값은 없다.(맵 x,z좌표에 의해서 효과가 발동되는지 검출);
    // 따라서 랜더링을 할때는 plane이나 circle을 랜더링 할 적당한 y값을 구한다;
    // 실제 효과는 랜더링된 palne,circle이 y축으로 무한히 확장시킨 영역에 지형효과를 준다;
    
    // version 0x0106 이후;
    // 지형효과의 영역정보가 x와 z값 y값을 가지고 있지만 현재 y값은 LevelEdit툴에서 랜더링을 위해서만 사용 중.(실제 효과는 맵 x,z좌표에 의해서 효과가 발동되는지 검출);
    // 높이 값이 존재한다고해서 서로 다른 높이의 서로 다른 지형효과가 적용되지는 않는다는 점 유의해야 함;
    
	CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	if (!pFrame)
        return;
    
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	
	float fYDist1 = 100.0f;
	float fYDist2 = 100.0f;
	DWORD dwCollisionID;
	BOOL bCollision;
	D3DXVECTOR3 vCollision;

	D3DXVECTOR3 vFromPt, vTargetPt;
	DWORD dwSelectNum = -1;
	VEC_LANDEFF vecLandEffect;
    DWORD dwVesion = SLEVEL_ETC_FUNC::VERSION;

	if( pFrame->m_DlgLevelEdit.m_pSheetTab->GetActiveIndex() != ETCFUNCINDEX ) 
	{
		GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
		SLEVEL_ETC_FUNC* pEtcFunc = &pGLLandMan->m_sLevelEtcFunc;
		if( pEtcFunc == NULL || pEtcFunc->m_bUseFunction[EMETCFUNC_LANDEFFECT] == FALSE ) return;

		vecLandEffect = pEtcFunc->m_vecLandEffect;
        dwVesion = pEtcFunc->m_dwVersion;
	}else{
		CEtcFuncPage *pDialog = &pFrame->m_DlgLevelEdit.m_pSheetTab->m_EtcFuncPage;
		if( !pDialog ) return;
		if( !GetWin_Check( pDialog, IDC_CHECK_LANDEFF ) ) return;

		dwSelectNum   = pDialog->m_dwSelectNum;
		vecLandEffect = pDialog->m_vecLandEffect;
	
		if( ( DxInputDevice::GetInstance().GetKeyState( DIK_LCONTROL ) & DXKEY_DOWNED ) &&
			( DxInputDevice::GetInstance().GetMouseState ( DXMOUSE_LEFT ) & DXKEY_DOWNED ) )
		{
			if( DxInputDevice::GetInstance().GetMouseState ( DXMOUSE_LEFT ) & DXKEY_DOWN )
			{
				pDialog->m_vMinLandPos = m_vCollisionPos;
			}else
			{
				pDialog->m_vMaxLandPos = m_vCollisionPos;
			}
		}

        vTargetPt.x = vFromPt.x = pDialog->m_vMinLandPos.x;
		vTargetPt.y = pDialog->m_vMinLandPos.y - 50.0f; vFromPt.y = pDialog->m_vMinLandPos.y + 50.0f;
		vTargetPt.z = vFromPt.z = pDialog->m_vMinLandPos.z;
		m_spDxLandMan->GetNaviMesh()->IsCollision( vFromPt, vTargetPt, vCollision, dwCollisionID, bCollision );
		if( bCollision )
		{
			fYDist1 = vCollision.y;
		}

        vTargetPt.x = vFromPt.x = pDialog->m_vMaxLandPos.x;
        vTargetPt.y = pDialog->m_vMaxLandPos.y - 50.0f; vFromPt.y = pDialog->m_vMaxLandPos.y + 50.0f;
        vTargetPt.z = vFromPt.z = pDialog->m_vMaxLandPos.z;
		m_spDxLandMan->GetNaviMesh()->IsCollision( vFromPt, vTargetPt, vCollision, dwCollisionID, bCollision );
		if( bCollision )
		{
			fYDist2 = vCollision.y;
		}

		if( pDialog->m_vMinLandPos != D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) &&
			pDialog->m_vMaxLandPos != D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) )
		{

			if( pDialog->m_bUseCircle )
			{
				D3DXVECTOR2 vTemp = D3DXVECTOR2(pDialog->m_vMaxLandPos.x,pDialog->m_vMaxLandPos.z) - D3DXVECTOR2(pDialog->m_vMinLandPos.x,pDialog->m_vMinLandPos.z);
				float fLength = fabs(D3DXVec2Length(&vTemp));
				D3DXVECTOR3 vPos = D3DXVECTOR3( pDialog->m_vMinLandPos.x, max(fYDist1,fYDist2), pDialog->m_vMinLandPos.z );
				D3DXVECTOR3 vScale = D3DXVECTOR3( fLength, 5.0f, fLength );

				EDITMESHS::RENDERSPHERE( m_pd3dDevice, vPos, vScale, TRUE, NULL );
			}else{

				D3DXVECTOR2 vMin, vMax;
				vMin = min( D3DXVECTOR2(pDialog->m_vMinLandPos.x,pDialog->m_vMinLandPos.z), D3DXVECTOR2(pDialog->m_vMaxLandPos.x,pDialog->m_vMaxLandPos.z) );
				vMax = max( D3DXVECTOR2(pDialog->m_vMinLandPos.x,pDialog->m_vMinLandPos.z), D3DXVECTOR2(pDialog->m_vMaxLandPos.x,pDialog->m_vMaxLandPos.z) );
				
				D3DXVECTOR3 vertex[4];
				vertex[0].y = vertex[1].y = vertex[2].y = vertex[3].y = max(fYDist1,fYDist2) + 1.0f;

				vertex[0].x = vMin.x;
				vertex[0].z = vMin.y;

				vertex[1].x = vMax.x;
				vertex[1].z = vMin.y;

				vertex[2].x = vMin.x;
				vertex[2].z = vMax.y;

				vertex[3].x = vMax.x;
				vertex[3].z = vMax.y;
			
				EDITMESHS::RENDERPLANE( m_pd3dDevice, vertex, 0xffffffff );

			}
		}

		/*CDebugSet::ToView( 3, _T("vMin %.2fx %.2fy"), pDialog->m_vMinLandPos.x, pDialog->m_vMinLandPos.y );
		CDebugSet::ToView( 4, _T("vMax %.2fx %.2fy"), pDialog->m_vMaxLandPos.x, pDialog->m_vMaxLandPos.y );
		CDebugSet::ToView( 5, _T("vCollisionPos %.2fx %.2fy %.2fz"), m_vCollisionPos.x, m_vCollisionPos.y, m_vCollisionPos.z );*/
	}

	SLANDEFFECT landEffect;
	float fMaxYPos = 0.0f;
    fYDist1 = 100.0f;
    fYDist2 = 100.0f;
	for( unsigned int i = 0; i < vecLandEffect.size(); i++ )
	{
		landEffect = vecLandEffect[i];

        //version 0x0105 이하에는 y값이 없으므로 적당한 위치의 하늘에서 지면으로 컬리전 레이를 쏜다.;
		if( dwVesion <= 0x0105 )
        {
		    vTargetPt.y = -1500.0f;
            vFromPt.y = 1500.0f;
            vTargetPt.x = vFromPt.x = landEffect.vMinPos.x;
            vTargetPt.z = vFromPt.z = landEffect.vMinPos.z;
            m_spDxLandMan->GetNaviMesh()->IsCollision( vFromPt, vTargetPt, vCollision, dwCollisionID, bCollision );
            if( bCollision )
            {
                fYDist1 = vCollision.y;
            }

            vTargetPt.y = -1500.0f;
            vFromPt.y = 1500.0f;
            vTargetPt.x = vFromPt.x = landEffect.vMaxPos.x;
            vTargetPt.z = vFromPt.z = landEffect.vMaxPos.z;
            m_spDxLandMan->GetNaviMesh()->IsCollision( vFromPt, vTargetPt, vCollision, dwCollisionID, bCollision );
            if( bCollision )
            {
                fYDist2 = vCollision.y;
            }
        }
        else
        {
            fYDist1 = landEffect.vMinPos.y + 1.0f;
            fYDist2 = landEffect.vMaxPos.y + 1.0f;
        }

		fMaxYPos = max(fYDist1,fYDist2);
        

		DWORD dwColor;
		BYTE  dwAlpha;
		if( dwSelectNum == i ) dwAlpha = 128;
		else							  dwAlpha = 255;		

		switch( landEffect.emLandEffectType ) 
		{
		case EMLANDEFFECT_ATK_SPEED:  // 빨간색
			dwColor = D3DCOLOR_ARGB( dwAlpha, 0xff, 0x00, 0x00 );
			break;
		case EMLANDEFFECT_MOVE_SPEED: // 주황색
			dwColor = D3DCOLOR_ARGB( dwAlpha, 0xff, 0xa5, 0x00 );
			break;
		case EMLANDEFFECT_MP_RATE:	  // 노란색
			dwColor = D3DCOLOR_ARGB( dwAlpha, 0xff, 0xff, 0x00 );
			break;
		case EMLANDEFFECT_HP_RATE:    // 초록색
			dwColor = D3DCOLOR_ARGB( dwAlpha, 0x00, 0x80, 0x00 );
			break;
		case EMLANDEFFECT_RECOVER_RATE: // 파란색
			dwColor = D3DCOLOR_ARGB( dwAlpha, 0x00, 0x00, 0xff );
			break;
		case EMLANDEFFECT_DAMAGE_RATE:	// 남색
			dwColor = D3DCOLOR_ARGB( dwAlpha, 0x4b, 0x00, 0x82 );
			break;
		case EMLANDEFFECT_DEFENSE_RATE:	// 보라색
			dwColor = D3DCOLOR_ARGB( dwAlpha, 0xdd, 0xa0, 0xdd );
			break;
		case EMLANDEFFECT_RESIST_RATE:	// 회색
			dwColor = D3DCOLOR_ARGB( dwAlpha, 0x80, 0x80, 0x80 );
			break;
		case EMLANDEFFECT_CANCEL_ALLBUFF: // 검정색
			dwColor = D3DCOLOR_ARGB( dwAlpha, 0x00, 0x00, 0x00 );
			break;
		case EMLANDEFFECT_SAFE_ZONE: // 하늘색
			dwColor = D3DCOLOR_ARGB( dwAlpha, 0x80, 0xFF, 0xFF );
			break;
		}

		if( landEffect.bCircleCheck )
		{
			D3DXVECTOR3 vPos = D3DXVECTOR3( landEffect.vMinPos.x, max(fYDist1,fYDist2), landEffect.vMinPos.z );
			D3DXVECTOR3 vScale = D3DXVECTOR3( landEffect.fLength, 5.0f, landEffect.fLength );

			EDITMESHS::RENDERSPHERE( m_pd3dDevice, vPos, vScale, TRUE, NULL, dwColor );
		}else{
			D3DXVECTOR3 vertex[4];
			vertex[0].y = vertex[1].y = vertex[2].y = vertex[3].y = fMaxYPos + 1.0f;

			vertex[0].x  = landEffect.vMinPos.x;
			vertex[0].z  = landEffect.vMinPos.z;

			vertex[1].x  = landEffect.vMaxPos.x;
			vertex[1].z  = landEffect.vMinPos.z;

			vertex[2].x  = landEffect.vMinPos.x;
			vertex[2].z  = landEffect.vMaxPos.z;

			vertex[3].x  = landEffect.vMaxPos.x;
			vertex[3].z  = landEffect.vMaxPos.z;
			
			EDITMESHS::RENDERPLANE( m_pd3dDevice, vertex, dwColor );
		}
	}	
}

HRESULT CLevelEditView::Render()
{
    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();

	DxFogMan::GetInstance().RenderFogSB ( m_pd3dDevice, FALSE );
	D3DCOLOR colorClear = DxFogMan::GetInstance().GetFogColor();

	RENDERPARAM::g_emTnLMode = TnL_PS_1_1;

	HRESULT hr(S_OK);
	hr = m_pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR );
	if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_NM )
	{
		hr = m_pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pSufFourCC_INTZ );
	}
	else
	{
		hr = m_pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pZBuffer );
	}

	// Clear the viewport
	hr = m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, colorClear, 1.0f, 0L );

	//if ( DxSurfaceTex::GetInstance().m_pColorSuf_1st && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF && m_spDxLandMan->GetUsedMaterialSystem() )
	//{
	//	hr = m_pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pColorSuf_1st );
	//}

	// Shadow Map Clear
	//
	DxShadowMap::GetInstance().ClearShadow ( m_pd3dDevice, m_spDxLandMan->GetUsedMaterialSystem() );

	// Begin the scene
	if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
	{
		// Note ; 안개는 필요없다.
		m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

		// Light 설정.
		//
		DxLightMan::GetInstance()->Render ( m_pd3dDevice, FALSE, m_spDxLandMan->GetUsedMaterialSystem() );

		// 버텍스 쉐이더 콘스탄트 설정.
		//
		DXLIGHT &Light = *DxLightMan::GetInstance()->GetDirectLight ();
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTDIRECT, (float*)&Light.m_Light.Direction, 1 );
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTDIFFUSE, (float*)&Light.m_Light.Diffuse, 1 );
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTAMBIENT, (float*)&Light.m_Light.Ambient, 1 );

		D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt ();
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_CAMERAPOSITION, (float*)&vFromPt, 1 );

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

		CLIPVOLUME &cv = DxViewPort::GetInstance().GetClipVolume ();


		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF )
		{
			RenderDefferedOpaqueForWLD( m_pd3dDevice, colorClear );
		}
		else
		{
			m_spDxLandMan->Render ( m_pd3dDevice, cv );
		}

		//if ( DxSurfaceTex::GetInstance().m_pColorSuf_1st && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF && m_spDxLandMan->GetUsedMaterialSystem() )
		//{
		//	// Note : 
		//	D3DSURFACE_DESC	sDesc;
		//	LPDIRECT3DSURFACEQ pBackBuffer;
		//	m_pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
		//	pBackBuffer->GetDesc ( &sDesc );
		//	pBackBuffer->Release();

		//	hr = m_pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pColorBuffer );
		//}

		//m_spDxLandMan->Render_2Pass( m_pd3dDevice, cv, TRUE );
	

		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF )
		{
			RenderDefferedSoftAlphaForWLD( m_pd3dDevice );
		}
		else
		{
			m_spDxLandMan->Render_LOW_SoftAlpha( m_pd3dDevice, cv );
			m_spDxLandMan->Render_LOW_SingleEff( m_pd3dDevice, cv );
		}


		GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
		if ( m_bRENDQUAD )
		{
			pGLLandMan->RendQuad ( m_pd3dDevice );
		}
		SpMobScheduleManServer spMobSchMan(pGLLandMan->GetMobSchMan());
		if ( spMobSchMan )
			spMobSchMan->Render ( m_pd3dDevice, cv );
		if ( CMobSetPage::m_emEditType != CMobSetPage::EM_EDIT_NONE )
		{
			CMobSetPage::m_Schedule.RenderGenPos ( m_pd3dDevice );
			CMobSetPage::m_Schedule.Render ( m_pd3dDevice, cv );
		}

		pGaeaServer->Render ( m_pd3dDevice, cv );
		pGaeaServer->GetRootMap()->Render ( m_pd3dDevice, cv );
		pGaeaServer->GetRootMap()->Render_EFF ( m_pd3dDevice, cv );


		if ( g_bVIEW_NAVIGATION_MESH_FOR_LEVELEDIT )
		{
			m_spDxLandMan->RenderNavigationForEditVerySlow( m_pd3dDevice );
		}

		// Collsion 정보를 보여준다.
		NSSimpleCollisionClient::Render( m_pd3dDevice, pGLLandMan->GetvecLayerCollision() );


		// 하늘 렌더링..!! 후
        DxSkyMan::GetInstance().Render_AFTER( m_pd3dDevice, DxViewPort::GetInstance().GetFOV(), DxViewPort::GetInstance().GetAspect() );



		//////////////////////////////////////////////////////////////////////////
		//			HDR - Bloom 적용.
		//m_pd3dDevice->StretchRect( DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR, NULL, DxSurfaceTex::GetInstance().m_pColorBuffer, NULL, D3DTEXF_POINT );+
		m_pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pColorBuffer );
		NSBloomHDR::OnRender( m_pd3dDevice );



		// Edit 용.
		//
		D3DXMatrixIdentity( &matIdentity );
		m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );		

		DWORD	dwNormal;
		m_pd3dDevice->GetRenderState( D3DRS_NORMALIZENORMALS, &dwNormal );
		m_pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );

		D3DXVECTOR3 vCollPos(FLT_MAX/100,FLT_MAX/100,FLT_MAX/100);
		D3DXMATRIX matWorld;
		D3DXMatrixIdentity ( &matWorld );

        DWORD dwFogEnable(0L);
        DWORD dwFogColor(0L);
        m_pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );
        m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );

        BOOL bBackUpWireFrame = g_bWIREFRAME_VIEW;
	    g_bWIREFRAME_VIEW = TRUE;

        NSMaterialManager::SetFogColorForEditor ( 0xffffff00 );

		switch ( m_CurType )
		{
		case _CUR_SELECT:
			{
				vCollPos = m_vCollisionPos;
				D3DXMatrixTranslation ( &matWorld, vCollPos.x, vCollPos.y, vCollPos.z );

				D3DXVECTOR3 vFromPt = DxViewPort::GetInstance().GetFromPt ();
				vFromPt -= vCollPos;

				float fDist = D3DXVec3Length ( &vFromPt );
				float fScale = DxViewPort::GetInstance().ComputeDistSize ( fDist ) / 800.0f;

				D3DXMATRIX matScale;
				D3DXMatrixScaling ( &matScale, fScale, fScale, fScale );
				D3DXMatrixMultiply ( &matWorld, &matScale, &matWorld );

                m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,	D3DFILL_WIREFRAME );
				m_pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_ALWAYS );
				
				m_p3dPosMesh->Render ( m_pd3dDevice, &matWorld, &cv );
				
				m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,	D3DFILL_SOLID );
				m_pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_LESSEQUAL );

				m_p3dPosMesh->Render ( m_pd3dDevice, &matWorld, &cv );
			}
			break;

		case _CUR_ROTATE:
			{
				m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,	D3DFILL_WIREFRAME );
				m_pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_ALWAYS );
				m_pDxEditmat->Render ( m_pd3dDevice );
				m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,	D3DFILL_SOLID );
				m_pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_LESSEQUAL );

				m_pDxEditmat->Render ( m_pd3dDevice );
			}
			break;

		case _CUR_MOVE:
			{
				m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,	D3DFILL_WIREFRAME );
				m_pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_ALWAYS );
				m_pDxEditmat->Render ( m_pd3dDevice );
				m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,	D3DFILL_SOLID );
				m_pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_LESSEQUAL );

				m_pDxEditmat->Render ( m_pd3dDevice );
			}
			break;

		case _CUR_SCALE:
			{
				m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,	D3DFILL_WIREFRAME );
				m_pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_ALWAYS );
				m_pDxEditmat->Render ( m_pd3dDevice );
				m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,	D3DFILL_SOLID );
				m_pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_LESSEQUAL );

				m_pDxEditmat->Render ( m_pd3dDevice );
			}
			break;

		case _CUR_EDIT_BOX:
			m_EditBox.Render ( m_pd3dDevice );
			break;
		};

        m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );

	    g_bWIREFRAME_VIEW = bBackUpWireFrame;


		RenderLandEffectState();

		m_pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, dwNormal );

		if ( DxLandGateMan::m_bREND )
		{
			DxLandGateMan* pLandGateMan(pGLLandMan->GetLandGateMan());
			if ( pLandGateMan )
				pLandGateMan->Render ( m_pd3dDevice );
		}

		m_sObjectMRS.Render( m_pd3dDevice );

		// 키를 누르는 행동.
		{
			pFrame->m_DlgLevelEdit.UpdateKeyBoardMouse();
		}

		if ( CLevelSingleEffect::g_bCheckViewNameEFFECT )
		{
			if( pFrame->m_DlgLevelEdit.m_pSheetTab->GetActiveIndex() == SINGLEFFECTINDEX )
			{
				pGLLandMan->RenderEffectName( m_pd3dDevice, 0 );
			}
			else if( pFrame->m_DlgLevelEdit.m_pSheetTab->GetActiveIndex() == SINGLEFFECTLAYERINDEX )
			{
				pGLLandMan->RenderEffectName( m_pd3dDevice, CMobSetPage::g_unLayer+1 );
			}
		}

		if ( CLevelCollision::g_bCheckViewNameCOLLISION )
		{
			if( pFrame->m_DlgLevelEdit.m_pSheetTab->GetActiveIndex() == COLLISIONINDEX )
			{
				NSSimpleCollisionClient::RenderCollisionName( m_pd3dDevice, pGLLandMan->GetvecLayerCollision(), 0 );
			}
			else if( pFrame->m_DlgLevelEdit.m_pSheetTab->GetActiveIndex() == COLLISIONLAYERINDEX )
			{
				NSSimpleCollisionClient::RenderCollisionName( m_pd3dDevice, pGLLandMan->GetvecLayerCollision(), CMobSetPage::g_unLayer+1 );
			}
		}

		RenderText();
		CDebugSet::Render ();

		// End the scene.
		m_pd3dDevice->EndScene();
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RenderText()
// Desc: Renders stats and help text to the scene.
//-----------------------------------------------------------------------------
HRESULT CLevelEditView::RenderText()
{
	D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,255);
    D3DCOLOR fontWarningColor = D3DCOLOR_ARGB(255,0,255,255);
    TCHAR szMsg[MAX_PATH] = TEXT("");

	CD3DFontPar* pD3dFont = DxFontMan::GetInstance().FindFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	if ( !pD3dFont )	return S_FALSE;

    // Output display stats
    FLOAT fNextLine = 40.0f; 

    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine -= 20.0f;
    pD3dFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strFrameStats );
    fNextLine -= 20.0f;
    pD3dFont->DrawText( 2, fNextLine, fontColor, szMsg );

    return S_OK;
}

HRESULT CLevelEditView::InvalidateDeviceObjects()
{
    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();

	// 각종 장치들.
	//
	m_pEngineDevice->InvalidateDeviceObjects ();

	m_pDxEditmat->InvalidateDeviceObjects ();
	m_p3dPosMesh->InvalidateDeviceObjects ( m_pd3dDevice );
	m_sObjectMRS.OnLostDevice( m_pd3dDevice );

	m_spDxLandMan->InvalidateDeviceObjects();
	pGaeaServer->GetRootMap()->InvalidateDeviceObjects ();

	return S_OK;
}

HRESULT CLevelEditView::DeleteDeviceObjects()
{
    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();

	// 각종 장치들.
	//
	m_pEngineDevice->DeleteDeviceObjects ();

	m_pDxEditmat->DeleteDeviceObjects ();
	m_p3dPosMesh->DeleteDeviceObjects ();

	m_spDxLandMan->DeleteDeviceObjects ();
	pGaeaServer->GetRootMap()->DeleteDeviceObjects ();

	pGaeaServer->CleanUp ();

	GMTOOL::CleanUp();

	return S_OK;
}

HRESULT CLevelEditView::FinalCleanup()
{
	HRESULT hr;

	// 각종 장치들.
	//
	m_pEngineDevice->FinalCleanup ();

	// 로직 대이터.
	//
	hr = GLogicData::GetInstance().ClearData ();
	if ( FAILED(hr) )	return hr;

	SAFE_DELETE(m_pDxEditmat);
	SAFE_DELETE(m_p3dPosMesh);	

	CLevelSingleEffect::FinalCleanup_STATIC();

	return S_OK;
}

void CLevelEditView::ActiveEditMatrix2( D3DXMATRIX* matWorld )
{
	m_CurType = _CUR_MOVE;

	if( matWorld )
	{
		m_sObjectMRS.SetMatrix( matWorld );
	}
	else
	{
		m_sObjectMRS.SetMatrix( NULL );
	}

	m_pDxEditmat->SetAffineMatrix ( NULL );	// 2개는 겹치면 안된다.
}

void CLevelEditView::ActiveEditMatrix ( DXAFFINEMATRIX *pAffineMatrix )
{
	m_CurType = _CUR_MOVE;
	m_pDxEditmat->SetAffineMatrix ( (DXAFFINEMATRIX*)pAffineMatrix );

	m_sObjectMRS.SetMatrix( NULL );		// 2개는 겹치면 안된다.
}

void CLevelEditView::DeActiveEditMatrix ()
{
	m_CurType = _CUR_SELECT;
	m_pDxEditmat->SetAffineMatrix ( NULL );
	m_sObjectMRS.SetMatrix( NULL );
}

void CLevelEditView::ActiveEditBox ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	m_CurType = _CUR_EDIT_BOX;
	m_EditBox.SetBox ( vMax, vMin );
}

void CLevelEditView::DeActiveEditBox ()
{
	m_CurType = _CUR_NONE;
}


void CLevelEditView::RenderDefferedOpaqueForWLD( IDirect3DDevice9* pd3dDevice, const D3DCOLOR& colorClear )
{
	//////////////////////////////////////////////////////////////////////////
	// Sky Render
	pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR );

	//////////////////////////////////////////////////////////////////////////
	//					RenderDefferedOpaqueForPSF
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );

	if ( RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_DF )
	{
		NSDeffered::RenderDefferedForSM30DF
		(
			pd3dDevice,
			colorClear,
			m_spDxLandMan.get(),
			DxSurfaceTex::GetInstance().GetWidth(),
			DxSurfaceTex::GetInstance().GetHeight(),
			DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR,
			DxSurfaceTex::GetInstance().m_pColorSuf_1st,
			DxSurfaceTex::GetInstance().m_pColorSuf_2nd,
			DxSurfaceTex::GetInstance().m_pColorSuf_4th,
			DxSurfaceTex::GetInstance().m_pColorTex_1st,
			DxSurfaceTex::GetInstance().m_pColorTex_2nd, 
			DxSurfaceTex::GetInstance().m_pColorTex_4th, 
			FALSE,
			0.f,
			matWorld,
			DxViewPort::GetInstance().GetMatView(),
			DxViewPort::GetInstance().GetMatProj(),
			DxViewPort::GetInstance().GetClipVolume(),
			FALSE,
			TRUE,
			TRUE,
			TRUE
		);
	}
	else
	{
		NSDeffered::RenderDefferedForSM30NM
		(
			pd3dDevice,
			colorClear,
			m_spDxLandMan.get(),
			DxSurfaceTex::GetInstance().GetWidth(),
			DxSurfaceTex::GetInstance().GetHeight(),
			DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR,
			DxSurfaceTex::GetInstance().m_pColorSuf_1st,
			DxSurfaceTex::GetInstance().m_pColorSuf_4th,
			DxSurfaceTex::GetInstance().m_pColorSuf_2nd,
			DxSurfaceTex::GetInstance().m_pColorSuf_3rd,	// Color는 unsign64 를 쓰겠다.
			DxSurfaceTex::GetInstance().m_pColorSuf_LBuffer,
			DxSurfaceTex::GetInstance().m_pSufFourCC_INTZ,
			DxSurfaceTex::GetInstance().m_pColorTex_1st,
			DxSurfaceTex::GetInstance().m_pColorTex_4th,
			DxSurfaceTex::GetInstance().m_pColorTex_2nd,
			DxSurfaceTex::GetInstance().m_pColorTex_3rd, 	// Color는 unsign64 를 쓰겠다.
			DxSurfaceTex::GetInstance().m_pColorTex_LBuffer,
			DxSurfaceTex::GetInstance().m_pTexFourCC_INTZ, 
			matWorld,
			DxViewPort::GetInstance().GetMatView(),
			DxViewPort::GetInstance().GetMatProj(),
			DxViewPort::GetInstance().GetClipVolume(),
			FALSE
		);
	}

	// 지형 물 렌더링
	NSWaterMain::Render( pd3dDevice );
}

void CLevelEditView::RenderDefferedSoftAlphaForWLD( IDirect3DDevice9* pd3dDevice )
{
	//////////////////////////////////////////////////////////////////////////
	//					RenderDefferedOpaqueForPSF
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	D3DCOLOR colorClear = D3DCOLOR_XRGB(0,0,0);

	if ( RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_DF )
	{
		NSDeffered::RenderDefferedForSM30DF
		(
			pd3dDevice,
			colorClear,
			m_spDxLandMan.get(),
			DxSurfaceTex::GetInstance().GetWidth(),
			DxSurfaceTex::GetInstance().GetHeight(),
			DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR,
			DxSurfaceTex::GetInstance().m_pColorSuf_1st,
			DxSurfaceTex::GetInstance().m_pColorSuf_2nd,
			DxSurfaceTex::GetInstance().m_pColorSuf_4th,
			DxSurfaceTex::GetInstance().m_pColorTex_1st,
			DxSurfaceTex::GetInstance().m_pColorTex_2nd, 
			DxSurfaceTex::GetInstance().m_pColorTex_4th, 
			FALSE,
			0.f,
			matWorld,
			DxViewPort::GetInstance().GetMatView(),
			DxViewPort::GetInstance().GetMatProj(),
			DxViewPort::GetInstance().GetClipVolume(),
			TRUE,
			TRUE,
			TRUE,
			FALSE
		);
	}
	else
	{
		NSDeffered::RenderDefferedForSM30NM
		(
			pd3dDevice,
			colorClear,
			m_spDxLandMan.get(),
			DxSurfaceTex::GetInstance().GetWidth(),
			DxSurfaceTex::GetInstance().GetHeight(),
			DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR,
			DxSurfaceTex::GetInstance().m_pColorSuf_1st,
			DxSurfaceTex::GetInstance().m_pColorSuf_4th,
			DxSurfaceTex::GetInstance().m_pColorSuf_2nd,
			DxSurfaceTex::GetInstance().m_pColorSuf_3rd,	// Color는 unsign64 를 쓰겠다.
			DxSurfaceTex::GetInstance().m_pColorSuf_LBuffer,
			DxSurfaceTex::GetInstance().m_pSufFourCC_INTZ,
			DxSurfaceTex::GetInstance().m_pColorTex_1st,
			DxSurfaceTex::GetInstance().m_pColorTex_4th,
			DxSurfaceTex::GetInstance().m_pColorTex_2nd,
			DxSurfaceTex::GetInstance().m_pColorTex_3rd, 	// Color는 unsign64 를 쓰겠다.
			DxSurfaceTex::GetInstance().m_pColorTex_LBuffer,
			DxSurfaceTex::GetInstance().m_pTexFourCC_INTZ, 
			matWorld,
			DxViewPort::GetInstance().GetMatView(),
			DxViewPort::GetInstance().GetMatProj(),
			DxViewPort::GetInstance().GetClipVolume(),
			TRUE
		);
	}

	m_spDxLandMan->RenderEff_Deffered( m_pd3dDevice, DxViewPort::GetInstance().GetClipVolume() );
}