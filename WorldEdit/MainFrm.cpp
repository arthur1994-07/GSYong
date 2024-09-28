// MainFrm.cpp : implementation of the CMainFrame class
//
 
#include "stdafx.h"

#include "../EngineLib/DxTools/DxViewPort.h"
#include "../EngineLib/DxTools/DxEditMat.h"
#include "../EngineLib/DxTools/CubeMap/NSCubeMapUtil.h"
#include "../EngineLib/DxTools/Material/NSMaterialManager.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinCharData.h"
#include "../EngineLib/DxLand/LightMap/NSLightMapFX.h"
#include "../EngineLib/DxLand/DxPiece/NSPieceQuickSort.h"
#include "../EngineLib/DxLand/NSLandSettingLua.h"

#include "../RanLogicClient/Char/GLCharacter.h"

#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"
#include "mainfrm.h"
#include "AssetDlg.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL			g_bFPS_VISIBLE = TRUE;
BOOL			g_bCONSOLE_VISIBLE = TRUE;
BOOL			g_bEDIT_PANEL = TRUE;
BOOL			g_bPieceCheck = FALSE;
BOOL			g_bFORCE_FOG_OFF = FALSE;
extern	BOOL	g_bDebugLine;
extern	BOOL	g_bCullNone;
extern BOOL		g_bWIREFRAME_VIEW;
/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_VIEW_BOXLINE, OnViewBoxline)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BOXLINE, OnUpdateViewBoxline)
	ON_COMMAND(ID_VIEW_WIREFRAME, OnViewWireframe)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WIREFRAME, OnUpdateViewWireframe)
	ON_COMMAND(ID_VIEW_PIECECHECK, OnViewPieceCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PIECECHECK, OnUpdateViewPieceCheck)
	ON_COMMAND(ID_VIEW_FRAME, OnViewFrame)
	ON_COMMAND(ID_VIEW_OCTREE, OnViewOctree)
	ON_COMMAND(ID_VIEW_PREFAB, OnViewPrefab)
	ON_COMMAND(ID_VIEW_NAVIMESH, OnViewNavimesh)
	ON_COMMAND(ID_VIEW_SELECT, OnViewCurSelect)
	ON_COMMAND(ID_VIEW_MOVE, OnViewCurMove)
	ON_COMMAND(ID_VIEW_ROTATE, OnViewRotate)
	ON_COMMAND(ID_VIEW_SCALE, OnViewScale)
	ON_COMMAND(ID_VIEW_SELECTPOINT, OnViewSelectpoint)
	ON_COMMAND(ID_VIEW_CURNONE, OnViewCurnone)
	ON_COMMAND(ID_VIEW_MOVETOFRAME, OnViewMovetoframe)
	ON_COMMAND(ID_VIEW_MOVETOPOINT, OnViewMovetopoint)
	ON_COMMAND(ID_CHAR_MOVE, OnCharMove)
	ON_COMMAND(ID_VIEW_CHARMOVETOPOINT, OnViewCharmovetopoint)
	ON_COMMAND(ID_VIEW_EDITBOX, OnViewEditbox)
	ON_COMMAND(ID_VIEW_PIECE, OnViewPiece)
	ON_COMMAND(ID_VIEW_TILE_DRAW, OnViewCurTileDraw)
	ON_COMMAND(ID_VIEW_TILE_GET, OnViewCurTileGet)
	ON_COMMAND(ID_VIEW_TILE_DELETE, OnViewCurTileDel)
	ON_COMMAND(ID_VIEW_ALL_90, OnViewAll90)
	ON_COMMAND(ID_VIEW_FPS_VISIBLE, OnViewFpsVisible)
	ON_COMMAND(ID_VIEW_CONSOLE_VISIBLE, OnViewConsoleVisible)
	ON_COMMAND(ID_VIEW_EDITPANEL, OnViewEditPanel)
	ON_COMMAND(ID_GAME_TEST, OnGameTest)
    ON_COMMAND(ID_CHECK_MIPMAP, OnCheckMipmap)
	ON_COMMAND(ID_CHECK_RENDER_TEXTURE_LIGHTMAP, OnCheckRenderTextureLightmap)
	ON_COMMAND(ID_CHECK_RENDER_LIGHTMAP, OnCheckRenderLightmap)
	ON_COMMAND(ID_CHECK_RENDER_LIGHTMAP_CHECKER, OnCheckRenderLightmapChecker)
	ON_COMMAND(ID_LOADING_LIGHTMAP_CHECKER_TEXTURE, &CMainFrame::OnLoadingLightmapCheckerTexture)
	ON_COMMAND(ID_CHECK_RENDER_LIGHTMAP_POINT, &CMainFrame::OnCheckRenderLightmapPoint)
	ON_COMMAND(ID_RADIOSITY_SETTING_INFO, OnRadiositSettingInfo)
    ON_UPDATE_COMMAND_UI(ID_ASSET_VIEW, &CMainFrame::OnUpdateAssetView)
    ON_COMMAND(ID_ASSET_VIEW, &CMainFrame::OnAssetView)
	ON_COMMAND(ID_TOOLBAR_SPEC_CAM, OnToolBarSpecCam)
	ON_COMMAND(ID_TOOLBAR_SMALL_REFRESH, OnToolBarSmallRefresh)
	ON_COMMAND(ID_VIEW_CHAR_CHANGE, OnViewCharChange)
    ON_WM_CLOSE()
    ON_WM_SHOWWINDOW()
	ON_COMMAND(ID_VIEW_FOG_OFF, &CMainFrame::OnViewFogOff)
	ON_COMMAND(ID_LOD_OFF, &CMainFrame::OnLodOff)
	ON_COMMAND(ID_INSTANCING_OFF, &CMainFrame::OnPieceInstancingOff)
	ON_COMMAND(ID_CHECK_WATER_RT_REFLECTION, OnCheckWaterRealTimeReflection)
	ON_COMMAND(ID_CHECK_WATER_CUBEMAP, OnCheckWaterCubeMap)
	ON_COMMAND(ID_CHECK_WATER_DIFFUSE_TEXTURE, OnCheckWaterDiffuseTexture)
	ON_COMMAND(ID_CHANGE_TEMP_CUBEMAP, &CMainFrame::OnChangeTempCubemap)
	ON_COMMAND(ID_RENDER_PS_3_0_HIGH, OnRenderPS_3_0_HIGH)
	ON_COMMAND(ID_RENDER_PS_3_0, OnRenderPS_3_0)
	ON_COMMAND(ID_RENDER_PS_2_0, OnRenderPS_2_0)
	ON_COMMAND(ID_RENDER_PS_1_1, OnRenderPS_1_1)
	ON_COMMAND(ID_RENDER_FIXED, OnRenderPS_FIXED)
	ON_COMMAND(ID_RENDER_CHAR_SS, OnRenderPS_Char_SS)
	ON_COMMAND(ID_RENDER_CHAR_DEFAULT, OnRenderPS_Char_Default)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame() :
    CSettingIni(_T("WorldEdit.ini"), _T("MainFrame"))
{
	// TODO: add member initialization code here
	
	//	Note : 일부 메뉴를 Disable 시키기 위한 코드
	m_bAutoMenuEnable = FALSE;
}

