// WeatherPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"

#include "ColorPickerDlg.h"
#include "sheetWithTabMapEdit.h"

#include "../EngineLib/DxLand/DxSetLandMan.h"

#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "MapEditGridPage.h"
#include "MapEditHeightPage.h"
#include "MapEditTilePage.h"
#include "MapEditPiecePage.h"

#include "MapEditPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMapEditPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMapEditPage, CPropertyPage)
CMapEditPage::CMapEditPage()
	: CPropertyPage(CMapEditPage::IDD),
	m_bDlgInit(FALSE)
{
}

CMapEditPage::~CMapEditPage()
{
}

void CMapEditPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_EDITTYPE, m_comboEditType);
}


BEGIN_MESSAGE_MAP(CMapEditPage, CPropertyPage)
	ON_CBN_SELCHANGE(IDC_COMBO_EDITTYPE, OnCbnSelchangeComboEdittype)
	ON_EN_CHANGE(IDC_EDIT_BLUSH_SCALE, OnEnChangeEditBlushScale)
	ON_BN_CLICKED(IDC_BUTTON_QURD, OnBnClickedButtonQurd)
	ON_BN_CLICKED(IDC_BUTTON_CIRCLE, OnBnClickedButtonCircle)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnBnClickedButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_GAME, OnBnClickedButtonGame)
END_MESSAGE_MAP()


// CMapEditPage 메시지 처리기입니다.

BOOL CMapEditPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CRect SheetRect;
	CWnd *pWnd;

	//	Note : Sheet 1 를 만든다.
	//
	pWnd = GetDlgItem ( IDC_STATIC_SHEETTAP );
	pWnd->GetWindowRect ( SheetRect );
	ScreenToClient ( &SheetRect );

	m_psheetWithTab = new CsheetWithTabMapEdit ( SheetRect, this );
	m_psheetWithTab->Create( this, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0 );
	m_psheetWithTab->SetActivePage ( GRIDPAGEINDEX );

	DWORD nIndex;
	nIndex = m_comboEditType.AddString ( g_strEditSheetMapEdit[GRIDPAGEINDEX] );
	m_comboEditType.SetItemData ( nIndex, GRIDPAGEINDEX );

	nIndex = m_comboEditType.AddString ( g_strEditSheetMapEdit[HEIGHTPAGEINDEX] );
	m_comboEditType.SetItemData ( nIndex, HEIGHTPAGEINDEX );

	nIndex = m_comboEditType.AddString ( g_strEditSheetMapEdit[TILEPAGEINDEX] );
	m_comboEditType.SetItemData ( nIndex, TILEPAGEINDEX );

	nIndex = m_comboEditType.AddString ( g_strEditSheetMapEdit[PIECEPAGEINDEX] );
	m_comboEditType.SetItemData ( nIndex, PIECEPAGEINDEX );


	//	Note : Set Active Page
	//
	nIndex = m_comboEditType.FindString ( 0, g_strEditSheetMapEdit[GRIDPAGEINDEX] );
	m_comboEditType.SetCurSel ( nIndex );

	OnCbnSelchangeComboEdittype();

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	char szValue[256];
	pWnd = GetDlgItem ( IDC_EDIT_BLUSH_SCALE );
	_gcvt ( pView->GetMapEditMan()->m_nBlushScale_EDIT, 7, szValue );
	pWnd->SetWindowText ( szValue );

	//	Note : 컬러 버튼 셋팅
	//
	cClrQurd.SubclassDlgItem ( IDC_BUTTON_QURD, this );
	cClrCircle.SubclassDlgItem ( IDC_BUTTON_CIRCLE, this );

	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(192, 192, 192);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	if ( pView->GetMapEditMan()->m_bBlushCircle_EDIT )
	{
		cClrQurd.SetColour ( cColorTEXT, cColorGRAY );
		cClrCircle.SetColour ( cColorTEXT, cColorBLUE );
	}
	else
	{
		cClrQurd.SetColour ( cColorTEXT, cColorBLUE );
		cClrCircle.SetColour ( cColorTEXT, cColorGRAY );
	}

	cClrEdit.SubclassDlgItem ( IDC_BUTTON_EDIT, this );
	cClrGame.SubclassDlgItem ( IDC_BUTTON_GAME, this );

	cClrEdit.SetColour ( cColorTEXT, cColorGRAY );
	cClrGame.SetColour ( cColorTEXT, cColorGRAY );

	if ( (pView->m_ViewType!=TILE_EDIT_VIEW) && (pView->m_ViewType!=TILE_GAME_VIEW) )	pView->m_ViewType = TILE_EDIT_VIEW;

	if ( pView->m_ViewType == TILE_EDIT_VIEW )	cClrEdit.SetColour ( cColorTEXT, cColorBLUE );
	else										cClrGame.SetColour ( cColorTEXT, cColorBLUE );


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CMapEditPage::UpdateContrl ()
{
	if ( !m_bDlgInit )	return;

	OnInitDialog();		// 다시 초기화를 시킨다.
}

