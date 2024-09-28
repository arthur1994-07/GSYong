// MainFrm.cpp : implementation of the CMainFrame class
//
#include "stdafx.h"
#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/Util/SystemInfo.h"
#include "../SigmaCore/File/FileUtil.h"
#include "../SigmaCore/Log/LogMan.h"

#include "../EngineLib/DxEffect/Frame/DxEffectMan.h"
#include "../EngineLib/DxMeshs/SkinMesh/_new/DxSkinMesh9.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinObject.h"
#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"
#include "../EngineLib/DxLand/DxLandMan.h"

#include "CharEdit.h"
#include "CharEditDoc.h"
#include "CharEditView.h"
#include "./Properties/CharPage.h"
#include "./Properties/sheetWithTabChar.h"
#include "./Dialog/AmbientDlg.h"
#include "./Dialog/DlgEdit.h"
#include "./DlgLog.h"
#include "./MainFrm.h"
#include "./Dialog/AssetDlg.h"

#pragma warning(disable:4996)

#include "../SigmaCore/DebugInclude.h"


//	Note	:	디버그용,
extern int		g_nITEMLEVEL;
extern BOOL		g_bCHAR_EDIT_RUN;
extern BOOL		g_bEFFECT_SPHERE_VISIBLE;
extern BOOL     g_bAmbientTest_CHAREDIT;
extern DWORD    g_dwAmbient_CHAREDIT;
BOOL			g_bAABBBOX_VISIBLE = TRUE;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_WIREFRAME, OnViewWireframe)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WIREFRAME, OnUpdateViewWireframe)
	ON_COMMAND(ID_VIEW_PLANE, OnViewPlane)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PLANE, OnUpdateViewPlane)
	ON_COMMAND(ID_VIEW_MOVE_PIECE, OnViewMovePiece)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MOVE_PIECE, OnUpdateViewMovePiece)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_ITEMLEVEL_DEFAULT, OnItemlevelDefault)
	ON_COMMAND(ID_ITEMLEVEL_LEVEL1, OnItemlevelLevel1)
	ON_COMMAND(ID_ITEMLEVEL_LEVEL2, OnItemlevelLevel2)
	ON_COMMAND(ID_ITEMLEVEL_LEVEL3, OnItemlevelLevel3)
	ON_COMMAND(ID_ITEMLEVEL_LEVEL4, OnItemlevelLevel4)
	ON_COMMAND(ID_ITEMLEVEL_LEVEL5, OnItemlevelLevel5)
	ON_COMMAND(ID_ITEMLEVEL_LEVEL6, OnItemlevelLevel6)
	ON_COMMAND(ID_ITEMLEVEL_LEVEL7, OnItemlevelLevel7)
	ON_COMMAND(ID_ITEMLEVEL_LEVEL8, OnItemlevelLevel8)
	ON_COMMAND(ID_ITEMLEVEL_LEVEL9, OnItemlevelLevel9)
	ON_COMMAND(ID_ITEMLEVEL_LEVEL10, OnItemlevelLevel10)
	ON_COMMAND(ID_ITEMLEVEL_LEVEL11, OnItemlevelLevel11)
	ON_COMMAND(ID_VIEW_AABBBOX_VISIBLE, OnViewAabbboxVisible)
	ON_COMMAND(ID_VIEW_EFFECT_SPHERE_VISIBLE, OnViewEffectSphereVisible)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BGTEX, OnUpdateViewBgtex)
	ON_COMMAND(ID_VIEW_BGTEX, OnViewBgtex)
	ON_COMMAND(ID_LOAD_BGTEX, OnLoadBgtex)
	ON_COMMAND( ID_SAVE_SOUND_CSV, OnSaveSoundCsv)
    ON_COMMAND(ID_VIEW_AMBIENT_CHANGE, &CMainFrame::OnViewAmbientChange)
    ON_WM_CLOSE()
    ON_COMMAND(ID_MENU_LOG_WINDOW, &CMainFrame::OnMenuLogWindow)
    ON_COMMAND(ID_ASSETVIEW, &CMainFrame::OnAssetview)
    ON_UPDATE_COMMAND_UI(ID_ASSETVIEW, &CMainFrame::OnUpdateAssetview)
    ON_WM_SHOWWINDOW()
	ON_COMMAND(ID_RENDER_PS_3_0, OnRenderPS_3_0)
	ON_COMMAND(ID_RENDER_PS_2_0, OnRenderPS_2_0)
	ON_COMMAND(ID_RENDER_PS_1_1, OnRenderPS_1_1)
	ON_COMMAND(ID_RENDER_FIXED_HIGH, OnRenderPS_FIXED_HIGH)
	ON_COMMAND(ID_RENDER_FIXED, OnRenderPS_FIXED)
	ON_COMMAND(ID_FIND_INVERSE_SCALE, OnFindInverseScale)
	ON_COMMAND(ID_FIND_DONT_HAVE_TEXTURE, OnFindDontHaveTexture)
	ON_COMMAND(ID_MENU_LAND_LOAD, OnMenuLandLoad)
	ON_COMMAND(ID_MENU_LAND_ERASE, OnMenuLandErase)
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
    CSettingIni(_T("CharEdit.ini"), _T("MainFrame"))