CMainFrame::~CMainFrame()
{
}

void CMainFrame::UpdateMenuAct ()
{
	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuItem ( ID_VIEW_BOXLINE, g_bDebugLine?MF_CHECKED:MF_UNCHECKED );
	pMenu->CheckMenuItem ( ID_VIEW_WIREFRAME, g_bWIREFRAME_VIEW?MF_CHECKED:MF_UNCHECKED );

	pMenu->CheckMenuRadioItem ( ID_VIEW_FRAME, ID_VIEW_PIECE, CWorldEditView::m_ViewType, MF_BYCOMMAND );
	pMenu->CheckMenuRadioItem ( ID_VIEW_CURNONE, ID_CHAR_MOVE, CWorldEditView::m_CurType, MF_BYCOMMAND );
	pMenu->CheckMenuRadioItem ( ID_CHECK_RENDER_TEXTURE_LIGHTMAP, ID_CHECK_RENDER_LIGHTMAP_CHECKER, CWorldEditView::m_RenderType, MF_BYCOMMAND );
	pMenu->CheckMenuRadioItem ( ID_CHECK_WATER_RT_REFLECTION, ID_CHECK_WATER_DIFFUSE_TEXTURE, CWorldEditView::m_WaterType, MF_BYCOMMAND );

	// OnRenderPS_3_0_HIGH, ID_RENDER_PS_3_0, ID_RENDER_PS_2_0, ID_RENDER_PS_1_1, ID_RENDER_FIXED
	pMenu->CheckMenuRadioItem ( ID_RENDER_PS_3_0_HIGH, ID_RENDER_FIXED, CWorldEditView::m_dwRenderPSType, MF_BYCOMMAND );
	pMenu->CheckMenuRadioItem ( ID_RENDER_CHAR_SS, ID_RENDER_CHAR_DEFAULT, CWorldEditView::m_dwRenderCharType, MF_BYCOMMAND );
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;    

	if (!m_wndTrueToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndTrueToolBar.LoadToolBar(IDR_MAINFRAME, IDB_BITMAP_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndTrueToolBar.CreateComboBox(m_wndTrueToolBar.m_comboMatrix, 33, ID_COMBO_MATRIX, 65, 100))
	{
		TRACE0("Failed to create toolbar's combo box\n");
		return -1;      // fail to create
	}

	m_wndTrueToolBar.m_comboMatrix.InsertString( 0, "World" );
	m_wndTrueToolBar.m_comboMatrix.InsertString( 1, "View" );
	m_wndTrueToolBar.m_comboMatrix.InsertString( 2, "Local" );
	m_wndTrueToolBar.m_comboMatrix.SetCurSel(0);

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	/*
	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	*/

	m_wndTrueToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndTrueToolBar);

	// Note : 실행파일의 경로를 찾아서 저장한다.
	//
	CString strAppPath;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strAppPath = szPath;

	if ( !strAppPath.IsEmpty() )
	{
		m_strAppPath = strAppPath.Left ( strAppPath.ReverseFind ( '\\' ) );
		
		if ( !m_strAppPath.IsEmpty() )
		if ( m_strAppPath.GetAt(0) == '"' )
			m_strAppPath = m_strAppPath.Right ( m_strAppPath.GetLength() - 1 );

		strcpy ( m_szAppPath, m_strAppPath.GetString() );
	}

	//	Note : Edit Bar
	//
	CString StrTitle1 = CString("Edit Bar");
	if ( !m_wndDialogBar1.Create ( this, &m_cDlgEdit, StrTitle1, IDD_DIALOG_EDITFORM ) )
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;      // fail to create

	}

    m_wndDialogBar1.SetBarStyle(m_wndDialogBar1.GetBarStyle()|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC);
	m_wndDialogBar1.EnableDocking(CBRS_ALIGN_RIGHT);
	DockControlBar(&m_wndDialogBar1,AFX_IDW_DOCKBAR_RIGHT);

    m_cDlgEditPIECE.Create( CDlgPieceEdit::IDD, this );

	CWorldEditView *pView = (CWorldEditView*) GetActiveView ();

    m_cDlgAsset.Create(IDD_DIALOG_ASSET, this);

	//	Note : Init
	//
	UpdateMenuAct ();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.cx = 800;
	cs.cy = 600;

	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnViewBoxline() 
{
	// TODO: Add your command handler code here
	g_bDebugLine = !g_bDebugLine;
	//DxReplaceContainer::SetRendAABB( g_bDebugLine );

	switch( g_bDebugLine )
	{
	case TRUE:
		DxMeshes::g_emMeshMeterial = EM_MESH_MATERIAL_RED;
		break;
	case FALSE:
		DxMeshes::g_emMeshMeterial = EM_MESH_MATERIAL;
		break;
	}
}

