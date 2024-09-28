// LevelMainPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../LevelEdit.h"
#include "./LevelSheetWithTab.h"
#include "../LevelMainFrm.h"
#include "../LevelEditDoc.h"
#include "../LevelEditView.h"

#include "../../RanLogicServer/Land/GLLandMan.h"

#include "./LevelMainPage.h"


// CLevelMainPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLevelMainPage, CPropertyPage)
CLevelMainPage::CLevelMainPage()
	: CPropertyPage(CLevelMainPage::IDD),
	m_pSheetTab(NULL),
	m_bInit(FALSE)
{
}

CLevelMainPage::~CLevelMainPage()
{
}

void CLevelMainPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DIV, m_comboDiv);
}


BEGIN_MESSAGE_MAP(CLevelMainPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_WLDFILENAME, OnBnClickedButtonWldfilename)
	ON_BN_CLICKED(IDC_BUTTON_CREATETREE, OnBnClickedButtonCreatetree)
	ON_CBN_SELCHANGE(IDC_COMBO_DIV, OnCbnSelchangeComboDiv)
	ON_BN_CLICKED(IDC_CHECK_RENDQUAD, OnBnClickedCheckRendquad)
	ON_BN_CLICKED(IDC_CHECK_CROWSCH, OnBnClickedCheckCrowsch)
	ON_BN_CLICKED(IDC_BUTTON_NAVI_FILENAME, &CLevelMainPage::OnBnClickedButtonNaviFilename)
	ON_BN_CLICKED(IDC_BUTTON_NAVI_FILENAME_CLEAR, &CLevelMainPage::OnBnClickedButtonNaviFilenameClear)
END_MESSAGE_MAP()


// CLevelMainPage 메시지 처리기입니다.
static CString strDiv[10] = { "01", "02", "03", "04", "05", "06", "07", "08", "09", "10" };
static DWORD dwDiv[10] = { QUAD_DIV_01, QUAD_DIV_02, QUAD_DIV_03, QUAD_DIV_04, QUAD_DIV_05,
		QUAD_DIV_06, QUAD_DIV_07, QUAD_DIV_08, QUAD_DIV_09, QUAD_DIV_10 };

BOOL CLevelMainPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CString StrTemp;

	StrTemp = "untitle.";
	StrTemp += GLLandMan::FILE_EXT;

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_FILENAME );
	pWnd->SetWindowText ( StrTemp );

	int nIndex;
	for ( int i=0; i<10; i++ )
	{
		nIndex = m_comboDiv.AddString ( strDiv[i] );
		m_comboDiv.SetItemData ( nIndex, dwDiv[i] );
	}

	nIndex = (int) QUAD_DEFAULT_DIV;

	int nLevel = (int) ( log10(static_cast<float>(nIndex)) / log10(static_cast<float>(2)) ) - 1;
	m_comboDiv.SetCurSel ( nLevel );

	pWnd = GetDlgItem ( IDC_EDIT_DIV );
	StrTemp.Format ( "%d", dwDiv[nLevel]*dwDiv[nLevel] );
	pWnd->SetWindowText ( StrTemp );

	CButton *pButton;
	pButton = (CButton *) GetDlgItem ( IDC_CHECK_RENDQUAD );
	pButton->SetCheck ( CLevelEditView::m_bRENDQUAD );

	GLMobSchedule::m_bRENDACT = TRUE;
	pButton = (CButton *) GetDlgItem ( IDC_CHECK_CROWSCH );
	pButton->SetCheck ( GLMobSchedule::m_bRENDACT );

	m_bInit = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CLevelMainPage::OnBnClickedButtonWldfilename()
{
	HRESULT hr = S_OK;

	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();    
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();

	CWnd *pWnd;

	//	Note : 파일 필터를 정의한다.
	CString szFilter = CString("Piece File |*.") + "wld" + "|";

	CString szFileExt = CString(".") + "wld";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,szFileExt,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	dlg.m_ofn.lpstrInitialDir = DxLandMan::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();

		pGaeaServer->ClearDropObj();
		
		boost::shared_ptr<DxLandMan> spLandMan = pView->GetspLandMan ();
		spLandMan->CleanUp ();
		BOOL bOk = spLandMan->LoadFile ( sFileName.GetString(), pView->GetD3dDevice () );
		
		if ( bOk )
		{
			spLandMan->ActiveMap_Tool( pView->GetD3dDevice(), spLandMan );

			GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
			bOk = pGLLandMan->LoadWldFile ( sFileName.GetString(), true );

			pWnd = GetDlgItem ( IDC_EDIT_WLDFILENAME );
			pWnd->SetWindowText ( sFileName.GetString() );
		}
	}
}