,   m_pLogDlg(NULL)
,   m_pAssetDlg(0)
{
	g_nITEMLEVEL			 = 0;
	g_bCHAR_EDIT_RUN		 = TRUE;
	g_bEFFECT_SPHERE_VISIBLE = TRUE;

	m_fRunSpeed  = 0.0f;
	m_fWalkSpeed = 0.0f;
}

CMainFrame::~CMainFrame()
{
	
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;    

	/*
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	*/

	if (!m_wndTrueToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndTrueToolBar.LoadToolBar(IDR_MAINFRAME, IDB_BITMAP_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	/*
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	*/

	m_wndTrueToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndTrueToolBar);

	//	Note : Edit Bar
	//
	CString StrTitle1 = CString("Edit Bar");
	if ( !m_wndDialogBar1.Create ( this, &m_cDlgEdit, StrTitle1, IDD_DIALOG_EDIT ) )
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;      // fail to create

	}

    m_wndDialogBar1.SetBarStyle(m_wndDialogBar1.GetBarStyle()|CBRS_TOOLTIPS|CBRS_SIZE_DYNAMIC);
	m_wndDialogBar1.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndDialogBar1,AFX_IDW_DOCKBAR_RIGHT);


	//	Note : Edit Bar
	//
	CString StrTitle2 = CString("Time Line Bar");
	if ( !m_wndDialogBar2.Create ( this, &m_cDlgTimeLine, StrTitle2, IDD_DIALOG_TIMELINE ) )
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;      // fail to create

	}

    m_wndDialogBar2.SetBarStyle(m_wndDialogBar2.GetBarStyle()|CBRS_TOOLTIPS|CBRS_SIZE_DYNAMIC);
	m_wndDialogBar2.EnableDocking(CBRS_ALIGN_RIGHT);
	DockControlBar(&m_wndDialogBar2,AFX_IDW_DOCKBAR_BOTTOM);


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
	}

	strcpy ( m_szAppPath, m_strAppPath.GetString() );

	CMenu *pMenu = GetMenu();
	pMenu->CheckMenuItem ( ID_VIEW_WIREFRAME, g_RendState.bWireFrame?MF_CHECKED:MF_UNCHECKED );
	pMenu->CheckMenuItem ( ID_VIEW_PLANE, g_RendState.bFlane?MF_CHECKED:MF_UNCHECKED );
	pMenu->CheckMenuItem ( ID_VIEW_BGTEX, g_RendState.bBGTex?MF_CHECKED:MF_UNCHECKED );

	//switch ( DxSkinMesh9::m_SkinningMethod)
	//{
	//case D3DINDEXED:
	//	pMenu->CheckMenuRadioItem ( ID_SKIN_NONINDEX, ID_SKIN_SOFTWARE, ID_SKIN_INDEXED, MF_BYCOMMAND );
	//	break;

	//case D3DNONINDEXED:
	//	pMenu->CheckMenuRadioItem ( ID_SKIN_NONINDEX, ID_SKIN_SOFTWARE, ID_SKIN_NONINDEX, MF_BYCOMMAND );
	//	break;

	//case SOFTWARE:
	//	pMenu->CheckMenuRadioItem ( ID_SKIN_NONINDEX, ID_SKIN_SOFTWARE, ID_SKIN_SOFTWARE, MF_BYCOMMAND );
	//	break;
	//};

	//	Note : 아이템 레벨 용.
	pMenu->CheckMenuRadioItem ( ID_ITEMLEVEL_DEFAULT, ID_ITEMLEVEL_LEVEL9, ID_ITEMLEVEL_DEFAULT, MF_BYCOMMAND );    

    m_pLogDlg = new DlgLog;
    m_pLogDlg->Create(IDD_DIALOG_LOG);
    m_pLogDlg->ShowWindow(SW_SHOW);
    sc::LogMan::getInstance()->attachEditControl(m_pLogDlg->GetEditWindow(), m_pLogDlg->GetEditWindow());

	//	Note : Init
	//
	UpdateMenuAct ();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
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