void CMainFrame::OnUpdateViewBoxline(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck ( g_bDebugLine );
	//DxReplaceContainer::SetRendAABB( g_bDebugLine );

	switch( g_bDebugLine )
	{
	case TRUE:
		DxMeshes::g_emMeshMeterial = EM_MESH_MATERIAL_RED;
		break;
	case FALSE:
		DxMeshes::g_emMeshMeterial = EM_MESH_MATERIAL;
		break;
	}
}

void CMainFrame::OnViewPieceCheck() 
{
	// TODO: Add your command handler code here
	g_bPieceCheck = !g_bPieceCheck;

	switch( g_bPieceCheck )
	{
	case TRUE:
		DxMeshes::g_emMeshMeterial = EM_MESH_MATERIAL_RED;
		break;
	case FALSE:
		DxMeshes::g_emMeshMeterial = EM_MESH_MATERIAL;
		break;
	}
}

void CMainFrame::OnUpdateViewPieceCheck(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck ( g_bPieceCheck );

	switch( g_bPieceCheck )
	{
	case TRUE:
		DxMeshes::g_emMeshMeterial = EM_MESH_MATERIAL_RED;
		break;
	case FALSE:
		DxMeshes::g_emMeshMeterial = EM_MESH_MATERIAL;
		break;
	}
}

void CMainFrame::OnViewWireframe() 
{
	// TODO: Add your command handler code here
	g_bWIREFRAME_VIEW = !g_bWIREFRAME_VIEW;
}

void CMainFrame::OnUpdateViewWireframe(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck ( g_bWIREFRAME_VIEW );	
}

void CMainFrame::OnViewFrame() 
{
	CWorldEditView *pView = (CWorldEditView*) GetActiveView ();

	DxViewPort::GetInstance().SetToolCamera();
	DxViewPort::GetInstance().SetCamCollisionTest( FALSE );

	CWorldEditView::m_ViewType=FRAME_VIEW;
	UpdateMenuAct ();

	pView->SetLandMan();
	pView->GetSetLandMan()->ActiveMap ();
	//m_cDlgEdit.UpdateContrl( TRUE );

	m_cDlgEdit.SetViewFrame();
}

void CMainFrame::OnViewOctree() 
{
	CWorldEditView *pView = (CWorldEditView*) GetActiveView ();

	if ( CWorldEditView::g_bOrthoView )
	{
		DxViewPort::GetInstance().SetToolCameraMiniMap();
	}
	else
	{
		DxViewPort::GetInstance().SetToolCamera();
	}
	DxViewPort::GetInstance().SetCamCollisionTest( FALSE );

	CWorldEditView::m_ViewType=OCTREE_VIEW;
	UpdateMenuAct ();

	pView->GetLandMan()->ActiveMap_Tool ( pView->GetD3DDevice(), pView->GetspLandMan() );

	m_cDlgEdit.SetViewOctree();

	//m_cDlgEdit.UpdateContrl( TRUE );
}

void CMainFrame::OnViewPrefab() 
{
	CWorldEditView *pView = (CWorldEditView*) GetActiveView ();

	CWorldEditView::m_ViewType=FRAME_VIEW;
	UpdateMenuAct ();

	pView->SetLandManPrefab();
	pView->GetSetLandMan()->ActiveMap ();

	m_cDlgEdit.SetViewPrefab();

	//m_cDlgEdit.UpdateContrl( TRUE );
}

