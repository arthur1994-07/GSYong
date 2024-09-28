// WeatherPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"

#include "ColorPickerDlg.h"
#include "sheetWithTab.h"

#include "../EngineLib/Common/SUBPATH.H"

#include "../EngineLib/DxEffect/MapEdit/DxMapEditMan.h"

#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"
#include "MapEditPiecePage.h"
#include ".\mapeditpiecepage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const COLORREF CMapEditPiecePage::CLOUDBLUE	= RGB(128, 184, 223);
const COLORREF CMapEditPiecePage::WHITE		= RGB(255, 255, 255);
const COLORREF CMapEditPiecePage::BLACK		= RGB(1, 1, 1);
const COLORREF CMapEditPiecePage::DKGRAY		= RGB(128, 128, 128);
const COLORREF CMapEditPiecePage::LLTGRAY		= RGB(220, 220, 220);
const COLORREF CMapEditPiecePage::LTGRAY		= RGB(192, 192, 192);
const COLORREF CMapEditPiecePage::YELLOW		= RGB(255, 255, 0);
const COLORREF CMapEditPiecePage::DKYELLOW		= RGB(128, 128, 0);
const COLORREF CMapEditPiecePage::COLORREF_RED = RGB(255, 0, 0);
const COLORREF CMapEditPiecePage::DKRED		= RGB(128, 0, 0);
const COLORREF CMapEditPiecePage::COLORREF_BLUE = RGB(0, 0, 255);
const COLORREF CMapEditPiecePage::LBLUE		= RGB(192, 192, 255);
const COLORREF CMapEditPiecePage::DKBLUE		= RGB(0, 0, 128);
const COLORREF CMapEditPiecePage::CYAN			= RGB(0, 255, 255);
const COLORREF CMapEditPiecePage::DKCYAN		= RGB(0, 128, 128);
const COLORREF CMapEditPiecePage::COLORREF_GREEN = RGB(0, 255, 0);
const COLORREF CMapEditPiecePage::DKGREEN		= RGB(0, 128, 0);
const COLORREF CMapEditPiecePage::MAGENTA		= RGB(255, 0, 255);
const COLORREF CMapEditPiecePage::DKMAGENTA	= RGB(128, 0, 128);

// CMapEditPiecePage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMapEditPiecePage, CPropertyPage)
CMapEditPiecePage::CMapEditPiecePage()
	: CPropertyPage(CMapEditPiecePage::IDD),
	m_bDlgInit(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CMapEditPiecePage::~CMapEditPiecePage()
{
}

void CMapEditPiecePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMapEditPiecePage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_PIECE, OnBnClickedButtonPiece)
	ON_BN_CLICKED(IDC_CHECK_RANDOM, OnBnClickedCheckRandom)
	ON_EN_CHANGE(IDC_EDIT_MOVE_X, OnEnChangeEditMoveX)
	ON_EN_CHANGE(IDC_EDIT_MOVE_Y, OnEnChangeEditMoveY)
	ON_EN_CHANGE(IDC_EDIT_MOVE_Z, OnEnChangeEditMoveZ)
	ON_EN_CHANGE(IDC_EDIT_ROTATE_X, OnEnChangeEditRotateX)
	ON_EN_CHANGE(IDC_EDIT_ROTATE_Y, OnEnChangeEditRotateY)
	ON_EN_CHANGE(IDC_EDIT_ROTATE_Z, OnEnChangeEditRotateZ)
	ON_BN_CLICKED(IDC_BUTTON_FILE_SAVE, OnBnClickedButtonFileSave)
	ON_BN_CLICKED(IDC_BUTTON_FILE_LOAD, OnBnClickedButtonFileLoad)
END_MESSAGE_MAP()


// CMapEditPiecePage 메시지 처리기입니다.