void CMainFrame::SetTimeLine ( DWORD dwStartKey, DWORD dwEndKey, DWORD dwUnitTime, BYTE aniType /*= 1*/  )
{
	m_cDlgTimeLine.SetTimeLine ( dwStartKey, dwEndKey, dwUnitTime, aniType );
}

// User 가 TimeLine 의 시간을 변경한다면 그와 연관된 작업을 한다.
void CMainFrame::ChangeTimeOfTimeLine( DWORD dwToTime )
{
	m_cDlgEdit.ChangeTimeOfTimeLine( dwToTime );
}

// User 가 CDlgTimeLine 의 OnButtonPlay 을 누른다면 그와 연관된 작업을 한다.
void CMainFrame::OnButtonPlayOfCDlgTimeLine()
{
	m_cDlgEdit.OnButtonPlayOfCDlgTimeLine();
}

void CMainFrame::ReStoreDialogBar ()
{
	m_cDlgEdit.ReFlash ();
	
	CCharEditView *pView = (CCharEditView*) GetActiveView ();
	DxSkinObject *pSkinObject = pView->GetSkinObject ();

	const DxSkinAnimation* pSelectedAniCont = pSkinObject->GETCURANIM ();
	if ( !pSelectedAniCont )	return;

	DWORD dwThisTime = 0;
	DWORD dwUnitTime = pSelectedAniCont->m_UNITTIME;

	if ( dwUnitTime > 0 && pSelectedAniCont )
	{
		SetTimeLine ( pSelectedAniCont->m_dwSTime/dwUnitTime,
			pSelectedAniCont->m_dwETime/dwUnitTime, dwUnitTime );

		float fTime = (float)fmod ( pSkinObject->GETCURTIME(), (float)pSelectedAniCont->m_dwETime );
		dwThisTime = (DWORD) ( fTime / dwUnitTime );
	}

	m_cDlgTimeLine.SetTimeState ( dwThisTime );
}



void CMainFrame::OnViewWireframe() 
{
	// TODO: Add your command handler code here
	g_RendState.bWireFrame = !g_RendState.bWireFrame;
}

void CMainFrame::OnUpdateViewWireframe(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck ( g_RendState.bWireFrame );
}

void CMainFrame::OnViewPlane() 
{
	// TODO: Add your command handler code here
	g_RendState.bFlane = !g_RendState.bFlane;
}

void CMainFrame::OnUpdateViewPlane(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(g_RendState.bFlane);
}

void CMainFrame::OnViewMovePiece() 
{
	// TODO: Add your command handler code here
	g_RendState.bMovePiece = !g_RendState.bMovePiece;
}

void CMainFrame::OnUpdateViewMovePiece(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck ( g_RendState.bMovePiece );
}