void CMainFrame::OnViewPiece()
{
	CWorldEditView *pView = (CWorldEditView*) GetActiveView ();

	DxViewPort::GetInstance().SetToolCamera();
	DxViewPort::GetInstance().SetCamCollisionTest( FALSE );

	CWorldEditView::m_ViewType=PIECE_VIEW;
	UpdateMenuAct ();
}

void CMainFrame::OnViewNavimesh()
{
	// TODO: Add your command handler code here
	CWorldEditView::m_ViewType=NAVI_VIEW;

	DxViewPort::GetInstance().SetToolCamera();
	DxViewPort::GetInstance().SetCamCollisionTest( FALSE );

	UpdateMenuAct ();
}

void CMainFrame::OnViewCurnone()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CWorldEditView::m_CurType = _CUR_NONE;

	DxViewPort::GetInstance().SetToolCamera();
	DxViewPort::GetInstance().SetCamCollisionTest( FALSE );

	UpdateMenuAct ();

	CWorldEditView *pView = (CWorldEditView*) GetActiveView ();
	if( pView )	pView->m_sObjectMRS.SetMode( 0 );
}

void CMainFrame::OnViewCurSelect()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CWorldEditView::m_CurType = _CUR_SELECT;

	DxViewPort::GetInstance().SetToolCamera();
	DxViewPort::GetInstance().SetCamCollisionTest( FALSE );

	UpdateMenuAct ();

	CWorldEditView *pView = (CWorldEditView*) GetActiveView ();
	if( pView )	pView->m_sObjectMRS.SetMode( 0 );
}

void CMainFrame::OnViewCurMove()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CWorldEditView::m_CurType = _CUR_MOVE;

	DxViewPort::GetInstance().SetToolCamera();
	DxViewPort::GetInstance().SetCamCollisionTest( FALSE );


	UpdateMenuAct ();

	CWorldEditView *pView = (CWorldEditView*) GetActiveView ();
	if ( pView )
		pView->SetCurEditType ( TRANS_MAT );
}

void CMainFrame::OnViewRotate()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CWorldEditView::m_CurType = _CUR_ROTATE;

	DxViewPort::GetInstance().SetToolCamera();
	DxViewPort::GetInstance().SetCamCollisionTest( FALSE );

	UpdateMenuAct ();

	CWorldEditView *pView = (CWorldEditView*) GetActiveView ();
	if ( pView )
		pView->SetCurEditType ( ROTATE_MAT );
}

void CMainFrame::OnViewScale()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CWorldEditView::m_CurType = _CUR_SCALE;

	DxViewPort::GetInstance().SetToolCamera();
	DxViewPort::GetInstance().SetCamCollisionTest( FALSE );

	UpdateMenuAct ();

	CWorldEditView *pView = (CWorldEditView*) GetActiveView ();
	if ( pView )
		pView->SetCurEditType ( SCALE_MAT );
}

void CMainFrame::OnViewSelectpoint()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CWorldEditView::m_CurType = _CUR_SELECT_POINT;

	DxViewPort::GetInstance().SetToolCamera();
	DxViewPort::GetInstance().SetCamCollisionTest( FALSE );

	UpdateMenuAct ();

	CWorldEditView *pView = (CWorldEditView*) GetActiveView ();
	if( pView )	pView->m_sObjectMRS.SetMode( 0 );
}

void CMainFrame::OnCharMove()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CWorldEditView::m_CurType = _CUR_CHAR_MOVE;

	DxViewPort::GetInstance().SetToolCamera();
	DxViewPort::GetInstance().SetCamCollisionTest( FALSE );

	UpdateMenuAct ();
}

void CMainFrame::OnGameTest()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CWorldEditView::m_CurType = _CUR_GAME_TEST;

	CWorldEditView *pView = (CWorldEditView*) GetActiveView ();	

	DxViewPort::GetInstance().SetGameCamera ();	
	DxViewPort::GetInstance().SetCamCollisionTest( TRUE, pView->GetLandMan() );

	UpdateMenuAct ();
}

void CMainFrame::OnViewMovetoframe()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxFrameMesh *pFrameMesh = pView->GetFrameMesh();
	LPDXFRAME pDxFrame = pFrameMesh->GetCollisionDetectedFrame ();

	if ( pDxFrame )
	{
		D3DXVECTOR3 vToMove;
		vToMove.x = pDxFrame->matCombined._41;
		vToMove.y = pDxFrame->matCombined._42;
		vToMove.z = pDxFrame->matCombined._43;

		DxViewPort::GetInstance().CameraJump ( vToMove );
	}
}

void CMainFrame::OnViewMovetopoint()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	D3DXVECTOR3 vCollisionPos = pView->GetCollisionPos ();
	DxViewPort::GetInstance().CameraJump ( vCollisionPos );
}



