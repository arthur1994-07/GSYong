// CubeMapDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "CubeMapDlg.h"


// CCubeMapDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CCubeMapDlg, CDialog)
CCubeMapDlg::CCubeMapDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCubeMapDlg::IDD, pParent)
	, m_strMap0(_T(""))
	, m_strMap1(_T(""))
	, m_strMap2(_T(""))
	, m_strMap3(_T(""))
	, m_strMap4(_T(""))
	, m_strMap5(_T(""))
	, m_fAlpha(0.0f)
{
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( FALSE );
}

CCubeMapDlg::~CCubeMapDlg()
{
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( TRUE );
}

void CCubeMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MAP0, m_strMap0);
	DDX_Text(pDX, IDC_EDIT_MAP1, m_strMap1);
	DDX_Text(pDX, IDC_EDIT_MAP2, m_strMap2);
	DDX_Text(pDX, IDC_EDIT_MAP3, m_strMap3);
	DDX_Text(pDX, IDC_EDIT_MAP4, m_strMap4);
	DDX_Text(pDX, IDC_EDIT_MAP5, m_strMap5);
	DDX_Text(pDX, IDC_EDIT_REFLECTION_ALPHA, m_fAlpha);
	DDX_Control(pDX, IDC_SPIN_ALPHA, m_ctrlReflectionAlpha);
}


BEGIN_MESSAGE_MAP(CCubeMapDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_MAP0, OnBnClickedButtonMap0)
	ON_BN_CLICKED(IDC_BUTTON_MAP1, OnBnClickedButtonMap1)
	ON_BN_CLICKED(IDC_BUTTON_MAP2, OnBnClickedButtonMap2)
	ON_BN_CLICKED(IDC_BUTTON_MAP3, OnBnClickedButtonMap3)
	ON_BN_CLICKED(IDC_BUTTON_MAP4, OnBnClickedButtonMap4)
	ON_BN_CLICKED(IDC_BUTTON_MAP5, OnBnClickedButtonMap5)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ALPHA, OnDeltaposSpinAlpha)
END_MESSAGE_MAP()


// CCubeMapDlg �޽��� ó�����Դϴ�.

void CCubeMapDlg::OnBnClickedButtonMap0()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CString StrInitDir = pFrame->m_strAppPath + "\\Texture";
	dlg.m_ofn.lpstrInitialDir = StrInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		m_strMap0 = dlg.GetFileName ();

		UpdateData( FALSE );		
	}
}

void CCubeMapDlg::OnBnClickedButtonMap1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CString StrInitDir = pFrame->m_strAppPath + "\\Texture";
	dlg.m_ofn.lpstrInitialDir = StrInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		m_strMap1 = dlg.GetFileName ();
		UpdateData( FALSE );		
	}
}

void CCubeMapDlg::OnBnClickedButtonMap2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CString StrInitDir = pFrame->m_strAppPath + "\\Texture";
	dlg.m_ofn.lpstrInitialDir = StrInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		m_strMap2 = dlg.GetFileName ();
		UpdateData( FALSE );		
	}
}

void CCubeMapDlg::OnBnClickedButtonMap3()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CString StrInitDir = pFrame->m_strAppPath + "\\Texture";
	dlg.m_ofn.lpstrInitialDir = StrInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		m_strMap3 = dlg.GetFileName ();
		UpdateData( FALSE );		
	}
}

void CCubeMapDlg::OnBnClickedButtonMap4()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CString StrInitDir = pFrame->m_strAppPath + "\\Texture";
	dlg.m_ofn.lpstrInitialDir = StrInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		m_strMap4 = dlg.GetFileName ();
		UpdateData( FALSE );		
	}
}

void CCubeMapDlg::OnBnClickedButtonMap5()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, 
		OFN_HIDEREADONLY, szFilter, this );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CString StrInitDir = pFrame->m_strAppPath + "\\Texture";
	dlg.m_ofn.lpstrInitialDir = StrInitDir;

	if ( dlg.DoModal() == IDOK )
	{
		m_strMap5 = dlg.GetFileName ();
		UpdateData( FALSE );		
	}
}

void CCubeMapDlg::SetInit(char* strMap0, char* strMap1, char* strMap2, char* strMap3, char* strMap4, char* strMap5, float fAlpha)
{
	m_strMap00 = strMap0;
	m_strMap11 = strMap1;
	m_strMap22 = strMap2;
	m_strMap33 = strMap3;
	m_strMap44 = strMap4;
	m_strMap55 = strMap5;
	m_fAlpha0 = fAlpha;	
}

BOOL CCubeMapDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_strMap0 = m_strMap00;
	m_strMap1 = m_strMap11;
	m_strMap2 = m_strMap22;
	m_strMap3 = m_strMap33;
	m_strMap4 = m_strMap44;
	m_strMap5 = m_strMap55;
	m_fAlpha = m_fAlpha0;

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CCubeMapDlg::OnDeltaposSpinAlpha(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	
	if(  pNMUpDown->iDelta < 0 )
		m_fAlpha += 0.1f;
	else
		m_fAlpha -= 0.1f;

	if( m_fAlpha > 1.0f ) m_fAlpha = 1.0f;
	if( m_fAlpha < 0.0f ) m_fAlpha = 0.0f;	

	UpdateData( FALSE );
		
	*pResult = 0;
}