void CMainFrame::OnItemlevelDefault()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	g_nITEMLEVEL = 0;

	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuRadioItem ( ID_ITEMLEVEL_DEFAULT, ID_ITEMLEVEL_LEVEL11, ID_ITEMLEVEL_DEFAULT, MF_BYCOMMAND );

	SetTitleName ( "Item Level Default" );
}

void CMainFrame::OnItemlevelLevel1()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	g_nITEMLEVEL = 1;

	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuRadioItem ( ID_ITEMLEVEL_DEFAULT, ID_ITEMLEVEL_LEVEL11, ID_ITEMLEVEL_LEVEL1, MF_BYCOMMAND );

	SetTitleName ( "Item Level 1" );
}

void CMainFrame::OnItemlevelLevel2()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	g_nITEMLEVEL = 2;

	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuRadioItem ( ID_ITEMLEVEL_DEFAULT, ID_ITEMLEVEL_LEVEL11, ID_ITEMLEVEL_LEVEL2, MF_BYCOMMAND );

	SetTitleName ( "Item Level 2" );
}

void CMainFrame::OnItemlevelLevel3()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	g_nITEMLEVEL = 3;

	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuRadioItem ( ID_ITEMLEVEL_DEFAULT, ID_ITEMLEVEL_LEVEL11, ID_ITEMLEVEL_LEVEL3, MF_BYCOMMAND );

	SetTitleName ( "Item Level 3" );
}

void CMainFrame::OnItemlevelLevel4()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	g_nITEMLEVEL = 4;

	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuRadioItem ( ID_ITEMLEVEL_DEFAULT, ID_ITEMLEVEL_LEVEL11, ID_ITEMLEVEL_LEVEL4, MF_BYCOMMAND );

	SetTitleName ( "Item Level 4" );
}

void CMainFrame::OnItemlevelLevel5()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	g_nITEMLEVEL = 5;

	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuRadioItem ( ID_ITEMLEVEL_DEFAULT, ID_ITEMLEVEL_LEVEL11, ID_ITEMLEVEL_LEVEL5, MF_BYCOMMAND );

	SetTitleName ( "Item Level 5" );
}

void CMainFrame::OnItemlevelLevel6()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	g_nITEMLEVEL = 6;

	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuRadioItem ( ID_ITEMLEVEL_DEFAULT, ID_ITEMLEVEL_LEVEL11, ID_ITEMLEVEL_LEVEL6, MF_BYCOMMAND );

	SetTitleName ( "Item Level 6" );
}

void CMainFrame::OnItemlevelLevel7()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	g_nITEMLEVEL = 7;

	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuRadioItem ( ID_ITEMLEVEL_DEFAULT, ID_ITEMLEVEL_LEVEL11, ID_ITEMLEVEL_LEVEL7, MF_BYCOMMAND );

	SetTitleName ( "Item Level 7" );
}

void CMainFrame::OnItemlevelLevel8()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	g_nITEMLEVEL = 8;

	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuRadioItem ( ID_ITEMLEVEL_DEFAULT, ID_ITEMLEVEL_LEVEL11, ID_ITEMLEVEL_LEVEL8, MF_BYCOMMAND );

	SetTitleName ( "Item Level 8" );
}

void CMainFrame::OnItemlevelLevel9()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	g_nITEMLEVEL = 9;

	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuRadioItem ( ID_ITEMLEVEL_DEFAULT, ID_ITEMLEVEL_LEVEL11, ID_ITEMLEVEL_LEVEL9, MF_BYCOMMAND );

	SetTitleName ( "Item Level 9" );
}

void CMainFrame::OnItemlevelLevel10()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	g_nITEMLEVEL = 10;

	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuRadioItem ( ID_ITEMLEVEL_DEFAULT, ID_ITEMLEVEL_LEVEL11, ID_ITEMLEVEL_LEVEL10, MF_BYCOMMAND );

	SetTitleName ( "Item Level 10" );
}