BOOL CMapEditPiecePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	//CWnd*		pWnd;

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	//m_sOldProperty	= m_sProperty	= DxFogMan::GetInstance().GetProperty();
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_sProperty = pView->GetMapEditMan()->GetProperty();
	
	m_bDlgInit = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CMapEditPiecePage::UpdateContrl ()
{
	if ( !m_bDlgInit )	return;

	OnInitDialog();		// 다시 초기화를 시킨다.
}

void CMapEditPiecePage::OnBnClickedButtonFileSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxPieceMan* pPieceMan = pView->GetMapEditMan()->GetPieceMan();

	CString szFilter = "All Piece (*.apis)|*.apis";
	CFileDialog dlg ( FALSE, ".apis", NULL, OFN_HIDEREADONLY, szFilter, this );

    CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	char szPath[MAX_PATH] = {0};
	strcpy(szPath, pFrame->m_szAppPath);
	strcat(szPath, pPath->ObjFileStaticMesh().c_str() ); //SUBPATH::OBJ_FILE_STATICMESH);

	CString StrInitDir = szPath;
	dlg.m_ofn.lpstrInitialDir = StrInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		char cName[256];
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_FILE );
		pWnd->SetWindowText ( FilePath );

		strcpy ( cName, StrInitDir );
		strcat ( cName, FilePath.GetString() );
		pPieceMan->Save ( cName );
	} // if ( dlg.DoModal() == IDOK )
}

void CMapEditPiecePage::OnBnClickedButtonFileLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxPieceMan* pPieceMan = pView->GetMapEditMan()->GetPieceMan();

	CString szFilter = "All Piece (*.apis)|*.apis";
	CFileDialog dlg ( TRUE, ".apis", NULL, OFN_HIDEREADONLY, szFilter, this );

    CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	char szPath[MAX_PATH] = {0};
	strcpy(szPath, pFrame->m_szAppPath);
	strcat(szPath, pPath->ObjFileStaticMesh().c_str()); //SUBPATH::OBJ_FILE_STATICMESH);

	CString StrInitDir = szPath;
	dlg.m_ofn.lpstrInitialDir = StrInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		char cName[256];
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_FILE );
		pWnd->SetWindowText ( FilePath );

		strcpy ( cName, FilePath.GetString() );
		pPieceMan->Load ( pView->GetD3DDevice(), cName );
	} // if ( dlg.DoModal() == IDOK )
}

void CMapEditPiecePage::OnBnClickedButtonPiece()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	DxPieceMan* pPieceMan = pView->GetMapEditMan()->GetPieceMan();

	CString szFilter = "Static Piece (*.spis)|*.spis";
	CFileDialog dlg ( TRUE, ".spis", NULL, OFN_HIDEREADONLY, szFilter, this );

    CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	char szPath[MAX_PATH] = {0};
	strcpy(szPath, pFrame->m_szAppPath);
	strcat(szPath, pPath->ObjFileStaticMesh().c_str()); //SUBPATH::OBJ_FILE_STATICMESH);

	CString StrInitDir = szPath;
	dlg.m_ofn.lpstrInitialDir = StrInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		char cName[256];
		CString FilePath = dlg.GetFileName ();

		CWnd *pWnd;
		pWnd = GetDlgItem ( IDC_EDIT_PIECE );
		pWnd->SetWindowText ( FilePath );

		strcpy ( cName, FilePath.GetString() );
		pPieceMan->SetPisName ( pView->GetD3DDevice(), cName );
	} // if ( dlg.DoModal() == IDOK )
}

void CMapEditPiecePage::OnBnClickedCheckRandom()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CMapEditPiecePage::OnEnChangeEditMoveX()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CPropertyPage::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CMapEditPiecePage::OnEnChangeEditMoveY()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CPropertyPage::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CMapEditPiecePage::OnEnChangeEditMoveZ()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CPropertyPage::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CMapEditPiecePage::OnEnChangeEditRotateX()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CPropertyPage::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CMapEditPiecePage::OnEnChangeEditRotateY()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CPropertyPage::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CMapEditPiecePage::OnEnChangeEditRotateZ()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CPropertyPage::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
