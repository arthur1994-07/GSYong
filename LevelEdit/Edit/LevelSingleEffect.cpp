// LevelSingleEffect.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../LevelEdit.h"
#include "./LevelSingleEffect.h"
#include "./LevelSheetWithTab.h"
#include "./LevelMainFrm.h"
#include "../LevelEditDoc.h"
#include "./LevelEditView.h"
#include "../../EngineLib/DxTools/DxViewPort.h"

// CLevelSingleEffect 대화 상자입니다.

DXLANDEFF*	CLevelSingleEffect::g_pLandEff_Ctrl_CV = NULL;
BOOL		CLevelSingleEffect::g_bCheckViewNameEFFECT = FALSE;

IMPLEMENT_DYNAMIC(CLevelSingleEffect, CPropertyPage)
CLevelSingleEffect::CLevelSingleEffect()
	: CPropertyPage(CLevelSingleEffect::IDD),
	m_bDlgInit(FALSE),
	m_pLandEff(NULL)
	, m_bLayer(FALSE)
{
}

CLevelSingleEffect::~CLevelSingleEffect()
{
}

void CLevelSingleEffect::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_EFFECT, m_EffListBox);
}


BEGIN_MESSAGE_MAP(CLevelSingleEffect, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnBnClickedButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, OnBnClickedButtonSearch)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnBnClickedButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_EFFAPPLY, OnBnClickedButtonEffapply)
	ON_BN_CLICKED(IDC_CHECK_VIEW_NAME, &CLevelSingleEffect::OnBnClickedCheckViewName)
END_MESSAGE_MAP()


// CLevelSingleEffect 메시지 처리기입니다.
BOOL CLevelSingleEffect::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_bDlgInit = TRUE;

	//UpdateContrl ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CLevelSingleEffect::FinalCleanup_STATIC()
{
	SAFE_DELETE( g_pLandEff_Ctrl_CV );
}

void CLevelSingleEffect::ViewContrls ( BOOL bView, BOOL bClear )
{
	if ( !m_bDlgInit )	return;

	DWORD dwContrls[] = { IDC_EDIT_NAME, IDC_EDIT_FINDSTR, IDC_BUTTON_SEARCH, IDC_BUTTON_ADD, IDC_BUTTON_EFFAPPLY };
	DWORD dwNumContrls = sizeof(dwContrls) / sizeof(DWORD);

	CWnd *pWnd;
	
	for ( DWORD i=0; i < dwNumContrls; i++ )
	{
		pWnd = GetDlgItem ( dwContrls[i] );
		pWnd->ShowWindow ( bView );
	}

	if ( bClear )
	{
		pWnd = GetDlgItem ( IDC_EDIT_NAME );
		pWnd->SetWindowText ( "" );

		pWnd = GetDlgItem ( IDC_EDIT_FINDSTR );
		pWnd->SetWindowText ( "" );
	}
	else if ( m_pLandEff )
	{
		pWnd = GetDlgItem ( IDC_EDIT_NAME );
		pWnd->SetWindowText ( m_pLandEff->m_szName );

		pWnd = GetDlgItem ( IDC_EDIT_FINDSTR );
		pWnd->SetWindowText ( m_pLandEff->m_szFileName );
	}
}

void CLevelSingleEffect::ListtingLandEff ()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();    
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();

	CString StrOldSelect;
	int nIndex = m_EffListBox.GetCurSel ();
	if ( nIndex==LB_ERR )
	{
		m_EffListBox.GetDlgItemText ( nIndex, StrOldSelect );
	}

	m_EffListBox.ResetContent ();

	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if ( !pGLLandMan )		return;

	PLANDEFF pCur = pGLLandMan->GetLandEffList( GetLayerIndex() );
	while ( pCur )
	{
		int nIndex = m_EffListBox.AddString ( pCur->m_szName );
		m_EffListBox.SetItemData( nIndex, (DWORD)pCur );

		pCur = pCur->m_pNext;
	}

	m_EffListBox.SelectString ( 0, StrOldSelect );
}