void CMainFrame::OnItemlevelLevel11()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	g_nITEMLEVEL = 11;

	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuRadioItem ( ID_ITEMLEVEL_DEFAULT, ID_ITEMLEVEL_LEVEL11, ID_ITEMLEVEL_LEVEL11, MF_BYCOMMAND );

	SetTitleName ( "Item Level 11" );
}

void CMainFrame::SetTitleName ( CString Name )
{
	CMainFrame *pFrame	= (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	CCharEditDoc *pDoc	= (CCharEditDoc*) pView->GetDocument();

	TSTRING strTitle;
	strTitle = Name;
	strTitle += _T(" - [ver.");
	strTitle += sc::getFileVersion();
	strTitle += _T("]");

	pDoc->SetTitle ( strTitle.c_str() );
}

void CMainFrame::OnViewAabbboxVisible()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	g_bAABBBOX_VISIBLE =  g_bAABBBOX_VISIBLE ? FALSE : TRUE;

	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuItem ( ID_VIEW_AABBBOX_VISIBLE, g_bAABBBOX_VISIBLE ? MF_CHECKED : MF_UNCHECKED );
}

void CMainFrame::OnViewEffectSphereVisible()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	g_bEFFECT_SPHERE_VISIBLE = g_bEFFECT_SPHERE_VISIBLE ? FALSE : TRUE;

	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuItem ( ID_VIEW_EFFECT_SPHERE_VISIBLE, g_bEFFECT_SPHERE_VISIBLE ? MF_CHECKED : MF_UNCHECKED );
}

void CMainFrame::OnUpdateViewBgtex(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck(g_RendState.bBGTex);
}

void CMainFrame::OnViewBgtex()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	g_RendState.bBGTex = !g_RendState.bBGTex;
}

void CMainFrame::OnLoadBgtex()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CCharEditView *pView = (CCharEditView*) GetActiveView ();

	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, OFN_HIDEREADONLY, szFilter, this );
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetPathName();

		pView->LoadBGTexture( FilePath.GetString() );
	}
}

void CMainFrame::OnSaveSoundCsv()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString strPath = DxSkinAnimationManager::GetInstance().GetPath().c_str();

	CFileFind FFind;

	std::string strWildcard ( strPath );
	strWildcard += _T("\\*.*");

	BOOL bFind ( TRUE );

	if ( !FFind.FindFile( strWildcard.c_str() ) )
	{
		MessageBox( "DxSkinAnimationManager::GetInstance().GetPath()" );
		return;
	}

	CString strCsvPath = DxSkinAnimationManager::GetInstance().GetPath().c_str();
	strCsvPath += _T( "Sound.csv" );

	std::fstream SFile;
	SFile.open ( strCsvPath.GetString(), std::ios_base::out );
	SFile << "File Name" << ",";
	for( INT i = 0; i < MAXPLAYFRAME - 1; i++ )
	{
		SFile << "Sound File " << i << ",";
	}
	SFile << std::endl;

	CCharEditView *pView = (CCharEditView*) GetActiveView ();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice ();

	CCharPage cCharPage;	
	CString strComp( "cfg" );
	while( bFind )
	{
		bFind = FFind.FindNextFile ();

		if ( FFind.IsDots() ) continue;

		if ( !FFind.IsDirectory() )
		{
			CString strNAME = FFind.GetFileName();			
			strNAME.MakeLower ();
			CString strPATH = FFind.GetFilePath();

			CString strTerm;
			AfxExtractSubString( strTerm, strNAME, 1, '.');
			
			if( strTerm != strComp )
				continue;

            SFile << strNAME << ",";

			SANIMCONINFO AnimInfo;
			AnimInfo.LoadFile ( strNAME.GetString(), pd3dDevice );

			std::string strSound;
			for( INT i = 0; i < AnimInfo.m_ChaSoundData.m_PlayFrameCount; i++ )
			{
				if( AnimInfo.m_ChaSoundData.m_szFileName[ i ] == "" )
					continue;
					
				strSound = AnimInfo.m_ChaSoundData.m_szFileName[ i ];
				SFile << strSound << ",";
			}
			SFile << std::endl;
		}
	}
	SFile.close();
}