void CMainFrame::OnViewCharmovetopoint()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	D3DXVECTOR3 vCollisionPos = pView->GetCollisionPos ();

	D3DXVECTOR3 vPos1 = vCollisionPos + D3DXVECTOR3(0,500.0f,0);
	D3DXVECTOR3 vPos2 = vCollisionPos + D3DXVECTOR3(0,-500.0f,0);
	
	D3DXVECTOR3 vCollision;
	DWORD dwCollisionID;
	BOOL bCollision;


	boost::shared_ptr<NavigationMesh> spNaviMesh = pView->GetLandMan()->GetNaviMesh();

	if ( spNaviMesh )
	{
		// 소수점 정밀도 관련 문제 확인 IsCollision
		spNaviMesh->IsCollision ( vPos1, vPos2, vCollision, dwCollisionID, bCollision );

		if ( bCollision )
		{
			pView->GetCharacter()->InitializeActorMove(vCollision);
		}
	}
}

void CMainFrame::OnViewEditbox()
{
	CWorldEditView::m_CurType = _CUR_EDIT_BOX;
}

void CMainFrame::OnFileNew()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CWorldEditView *pView = (CWorldEditView*) GetActiveView ();

	pView->ResetObjects ();
}

void CMainFrame::OnViewCurTileDraw()
{
	CWorldEditView::m_CurType = _CUR_TILE_DRAW;
}

void CMainFrame::OnViewCurTileGet()
{
	CWorldEditView::m_CurType = _CUR_TILE_GET;
}

void CMainFrame::OnViewCurTileDel()
{
	CWorldEditView::m_CurType = _CUR_TILE_DEL;
}

void CMainFrame::OnViewAll90()
{
	if( IDYES == AfxMessageBox( "모든 셋팅값을 90도 회전 하시겠습니까 ?", MB_YESNO ) )
	{
		CWorldEditView *pView = (CWorldEditView*) GetActiveView ();
		pView->SetObjRotate90();
	}
}

void CMainFrame::OnViewFpsVisible()
{
	g_bFPS_VISIBLE = g_bFPS_VISIBLE ? FALSE : TRUE;

	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuItem ( ID_VIEW_FPS_VISIBLE, g_bFPS_VISIBLE?MF_CHECKED:MF_UNCHECKED );
}

void CMainFrame::OnViewConsoleVisible()
{
	g_bCONSOLE_VISIBLE = g_bCONSOLE_VISIBLE ? FALSE : TRUE;

	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuItem ( ID_VIEW_CONSOLE_VISIBLE, g_bCONSOLE_VISIBLE?MF_CHECKED:MF_UNCHECKED );
}

void CMainFrame::OnViewEditPanel()
{
	g_bEDIT_PANEL = g_bEDIT_PANEL ? FALSE : TRUE;

	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuItem ( ID_VIEW_EDITPANEL, g_bEDIT_PANEL?MF_CHECKED:MF_UNCHECKED );

	if( g_bEDIT_PANEL )
	{
		BOOL bZoomSize = IsZoomed();

		ShowWindow(SW_HIDE);
		UpdateWindow();

		ShowWindow(SW_MINIMIZE);
		
		

		m_wndDialogBar1.ShowWindow( SW_SHOW );


		if( bZoomSize )
		{
			ShowWindow(SW_MAXIMIZE);
		}else{
			ShowWindow(SW_NORMAL);
		}

		ShowWindow(SW_SHOW);
		UpdateWindow();
	}
	else
	{
		BOOL bZoomSize = IsZoomed();

		ShowWindow(SW_HIDE);
		UpdateWindow();

		ShowWindow(SW_MINIMIZE);

		m_wndDialogBar1.ShowWindow( SW_HIDE );

		if( bZoomSize )
		{
			ShowWindow(SW_MAXIMIZE);
		}else{
			ShowWindow(SW_NORMAL);
		}

		ShowWindow(SW_SHOW);
		UpdateWindow();
	}
}

void CMainFrame::OnCheckMipmap()
{
    CWorldEditView *pView = (CWorldEditView*) GetActiveView ();
    TextureManagerUtil::AllTextureCheckMipMap( pView->GetD3DDevice() );
}

void CMainFrame::OnCheckRenderTextureLightmap()
{
	CWorldEditView::m_RenderType = RENDER_TEXTURE_LIGHTMAP;
	UpdateMenuAct ();

	// FX 변경되면 RegistPresetList 를 꼭 해줘야 한다.
	CWorldEditView *pView = (CWorldEditView*) GetActiveView ();
	pView->SetNSLightMapFX_SetRenderMode();
	pView->GetLandMan()->Active_MATERIAL_DATA( pView->GetD3DDevice(), FALSE );
	NSMaterialManager::OnlyVersionUpForTool();
}

void CMainFrame::OnCheckRenderLightmap()
{
	CWorldEditView::m_RenderType = RENDER_LIGHTMAP;
	UpdateMenuAct ();

	// FX 변경되면 RegistPresetList 를 꼭 해줘야 한다.
	CWorldEditView *pView = (CWorldEditView*) GetActiveView ();
	pView->SetNSLightMapFX_SetRenderMode();
	pView->GetLandMan()->Active_MATERIAL_DATA( pView->GetD3DDevice(), FALSE );
	NSMaterialManager::OnlyVersionUpForTool();
}