void CLevelSingleEffect::UpdateContrl ()
{
	if ( !m_bDlgInit )	return;

	m_pLandEff = NULL;

	ViewContrls ( FALSE );
	ListtingLandEff ();

	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();

	pView->DeActiveEditMatrix();
}

void CLevelSingleEffect::OnBnClickedButtonNew()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();    
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if (!pGLLandMan)
        return;

	DXLANDEFF *pLandEff;
	pLandEff = new DXLANDEFF;

	char szNewName[DXLANDEFF::LANDEFF_MAXSZ];
	
	int i=0;
	do
	{
		sprintf ( szNewName, "%s[%d]", pLandEff->m_szName, i++ );
	} while ( pGLLandMan->FindLandEff( GetLayerIndex(), szNewName ) );

	strcpy ( pLandEff->m_szName, szNewName );

	pLandEff->SetPosition( pView->GetCollisionPos() );

	m_pLandEff = pLandEff;
	pView->ActiveEditMatrix2( &m_pLandEff->m_matWorld );
	pGLLandMan->AddLandEff( GetLayerIndex(), m_pLandEff );
	pGLLandMan->BuildSingleEffTree ( GetLayerIndex() );

	m_EffListBox.SelectString ( 0, m_pLandEff->m_szName );

	ViewContrls ( TRUE );
	ListtingLandEff ();
}

void CLevelSingleEffect::OnBnClickedButtonSearch()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();

	//	Note : 파일 오픈 다이알로그를 만듬.
	CString szFilter = "Effect Group ( *.egp ) |*.egp";
	CFileDialog dlg ( TRUE, ".egp", "untitled",
		OFN_HIDEREADONLY|OFN_READONLY, szFilter, pView );

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		CString WriteName = dlg.GetFileName ();

		CWnd *pWnd = GetDlgItem ( IDC_EDIT_FINDSTR );
		pWnd->SetWindowText ( WriteName );
	}
}

void CLevelSingleEffect::OnBnClickedButtonAdd()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();    
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if (!pGLLandMan)
        return;

	CWnd *pWnd;
	char szName[DXLANDEFF::LANDEFF_MAXSZ] = {0};
	char szFileName[MAX_PATH] = {0};

	pWnd = GetDlgItem ( IDC_EDIT_NAME );
	pWnd->GetWindowText ( szName, DXLANDEFF::LANDEFF_MAXSZ );

	pWnd = GetDlgItem ( IDC_EDIT_FINDSTR );
	pWnd->GetWindowText ( szFileName, MAX_PATH );

	BOOL bExist = FALSE;
	PLANDEFF pCur = pGLLandMan->GetLandEffList ( GetLayerIndex() );
	while ( pCur )
	{
		if ( !strcmp(pCur->m_szName,szName) && pCur!=m_pLandEff )
		{
			MessageBox ( "ERROR : Same name is already registered.", "Caution" );
			bExist = TRUE;
			return;
		}

		pCur = pCur->m_pNext;
	}

	if ( szFileName[0] == NULL )
	{
		MessageBox ( "ERROR : Effect file is not selected.", "Caution" );
		return;
	}
   
	HRESULT hr;
	hr = m_pLandEff->SetEffect(szName, m_pLandEff->m_matWorld, szFileName, 1, 1, pView->GetD3DDevice());
	if ( FAILED(hr) )
	{
		MessageBox ( "FAILURE : Object can not be initialized with fixed value.", "Caution" );
	}

	pGLLandMan->BuildSingleEffTree ( GetLayerIndex() );


	m_EffListBox.SelectString ( 0, m_pLandEff->m_szName );
	ListtingLandEff ();

	m_pLandEff = NULL;
	ViewContrls ( FALSE );
	UpdateContrl ();
}