void CMapEditPage::OnCbnSelchangeComboEdittype()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DWORD nIndex = m_comboEditType.GetCurSel ();
	if ( nIndex == -1 )		return;

	DWORD dwData = m_comboEditType.GetItemData ( nIndex );
	m_psheetWithTab->SetActivePage ( dwData );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	switch ( dwData )
	{
	case GRIDPAGEINDEX:
		pView->m_SetType = BASE_VIEW;
		break;

	case HEIGHTPAGEINDEX:
		pView->m_SetType = HEIGHT_VIEW;
		break;

	case TILEPAGEINDEX:
		pView->m_SetType = TILE_COLOR_VIEW;
		break;

	case PIECEPAGEINDEX:
		pView->m_SetType = MAP_PIECE_VIEW;
		break;
	}
}

void CMapEditPage::OnEnChangeEditBlushScale()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CPropertyPage::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	CWnd*		pWnd;
	CString		strText;

	pWnd = GetDlgItem ( IDC_EDIT_BLUSH_SCALE );
	pWnd->GetWindowText ( strText );
	pView->GetMapEditMan()->m_nBlushScale_EDIT = atoi ( strText.GetString() );
}

void CMapEditPage::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	SAFE_DELETE(m_psheetWithTab);

	CPropertyPage::PostNcDestroy();
}

void CMapEditPage::OnBnClickedButtonQurd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(192, 192, 192);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	cClrQurd.SetColour ( cColorTEXT, cColorBLUE );
	cClrCircle.SetColour ( cColorTEXT, cColorGRAY );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetMapEditMan()->m_bBlushCircle_EDIT = FALSE;
}

void CMapEditPage::OnBnClickedButtonCircle()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(192, 192, 192);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	cClrQurd.SetColour ( cColorTEXT, cColorGRAY );
	cClrCircle.SetColour ( cColorTEXT, cColorBLUE );


	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetMapEditMan()->m_bBlushCircle_EDIT = TRUE;
}

void CMapEditPage::OnBnClickedButtonSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Map <Tile Edit Data> (*.ted)|*.ted";
	CFileDialog dlg ( FALSE, ".ted", NULL, OFN_HIDEREADONLY, szFilter, this );

	CString StrInitDir = pView->GetMapEditMan()->GetPathData();
	dlg.m_ofn.lpstrInitialDir = StrInitDir.GetString();

	if ( dlg.DoModal() == IDOK )
	{
		char cName[256];
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_MAP );
		pWnd->SetWindowText ( FilePath );

		FilePath = StrInitDir + FilePath;

		strcpy ( cName, FilePath.GetString() );

		pView->GetMapEditMan()->SaveSet ( cName );
	} // if ( dlg.DoModal() == IDOK )
}

void CMapEditPage::OnBnClickedButtonLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Map <Tile Edit Data> (*.ted)|*.ted";
	CFileDialog dlg ( TRUE, ".ted", NULL, OFN_HIDEREADONLY, szFilter, this );

	CString StrInitDir = pView->GetMapEditMan()->GetPathData();
	dlg.m_ofn.lpstrInitialDir = StrInitDir.GetString();

	if ( dlg.DoModal() == IDOK )
	{
		char cName[256];
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_MAP );
		pWnd->SetWindowText ( FilePath );

		FilePath = StrInitDir + FilePath;

		strcpy ( cName, FilePath.GetString() );

        CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
        const SUBPATH* pPath = pApp->GetSubPath();

		pView->GetMapEditMan()->LoadSet ( cName, pView->GetD3DDevice(), pPath );
	} // if ( dlg.DoModal() == IDOK )
}

void CMapEditPage::OnBnClickedButtonEdit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(192, 192, 192);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	cClrGame.SetColour ( cColorTEXT, cColorGRAY );
	cClrEdit.SetColour ( cColorTEXT, cColorBLUE );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetMapEditMan()->EditMode ();			// 에딧모드시 모든 정보 로딩.
	pView->m_ViewType = TILE_EDIT_VIEW;
}

void CMapEditPage::OnBnClickedButtonGame()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COLORREF cColorBLUE, cColorGRAY, cColorTEXT;
	cColorGRAY = RGB(192, 192, 192);
	cColorBLUE = RGB(192, 192, 255);
	cColorTEXT = RGB(0, 0, 0);

	cClrGame.SetColour ( cColorTEXT, cColorBLUE );
	cClrEdit.SetColour ( cColorTEXT, cColorGRAY );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->m_ViewType = TILE_GAME_VIEW;
}