void CMainFrame::OnCheckRenderLightmapPoint()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CWorldEditView::m_RenderType = RENDER_LIGHTMAP_POINT;
	UpdateMenuAct ();

	// FX 변경되면 RegistPresetList 를 꼭 해줘야 한다.
	CWorldEditView *pView = (CWorldEditView*) GetActiveView ();
	pView->SetNSLightMapFX_SetRenderMode();
	pView->GetLandMan()->Active_MATERIAL_DATA( pView->GetD3DDevice(), FALSE );
	NSMaterialManager::OnlyVersionUpForTool();
}

void CMainFrame::OnCheckRenderLightmapChecker()
{
	CWorldEditView::m_RenderType = RENDER_LIGHTMAP_CHECKER;
	UpdateMenuAct ();

	// FX 변경되면 RegistPresetList 를 꼭 해줘야 한다.
	CWorldEditView *pView = (CWorldEditView*) GetActiveView ();
	pView->SetNSLightMapFX_SetRenderMode();
	pView->GetLandMan()->Active_MATERIAL_DATA( pView->GetD3DDevice(), FALSE );
	NSMaterialManager::OnlyVersionUpForTool();
}

// Water Begin
void CMainFrame::OnCheckWaterRealTimeReflection()
{
	CWorldEditView::m_WaterType = RENDER_WATER_RT_REFLECTION;
	UpdateMenuAct ();
}

void CMainFrame::OnCheckWaterCubeMap()
{
	CWorldEditView::m_WaterType = RENDER_WATER_CUBEMAP;
	UpdateMenuAct ();
}

void CMainFrame::OnCheckWaterDiffuseTexture()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CWorldEditView::m_WaterType = RENDER_WATER_DIFFUSE_TEXTURE;
	UpdateMenuAct ();
}
// Water End

void CMainFrame::PostNcDestroy()
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

    m_cDlgEditPIECE.DestroyWindow();

    CFrameWnd::PostNcDestroy();
}

//void CMainFrame::OnUpdateCheckRenderTextureLightmap(CCmdUI *pCmdUI)
//{
//	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
//	CMenu *pMenu = GetMenu ();
//	pMenu->CheckMenuItem ( ID_CHECK_RENDER_TEXTURE_LIGHTMAP, FALSE );
//	pMenu->CheckMenuItem ( ID_CHECK_RENDER_LIGHTMAP_CHECKER, FALSE );
//
//	pCmdUI->SetCheck ( TRUE );
//}
//
//void CMainFrame::OnUpdateCheckRenderLightmap(CCmdUI *pCmdUI)
//{
//	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
//	CMenu *pMenu = GetMenu ();
//	pMenu->CheckMenuItem ( ID_CHECK_RENDER_LIGHTMAP, FALSE );
//	pMenu->CheckMenuItem ( ID_CHECK_RENDER_LIGHTMAP_CHECKER, FALSE );
//
//	pCmdUI->SetCheck ( TRUE );
//}
//
//void CMainFrame::OnUpdateCheckRenderLightmapChecker(CCmdUI *pCmdUI)
//{
//	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
//	CMenu *pMenu = GetMenu ();
//	pMenu->CheckMenuItem ( ID_CHECK_RENDER_LIGHTMAP, FALSE );
//	pMenu->CheckMenuItem ( ID_CHECK_RENDER_TEXTURE_LIGHTMAP, FALSE );
//
//	pCmdUI->SetCheck ( TRUE );
//}

void CMainFrame::OnLoadingLightmapCheckerTexture()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString szFilter = _T("Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|");
	CFileDialog dlg ( TRUE, _T(".BMP"), NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();
		NSLightMapFX::SetLightMapCheckerName( FilePath.GetString() );
	}
}