void CLevelSingleEffect::OnBnClickedButtonEdit()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();

	int nIndex = m_EffListBox.GetCurSel ();
	if ( nIndex==LB_ERR )	return;
	
	m_pLandEff = (PLANDEFF) m_EffListBox.GetItemData ( nIndex );
	pView->ActiveEditMatrix2( &m_pLandEff->m_matWorld );

	ViewContrls ( TRUE );

	D3DXVECTOR3 vPos( m_pLandEff->m_matWorld._41, m_pLandEff->m_matWorld._42, m_pLandEff->m_matWorld._43 );
	DxViewPort::GetInstance().CameraJump( vPos );
}

void CLevelSingleEffect::OnBnClickedButtonDel()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if (!pGLLandMan)
        return;

	int nIndex = m_EffListBox.GetCurSel ();
	if ( nIndex==LB_ERR )	return;

	PLANDEFF pLandEff = (PLANDEFF) m_EffListBox.GetItemData ( nIndex );

	pGLLandMan->DelLandEff ( GetLayerIndex(), pLandEff );
	pGLLandMan->BuildSingleEffTree ( GetLayerIndex() );

	//	현제 편집중인 이팩트일경우.
	if ( pLandEff== m_pLandEff )
	{
		m_pLandEff = NULL;
		ViewContrls ( FALSE );
		UpdateContrl ();
	}
	else
	{
		ListtingLandEff ();
	}
}

void CLevelSingleEffect::OnBnClickedButtonEffapply()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();    
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if (!pGLLandMan)
        return;

	CWnd *pWnd;
	char szName[DXLANDEFF::LANDEFF_MAXSZ] = {0};
	char szFileName[MAX_PATH] = {0};

	pWnd = GetDlgItem ( IDC_EDIT_NAME );
	pWnd->GetWindowText ( szName, DXLANDEFF::LANDEFF_MAXSZ );

	pWnd = GetDlgItem ( IDC_EDIT_FINDSTR );
	pWnd->GetWindowText ( szFileName, MAX_PATH );

	BOOL bExist = FALSE;
	PLANDEFF pCur = pGLLandMan->GetLandEffList ( GetLayerIndex() );
	while ( pCur )
	{
		if ( !strcmp(pCur->m_szName,szName) && pCur!=m_pLandEff )
		{
			MessageBox ( "ERROR : Same name is already registered.", "Caution" );
			bExist = TRUE;
			return;
		}

		pCur = pCur->m_pNext;
	}

	if ( szFileName[0] == NULL )
	{
		MessageBox ( "ERROR : Effect file is not selected.", "Caution" );
		return;
	}

	HRESULT hr;
	hr = m_pLandEff->SetEffect ( szName, m_pLandEff->m_matWorld, szFileName, 1, 1, pView->GetD3DDevice() );
	if ( FAILED(hr) )
	{
		MessageBox ( "FAILURE : Object can not be initialized with fixed value.", "Caution" );
	}

	pGLLandMan->BuildSingleEffTree ( GetLayerIndex() );


	m_EffListBox.SelectString ( 0, m_pLandEff->m_szName );
	ListtingLandEff ();
}

BOOL CLevelSingleEffect::OnKillActive()
{
	m_pLandEff = NULL;
	ViewContrls ( FALSE );
	UpdateContrl ();

	return CPropertyPage::OnKillActive();
}

void CLevelSingleEffect::ActiveSingleEffectIndex()
{
	m_bLayer = FALSE;

	CWnd *pWnd = GetDlgItem( IDC_STATIC_LAYER_NUMBER );
	pWnd->ShowWindow( FALSE );

	pWnd = GetDlgItem( IDC_STATIC_LAYER_NUMBER );
	pWnd->SetWindowText( _T("") );

	UpdateContrl();
}

