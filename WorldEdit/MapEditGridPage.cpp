// WeatherPage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "WorldEdit.h"

#include "sheetWithTab.h"

#include "../EngineLib/DxLand/DxSetLandMan.h"

#include "../EngineLib/DxEffect/MapEdit/DxMapEditMan.h"

#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"
#include "MapEditGridPage.h"
#include ".\mapeditgridpage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMapEditGridPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CMapEditGridPage, CPropertyPage)
CMapEditGridPage::CMapEditGridPage()
	: CPropertyPage(CMapEditGridPage::IDD),
	m_bDlgInit(FALSE)
{
}

CMapEditGridPage::~CMapEditGridPage()
{
}

void CMapEditGridPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_GRID_NUM, m_comboGRIDNUM);
}


BEGIN_MESSAGE_MAP(CMapEditGridPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_CHECK_LENGTH, OnBnClickedCheckLength)
	ON_BN_CLICKED(IDC_BUTTON_CREATE, OnBnClickedButtonCreate)
END_MESSAGE_MAP()


// CMapEditGridPage �޽��� ó�����Դϴ�.

BOOL CMapEditGridPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	//m_sOldProperty	= m_sProperty	= DxFogMan::GetInstance().GetProperty();
	m_bDlgInit = TRUE;

	DWORD nIndex;
	nIndex = m_comboGRIDNUM.AddString ( "1" );
	m_comboGRIDNUM.SetItemData ( nIndex, 1 );

	nIndex = m_comboGRIDNUM.AddString ( "2" );
	m_comboGRIDNUM.SetItemData ( nIndex, 2 );

	nIndex = m_comboGRIDNUM.AddString ( "4" );
	m_comboGRIDNUM.SetItemData ( nIndex, 4 );

	nIndex = m_comboGRIDNUM.AddString ( "8" );
	m_comboGRIDNUM.SetItemData ( nIndex, 8 );

	nIndex = m_comboGRIDNUM.AddString ( "16" );
	m_comboGRIDNUM.SetItemData ( nIndex, 16 );

	nIndex = m_comboGRIDNUM.AddString ( "32" );
	m_comboGRIDNUM.SetItemData ( nIndex, 32 );

	nIndex = m_comboGRIDNUM.AddString ( "64" );
	m_comboGRIDNUM.SetItemData ( nIndex, 64 );

	nIndex = m_comboGRIDNUM.AddString ( "128" );
	m_comboGRIDNUM.SetItemData ( nIndex, 128 );

	nIndex = m_comboGRIDNUM.AddString ( "256" );
	m_comboGRIDNUM.SetItemData ( nIndex, 256 );

	nIndex = m_comboGRIDNUM.AddString ( "512" );
	m_comboGRIDNUM.SetItemData ( nIndex, 512 );

	nIndex = m_comboGRIDNUM.AddString ( "1024" );
	m_comboGRIDNUM.SetItemData ( nIndex, 1024 );

	InitDialog ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CMapEditGridPage::InitDialog ()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_sProperty = pView->GetMapEditMan()->GetProperty();

	CWnd*		pWnd;
	CButton*	pButton;

	pWnd	= GetDlgItem ( IDC_EDIT_LENGTH );
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_LENGTH );
	if ( m_sProperty.m_dwFlag & USE_LENGTH )
	{
		pWnd->EnableWindow ( TRUE );
		pButton->SetCheck ( TRUE );
	}
	else
	{
		pWnd->EnableWindow ( FALSE );
		pButton->SetCheck ( FALSE );
	}
	
	char szValue[256];
	pWnd = GetDlgItem ( IDC_EDIT_LENGTH );
	_gcvt ( pView->GetMapEditMan()->m_dwLength_EDIT, 7, szValue );
	pWnd->SetWindowText ( szValue );

	if ( m_sProperty.m_dwGrid == 1 )	m_comboGRIDNUM.SetCurSel ( 0 );
	if ( m_sProperty.m_dwGrid == 2 )	m_comboGRIDNUM.SetCurSel ( 1 );
	if ( m_sProperty.m_dwGrid == 4 )	m_comboGRIDNUM.SetCurSel ( 2 );
	if ( m_sProperty.m_dwGrid == 8 )	m_comboGRIDNUM.SetCurSel ( 3 );
	if ( m_sProperty.m_dwGrid == 16 )	m_comboGRIDNUM.SetCurSel ( 4 );
	if ( m_sProperty.m_dwGrid == 32 )	m_comboGRIDNUM.SetCurSel ( 5 );
	if ( m_sProperty.m_dwGrid == 64 )	m_comboGRIDNUM.SetCurSel ( 6 );
	if ( m_sProperty.m_dwGrid == 128 )	m_comboGRIDNUM.SetCurSel ( 7 );
	if ( m_sProperty.m_dwGrid == 256 )	m_comboGRIDNUM.SetCurSel ( 8 );
	if ( m_sProperty.m_dwGrid == 512 )	m_comboGRIDNUM.SetCurSel ( 9 );
	if ( m_sProperty.m_dwGrid == 1024 )	m_comboGRIDNUM.SetCurSel ( 10 );
}

void CMapEditGridPage::UpdateContrl ()
{
	if ( !m_bDlgInit )	return;

	InitDialog();		// �ٽ� �ʱ�ȭ�� ��Ų��.
}

void CMapEditGridPage::OnBnClickedButtonApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_sProperty = pView->GetMapEditMan()->GetProperty();

	CWnd*		pWnd;

	CString strText;

	pWnd = GetDlgItem ( IDC_EDIT_LENGTH );
	pWnd->GetWindowText ( strText );
	pView->GetMapEditMan()->m_dwLength_EDIT = atoi ( strText.GetString() );

	DWORD nIndex = m_comboGRIDNUM.GetCurSel ();
	if ( nIndex != -1 )
	{
		DWORD dwData = m_comboGRIDNUM.GetItemData ( nIndex );
		m_sProperty.m_dwGrid = dwData;
	}

	pView->GetMapEditMan()->SetProperty ( m_sProperty );
}

void CMapEditGridPage::OnBnClickedCheckLength()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CWnd*		pWnd;
	CButton*	pButton;
	pWnd	= GetDlgItem ( IDC_EDIT_LENGTH );
	pButton = (CButton*) GetDlgItem ( IDC_CHECK_LENGTH );

	if ( pButton->GetCheck() )
	{
		pWnd->EnableWindow ( TRUE );
		pButton->SetCheck ( TRUE );
	}
	else
	{
		pWnd->EnableWindow ( FALSE );
		pButton->SetCheck ( FALSE );
	}
}

void CMapEditGridPage::OnBnClickedButtonCreate()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	OnBnClickedButtonApply();		// ��� ���� ������ ����

	pView->GetMapEditMan()->ModifyGrid ( pView->GetD3DDevice() );
}