void CMainFrame::OnRadiositSettingInfo()
{
	CWorldEditView *pView = (CWorldEditView*) GetActiveView ();
	if ( !pView )
	{
		AfxMessageBox( _T("Error") );
		return;
	}

	DxLandMan* pLandMan = pView->GetLandMan();
	if ( !pLandMan )
	{
		AfxMessageBox( _T("Error") );
		return;
	}

	DWORD dwDiffuseRGB[2][3] = { { 0, 0, 0 },  { 0, 0, 0 } };
	DWORD dwAmbientRGB[2][3] = { { 0, 0, 0 },  { 0, 0, 0 } };
	float fDiffusePower(1.f);
	DWORD dwRAD_Pass( pLandMan->GetRAD_Pass() );
	float fRAD_Reflectance( pLandMan->GetRAD_Reflectance() );
	float fRAD_FogBlend( pLandMan->GetRAD_FogBlend() );
	DWORD dwRAD_FogLength( pLandMan->GetRAD_FogLength() );

	DXLIGHT* pLight(NULL);
	DxLightMan* pLightMan = pLandMan->GetLightMan();
	if ( pLightMan )
	{
		fDiffusePower = pLightMan->GetLightMapDirectionDiffusePower();

		pLight = pLightMan->GetDirectNoonLight();
		if ( pLight )
		{
			dwDiffuseRGB[0][0] = static_cast<DWORD>( 255 * pLight->m_Light.Diffuse.r );
			dwDiffuseRGB[0][1] = static_cast<DWORD>( 255 * pLight->m_Light.Diffuse.g );
			dwDiffuseRGB[0][2] = static_cast<DWORD>( 255 * pLight->m_Light.Diffuse.b );
			dwAmbientRGB[0][0] = static_cast<DWORD>( 255 * pLight->m_Light.Ambient.r );
			dwAmbientRGB[0][1] = static_cast<DWORD>( 255 * pLight->m_Light.Ambient.g );
			dwAmbientRGB[0][2] = static_cast<DWORD>( 255 * pLight->m_Light.Ambient.b );
		}

		pLight = pLightMan->GetDirectNightLight();
		if ( pLight )
		{
			dwDiffuseRGB[1][0] = static_cast<DWORD>( 255 * pLight->m_Light.Diffuse.r );
			dwDiffuseRGB[1][1] = static_cast<DWORD>( 255 * pLight->m_Light.Diffuse.g );
			dwDiffuseRGB[1][2] = static_cast<DWORD>( 255 * pLight->m_Light.Diffuse.b );
			dwAmbientRGB[1][0] = static_cast<DWORD>( 255 * pLight->m_Light.Ambient.r );
			dwAmbientRGB[1][1] = static_cast<DWORD>( 255 * pLight->m_Light.Ambient.g );
			dwAmbientRGB[1][2] = static_cast<DWORD>( 255 * pLight->m_Light.Ambient.b );
		}
	}

	TSTRINGSTREAM ssInfo;
	ssInfo << _T("Pass : ");
	ssInfo << dwRAD_Pass;
	ssInfo << _T("\r\n");

	ssInfo << _T("Reflectance : ");
	ssInfo << fRAD_Reflectance;
	ssInfo << _T("\r\n");

	ssInfo << _T("FogBlend : ");
	ssInfo << fRAD_FogBlend;
	ssInfo << _T("\r\n");

	ssInfo << _T("FogLength : ");
	ssInfo << dwRAD_FogLength;
	ssInfo << _T("\r\n");

	ssInfo << _T("Diffuse Power : ");
	ssInfo << fDiffusePower;
	ssInfo << _T("\r\n");

	ssInfo << _T("Day Diffuse : ");
	ssInfo << dwDiffuseRGB[0][0];
	ssInfo << _T(",");
	ssInfo << dwDiffuseRGB[0][1];
	ssInfo << _T(",");
	ssInfo << dwDiffuseRGB[0][2];
	ssInfo << _T("\r\n");

	ssInfo << _T("Day Ambient : ");
	ssInfo << dwAmbientRGB[0][0];
	ssInfo << _T(",");
	ssInfo << dwAmbientRGB[0][1];
	ssInfo << _T(",");
	ssInfo << dwAmbientRGB[0][2];
	ssInfo << _T("\r\n");

	ssInfo << _T("Night Diffuse : ");
	ssInfo << dwDiffuseRGB[1][0];
	ssInfo << _T(",");
	ssInfo << dwDiffuseRGB[1][1];
	ssInfo << _T(",");
	ssInfo << dwDiffuseRGB[1][2];
	ssInfo << _T("\r\n");

	ssInfo << _T("Night Ambient : ");
	ssInfo << dwAmbientRGB[1][0];
	ssInfo << _T(",");
	ssInfo << dwAmbientRGB[1][1];
	ssInfo << _T(",");
	ssInfo << dwAmbientRGB[1][2];
	ssInfo << _T("\r\n");

	AfxMessageBox( ssInfo.str().c_str() );
}

void CMainFrame::OnUpdateAssetView(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck( m_cDlgAsset.IsWindowVisible() );
}

void CMainFrame::OnAssetView()
{
    if ( m_cDlgAsset.IsWindowVisible() )
    {
        m_cDlgAsset.ShowWindow( SW_HIDE );
    }
    else
    {
        m_cDlgAsset.ShowWindow( SW_SHOW );
    }
}

void CMainFrame::OnClose()
{
    SavePosition(this);
    m_cDlgAsset.SavePosition(&m_cDlgAsset);
    m_cDlgEditPIECE.SavePosition(&m_cDlgEditPIECE);
    __super::OnClose();
}

void CMainFrame::OnToolBarSpecCam()
{
	CWorldEditView::g_bSpecCamaraMode = !CWorldEditView::g_bSpecCamaraMode;
}

void CMainFrame::OnToolBarSmallRefresh()
{
	CWorldEditView *pView = (CWorldEditView*) GetActiveView ();

	TSTRING strFullName = DxLandMan::GetPath();
	strFullName += pView->GetLandMan()->GetNameWLD();
	NSLandSettingLua::Load( pView->GetD3DDevice(), strFullName.c_str() );
	if ( CWorldEditView::m_CurType == _CUR_GAME_TEST )
	{
		DxViewPort::GetInstance().SetGameCamera();
	}

	NSMaterialManager::SimpleReload( pView->GetD3DDevice() );

	// FX 변경되면 RegistPresetList 를 꼭 해줘야 한다.
	pView->GetLandMan()->Active_MATERIAL_DATA( pView->GetD3DDevice(), FALSE );
	NSMaterialManager::OnlyVersionUpForTool();
}