void CMainFrame::OnViewAmbientChange()
{
    // TODO: 여기에 명령 처리기 코드를 추가합니다.
    CCharEditView *pView = (CCharEditView*) GetActiveView ();

	CAmbientDlg sDlg;
    sDlg.m_dwAmbient = g_dwAmbient_CHAREDIT;
    sDlg.m_bAmbientTest = g_bAmbientTest_CHAREDIT;

	if ( sDlg.DoModal() == IDOK )
	{
		g_dwAmbient_CHAREDIT = sDlg.m_dwAmbient;
        g_bAmbientTest_CHAREDIT = sDlg.m_bAmbientTest;
	}
}

void CMainFrame::OnClose()
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    sc::LogMan::getInstance()->releaseInstance();       

    SavePosition(this);

    if (m_pAssetDlg)
    {
        m_pAssetDlg->SavePosition(m_pAssetDlg);
    }

    SAFE_DELETE(m_pLogDlg);
    SAFE_DELETE(m_pAssetDlg);

    CFrameWnd::OnClose();
}

void CMainFrame::OnMenuLogWindow()
{
    // TODO: 여기에 명령 처리기 코드를 추가합니다.
    if (m_pLogDlg)
        m_pLogDlg->ShowWindow(SW_SHOW);
}

void CMainFrame::OnAssetview()
{
    // TODO: 여기에 명령 처리기 코드를 추가합니다.
    if ( m_pAssetDlg == 0 )
    {
        m_pAssetDlg = new CAssetDlg;
        m_pAssetDlg->Create(IDD_DIALOG_ASSET);
    }
    else
    {
        if ( m_pAssetDlg->IsWindowVisible() )
        {
            m_pAssetDlg->ShowWindow( SW_HIDE );
        }
        else
        {
            m_pAssetDlg->ShowWindow( SW_SHOW );
        }        
    }
}