void CLevelMainPage::OnBnClickedButtonCreatetree()
{
    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	pGaeaServer->ClearDropObj ();
	pGLLandMan->SetMap( TRUE );
}

void CLevelMainPage::OnCbnSelchangeComboDiv()
{
	int nIndex = m_comboDiv.GetCurSel ();
	if ( nIndex == LB_ERR )
        return;
	DWORD dwData = (DWORD) m_comboDiv.GetItemData ( nIndex );

	CWnd *pWnd = NULL;
	CString StrTemp;

    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	pGLLandMan->SetLandTreeDiv ( (EMDIVISION)dwData );

	pWnd = GetDlgItem ( IDC_EDIT_DIV );
	StrTemp.Format ( "%d", dwData*dwData );
	pWnd->SetWindowText ( StrTemp );
}

void CLevelMainPage::OnBnClickedCheckRendquad()
{
	CButton *pButton = (CButton *) GetDlgItem ( IDC_CHECK_RENDQUAD );
	CLevelEditView::m_bRENDQUAD = pButton->GetCheck();
}

void CLevelMainPage::OnBnClickedCheckCrowsch()
{
	CButton *pButton = (CButton *) GetDlgItem ( IDC_CHECK_CROWSCH );
	GLMobSchedule::m_bRENDACT = pButton->GetCheck ();
}

void CLevelMainPage::UpdatePage ()
{
	if (!m_bInit)
        return;

	CString StrTemp;
	CWnd *pWnd = NULL;
    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();

	pWnd = GetDlgItem ( IDC_EDIT_FILENAME );
	pWnd->SetWindowText ( pGLLandMan->GetFileName() );
	
	pWnd = GetDlgItem ( IDC_EDIT_WLDFILENAME );
	pWnd->SetWindowText ( pGLLandMan->GetWldFileName() );

	pWnd = GetDlgItem ( IDC_EDIT_NAVI_FILENAME );
	pWnd->SetWindowText ( pGLLandMan->GetNaviBypassFileName() );

	int nIndex = (int) pGLLandMan->GetLandTreeDiv ();
	int nLevel = (int) ( log10(static_cast<float>(nIndex)) / log10(static_cast<float>(2)) ) - 1;
	m_comboDiv.SetCurSel ( nLevel );

	pWnd = GetDlgItem ( IDC_EDIT_DIV );
	StrTemp.Format ( "%d", dwDiv[nLevel]*dwDiv[nLevel] );
	pWnd->SetWindowText ( StrTemp );
}

void CLevelMainPage::OnBnClickedButtonNaviFilename()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();    
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();

	CWnd *pWnd;

	//	Note : 파일 필터를 정의한다.
	CString szFilter = CString("navi |*.") + "navi" + "|";

	CString szFileExt = CString(".") + "navi";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,szFileExt,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	dlg.m_ofn.lpstrInitialDir = DxLandMan::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();

		GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
		pGLLandMan->LoadNaviBypassFile( sFileName.GetString() );

		if ( pView->GetLandMan() )
		{
			pView->GetLandMan()->LoadNaviBypassFile( sFileName.GetString() );
		}

		pWnd = GetDlgItem ( IDC_EDIT_NAVI_FILENAME );
		pWnd->SetWindowText ( sFileName.GetString() );

		OnBnClickedButtonCreatetree();
	}
}

void CLevelMainPage::OnBnClickedButtonNaviFilenameClear()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();    
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();

	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	pGLLandMan->ClearNaviBypassFile();

	if ( pView->GetLandMan() )
	{
		pView->GetLandMan()->ClearNaviBypassFile();
	}

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_EDIT_NAVI_FILENAME );
	pWnd->SetWindowText ( _T("") );

	OnBnClickedButtonCreatetree();
}