void CMainFrame::OnViewCharChange()
{
	CWorldEditView *pView = (CWorldEditView*)GetActiveView ();
	if( !pView )
		return;

	CString szFilter = _T("Charactor File (*.chf)|*.chf|");

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE, _T(".chf"), NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	dlg.m_ofn.lpstrInitialDir = DxSkinCharDataContainer::GetInstance().GetPath();
	if ( dlg.DoModal() == IDOK )
	{
		pView->SkinLoadForTool( dlg.GetFileName().GetString() ); 

		//TSTRING strCHF = dlg.GetFileName();

		//SAFE_DELETE( pView->m_pSkinChar );

		//DxSkinCharData* pSkinCharData = DxSkinCharDataContainer::GetInstance( pView->GetD3dDevice() )->LoadData( strCHF.c_str(), TRUE );
		//if( !pSkinCharData )
		//	return;

		//pView->m_pSkinChar = new DxSkinChar;
		//pView->m_pSkinChar->SetCharData( pSkinCharData, pView->GetD3dDevice(), EMSCD_SET_BASE_CPS | EMSCD_MESH_THREAD | EMSCD_JIGGLE_BONE );
	}    
}

void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
    __super::OnShowWindow(bShow, nStatus);

    static bool bOnce = true;

    if (bShow && bOnce)
    {
        LoadPosition(this);
        bOnce = false;
    }
}

void CMainFrame::OnViewFogOff()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	g_bFORCE_FOG_OFF = g_bFORCE_FOG_OFF ? FALSE : TRUE;

	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuItem ( ID_VIEW_FOG_OFF, g_bFORCE_FOG_OFF ? MF_CHECKED : MF_UNCHECKED );

	if ( CWorldEditView::g_bMiniMap )
	{
		DxViewPort::GetInstance().SetToolCameraMiniMap();
	}
	else
	{
		DxViewPort::GetInstance().SetToolCamera();
	}
	DxViewPort::GetInstance().SetCamCollisionTest( FALSE );
}

void CMainFrame::OnLodOff()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	NSPieceQuickSort::g_bViewAlphaLOD = NSPieceQuickSort::g_bViewAlphaLOD ? FALSE : TRUE;

	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuItem ( ID_LOD_OFF, NSPieceQuickSort::g_bViewAlphaLOD ? MF_UNCHECKED : MF_CHECKED );
}

void CMainFrame::OnPieceInstancingOff()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	NSPieceQuickSort::g_bPieceInstancing = NSPieceQuickSort::g_bPieceInstancing ? FALSE : TRUE;

	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuItem ( ID_INSTANCING_OFF, NSPieceQuickSort::g_bPieceInstancing ? MF_UNCHECKED : MF_CHECKED );
}

void CMainFrame::OnChangeTempCubemap()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString szFilter = _T("Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|");
	CFileDialog dlg ( TRUE, _T(".BMP"), NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetFileName ();
		NSCubeMapUtil::SetTempCubeMapName( FilePath.GetString() );
	}
}

void CMainFrame::OnRenderPS_3_0_HIGH()
{
	CWorldEditView::m_dwRenderPSType = ID_RENDER_PS_3_0_HIGH;
	UpdateMenuAct ();
}

void CMainFrame::OnRenderPS_3_0()
{
	CWorldEditView::m_dwRenderPSType = ID_RENDER_PS_3_0;
	UpdateMenuAct ();
}

void CMainFrame::OnRenderPS_2_0()
{
	CWorldEditView::m_dwRenderPSType = ID_RENDER_PS_2_0;
	UpdateMenuAct ();
}

void CMainFrame::OnRenderPS_1_1()
{
	CWorldEditView::m_dwRenderPSType = ID_RENDER_PS_1_1;
	UpdateMenuAct ();
}

void CMainFrame::OnRenderPS_FIXED()
{
	CWorldEditView::m_dwRenderPSType = ID_RENDER_FIXED;
	UpdateMenuAct ();
}

void CMainFrame::OnRenderPS_Char_SS()
{
	CWorldEditView::m_dwRenderCharType = ID_RENDER_CHAR_SS;
	UpdateMenuAct ();
}

void CMainFrame::OnRenderPS_Char_Default()
{
	CWorldEditView::m_dwRenderCharType = ID_RENDER_CHAR_DEFAULT;
	UpdateMenuAct ();
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	//switch(pMsg->message)
	//{
	//case WM_UPDATE_LOG_CONSOLE:
	//	{
	//		char *pBuffer = (char *)pMsg->lParam;
	//		if(pBuffer)
	//		{
	//			SAFE_DELETE_ARRAY(pBuffer);
	//		}
	//	}
	//	break;
	//default:
	//	break;
	//}

	return __super::PreTranslateMessage(pMsg);
}