void CMainFrame::OnUpdateAssetview(CCmdUI *pCmdUI)
{
    // TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
    if ( m_pAssetDlg )
    {
        pCmdUI->SetCheck( m_pAssetDlg->IsWindowVisible() );
    }
    else
    {
        pCmdUI->SetCheck(FALSE);
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

void CMainFrame::OnRenderPS_3_0()
{
	CCharEditView::m_dwRenderPSType = ID_RENDER_PS_3_0;
	RENDERPARAM::g_emCharacterQulity = TnL_CHAR_PS_3_0;

	//ResetSkinPiece();
	UpdateMenuAct ();
}

void CMainFrame::OnRenderPS_2_0()
{
	CCharEditView::m_dwRenderPSType = ID_RENDER_PS_2_0;
	RENDERPARAM::g_emCharacterQulity = TnL_CHAR_PS_2_0;

	//ResetSkinPiece();
	UpdateMenuAct ();
}

void CMainFrame::OnRenderPS_1_1()
{
	CCharEditView::m_dwRenderPSType = ID_RENDER_PS_1_1;
	RENDERPARAM::g_emCharacterQulity = TnL_CHAR_PS_1_1;

	//ResetSkinPiece();
	UpdateMenuAct ();
}

void CMainFrame::OnRenderPS_FIXED_HIGH()
{
	DxEffectMan::GetInstance().SetSkinDetail( SKD_BEST );
	CCharEditView::m_dwRenderPSType = ID_RENDER_FIXED_HIGH;
	RENDERPARAM::g_emCharacterQulity = TnL_CHAR_FIXED_HIGH;

	//ResetSkinPiece();
	UpdateMenuAct ();
}

void CMainFrame::OnRenderPS_FIXED()
{
	DxEffectMan::GetInstance().SetSkinDetail( SKD_NONE );
	CCharEditView::m_dwRenderPSType = ID_RENDER_FIXED;
	RENDERPARAM::g_emCharacterQulity = TnL_CHAR_FIXED;

	//ResetSkinPiece();
	UpdateMenuAct ();
}

void CMainFrame::UpdateMenuAct ()
{
	CMenu *pMenu = GetMenu ();

	pMenu->CheckMenuRadioItem ( ID_RENDER_PS_3_0, ID_RENDER_FIXED, CCharEditView::m_dwRenderPSType, MF_BYCOMMAND );
}

void CMainFrame::ResetSkinPiece()
{
	CCharEditView *pView = (CCharEditView*) GetActiveView ();

	DxSkinPieceContainer::GetInstance().CleanUp ();
	DxSkinMeshManager::GetInstance().DeleteCacheDataAll();

	DxSkinPiece* pPiece = pView->GetSkinPiece();
	if ( pPiece )
		pPiece->ReSetSkinMesh( pView->GetD3dDevice() );

	DxSkinChar* pSkinChar = pView->GetSkinChar();
	DxSkinCharData* pSkinCharData = pView->GetSkinCharData();
	pSkinChar->SetCharData ( pSkinCharData, pView->GetD3dDevice(), false, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );

	DxVehicle* pVehicle = pView->GetVehicle();
	DxVehicleData* pVehicleData = pView->GetVehicleData();
	if ( pVehicle && pVehicleData )
	{
		pVehicle->SetVehiclePartData ( pVehicleData, pView->GetD3dDevice() );
	}
}

void CMainFrame::OnFindInverseScale()
{
	DxSkinMeshManager::GetInstance().OnFindInverseScale();
}

void CMainFrame::OnFindDontHaveTexture()
{
	DxSkinPieceContainer::GetInstance().OnFindDontHaveTexture();
	DxAttBoneDataContainer::GetInstance().OnFindDontHaveTexture();
	DxVehicleDataContainer::GetInstance().OnFindDontHaveTexture();

	DxSkinPieceContainer::GetInstance().Dialog_ExportEffTexture_NoHave_TextureName();
}

//
static void CreateChfBillboard( DWORD_PTR pd3dDevice, const TCHAR* FileName, const TCHAR* pExt )
{
	TSTRING strName = FileName;
	std::transform( strName.begin(), strName.end(), strName.begin(), tolower );

	strName = sc::file::getFileExt( strName );
	if ( strName != pExt )
		return;

	DxSkinCharDataContainer::GetInstance().CreateChfBillboard( reinterpret_cast<LPDIRECT3DDEVICEQ>( pd3dDevice ), FileName );
}

//void CMainFrame::OnCreateChfBillboard()
//{
//	CCharEditView *pView = (CCharEditView*) GetActiveView ();
//
//	// 정말 비 정상적인 경우 폴더가 없을 수도 있으므로 만들도록 한다.
//	CreateDirectory ( DxSkinChar::GetBillboardPath().c_str(), NULL );
//
//	sc::CFileFindTree	m_sFileTree;
//	m_sFileTree.CreateTree( DxSkinCharDataContainer::GetInstance().GetPath(), FALSE );
//	m_sFileTree.SomeWorkSameExt( CreateChfBillboard, reinterpret_cast<DWORD_PTR>( pView->GetD3dDevice() ), "chf" );
//}

void CMainFrame::OnMenuLandLoad()
{
	CCharEditView *pView = (CCharEditView*) GetActiveView ();

	//	Note : 파일 필터를 정의한다.
	CString szFilter = "World ( *.wld ) |*.wld|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".wld",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CCharEditView*)pView );

	dlg.m_ofn.lpstrInitialDir = DxLandMan::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();
		pView->LoadLandFile( pView->GetD3dDevice(), sFileName.GetString() );
	}
}

void CMainFrame::OnMenuLandErase()
{
	CCharEditView *pView = (CCharEditView*) GetActiveView ();
	pView->EraseLand();
	pView->ResetSkinPos();

	D3DXVECTOR3 vPos(0.f,0.f,0.f);
	DxViewPort::GetInstance().CameraJump( vPos );
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