void CLevelSingleEffect::ActiveSingleEffectLayerIndex()
{
	m_bLayer = TRUE;

	CWnd *pWnd = GetDlgItem( IDC_STATIC_LAYER_NUMBER );
	pWnd->ShowWindow( TRUE );

	TSTRINGSTREAM strStringStream;
	strStringStream << "Layer : ";
	strStringStream << CMobSetPage::g_unLayer;
	pWnd = GetDlgItem( IDC_STATIC_LAYER_NUMBER );
	pWnd->SetWindowText( strStringStream.str().c_str() );

	UpdateContrl();
}

UINT CLevelSingleEffect::GetLayerIndex()
{
	if ( m_bLayer )
		return CMobSetPage::g_unLayer+1;
	else
		return 0;
}

void CLevelSingleEffect::CutMobSch()
{
	SAFE_DELETE ( g_pLandEff_Ctrl_CV );

	int nIndex = m_EffListBox.GetCurSel ();
	if ( nIndex==LB_ERR )	return;

	DXLANDEFF* pLandEff = (PLANDEFF) m_EffListBox.GetItemData ( nIndex );

	if ( !pLandEff )
		return;

	g_pLandEff_Ctrl_CV = new DXLANDEFF;
	StringCchCopy( g_pLandEff_Ctrl_CV->m_szFileName, MAX_PATH, pLandEff->m_szFileName );
	g_pLandEff_Ctrl_CV->m_matWorld			= pLandEff->m_matWorld;
}

void CLevelSingleEffect::CopyMobSch()
{
	SAFE_DELETE ( g_pLandEff_Ctrl_CV );

	int nIndex = m_EffListBox.GetCurSel ();
	if ( nIndex==LB_ERR )	return;

	DXLANDEFF* pLandEff = (PLANDEFF) m_EffListBox.GetItemData ( nIndex );

	if ( !pLandEff )
		return;

	g_pLandEff_Ctrl_CV = new DXLANDEFF;
	StringCchCopy( g_pLandEff_Ctrl_CV->m_szFileName, MAX_PATH, pLandEff->m_szFileName );
	g_pLandEff_Ctrl_CV->m_matWorld			= pLandEff->m_matWorld;
}

void CLevelSingleEffect::PasteMobSch()
{
	if ( !g_pLandEff_Ctrl_CV )
		return;

	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();    
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	if (!pGLLandMan)
		return;

	DXLANDEFF *pLandEff;
	pLandEff = new DXLANDEFF;

	StringCchCopy( pLandEff->m_szName, MAX_PATH, g_pLandEff_Ctrl_CV->m_szName );

	char szNewName[DXLANDEFF::LANDEFF_MAXSZ];

	int i=0;
	do
	{
		sprintf ( szNewName, "%s[%d]", pLandEff->m_szName, i++ );
	} while ( pGLLandMan->FindLandEff( GetLayerIndex(), szNewName ) );

	m_pLandEff = pLandEff;

	pGLLandMan->AddLandEff( GetLayerIndex(), m_pLandEff );

	HRESULT hr = m_pLandEff->SetEffect( szNewName, g_pLandEff_Ctrl_CV->m_matWorld, g_pLandEff_Ctrl_CV->m_szFileName, 1, 1, pView->GetD3DDevice());
	if ( FAILED(hr) )
	{
		MessageBox ( "FAILURE : Object can not be initialized with fixed value.", "Caution" );
	}

	pGLLandMan->BuildSingleEffTree ( GetLayerIndex() );


	m_EffListBox.SelectString ( 0, m_pLandEff->m_szName );
	ListtingLandEff ();

	m_pLandEff = NULL;
	ViewContrls ( FALSE );
	UpdateContrl ();
}

void CLevelSingleEffect::OnBnClickedCheckViewName()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_VIEW_NAME );
	if ( pButton->GetCheck() )
	{
		g_bCheckViewNameEFFECT = TRUE;
	}
	else
	{
		g_bCheckViewNameEFFECT = FALSE;
	}
}

BOOL CLevelSingleEffect::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_VIEW_NAME );
	pButton->SetCheck( g_bCheckViewNameEFFECT );

	return CPropertyPage::OnSetActive();
}
