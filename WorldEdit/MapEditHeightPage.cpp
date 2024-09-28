// WeatherPage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "WorldEdit.h"

#include "ColorPickerDlg.h"
#include "sheetWithTab.h"

#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"
#include "MapEditHeightPage.h"
#include ".\mapeditheightpage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const COLORREF CMapEditHeightPage::CLOUDBLUE	= RGB(128, 184, 223);
const COLORREF CMapEditHeightPage::WHITE		= RGB(255, 255, 255);
const COLORREF CMapEditHeightPage::BLACK		= RGB(1, 1, 1);
const COLORREF CMapEditHeightPage::DKGRAY		= RGB(128, 128, 128);
const COLORREF CMapEditHeightPage::LLTGRAY		= RGB(220, 220, 220);
const COLORREF CMapEditHeightPage::LTGRAY		= RGB(192, 192, 192);
const COLORREF CMapEditHeightPage::YELLOW		= RGB(255, 255, 0);
const COLORREF CMapEditHeightPage::DKYELLOW		= RGB(128, 128, 0);
const COLORREF CMapEditHeightPage::COLORREF_RED = RGB(255, 0, 0);
const COLORREF CMapEditHeightPage::DKRED		= RGB(128, 0, 0);
const COLORREF CMapEditHeightPage::COLORREF_BLUE = RGB(0, 0, 255);
const COLORREF CMapEditHeightPage::LBLUE		= RGB(192, 192, 255);
const COLORREF CMapEditHeightPage::DKBLUE		= RGB(0, 0, 128);
const COLORREF CMapEditHeightPage::CYAN			= RGB(0, 255, 255);
const COLORREF CMapEditHeightPage::DKCYAN		= RGB(0, 128, 128);
const COLORREF CMapEditHeightPage::COLORREF_GREEN = RGB(0, 255, 0);
const COLORREF CMapEditHeightPage::DKGREEN		= RGB(0, 128, 0);
const COLORREF CMapEditHeightPage::MAGENTA		= RGB(255, 0, 255);
const COLORREF CMapEditHeightPage::DKMAGENTA	= RGB(128, 0, 128);

// CMapEditHeightPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CMapEditHeightPage, CPropertyPage)
CMapEditHeightPage::CMapEditHeightPage()
	: CPropertyPage(CMapEditHeightPage::IDD),
	m_bDlgInit(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CMapEditHeightPage::~CMapEditHeightPage()
{
}

void CMapEditHeightPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMapEditHeightPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_GROUND, OnBnClickedButtonGround)
	ON_BN_CLICKED(IDC_BUTTON_UNEVEN, OnBnClickedButtonUneven)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnBnClickedButtonDown)
	ON_BN_CLICKED(IDC_BUTTON_RELATIVE_HEIGHT, OnBnClickedButtonRelativeHeight)
	ON_BN_CLICKED(IDC_BUTTON_ABSOLUTE_HEIGHT, OnBnClickedButtonAbsoluteHeight)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT_HEIGHT_VALUE, OnEnChangeEditHeightValue)
END_MESSAGE_MAP()


// CMapEditHeightPage �޽��� ó�����Դϴ�.

BOOL CMapEditHeightPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CWnd*		pWnd;

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	//m_sOldProperty	= m_sProperty	= DxFogMan::GetInstance().GetProperty();
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_sProperty = pView->GetMapEditMan()->GetProperty();
	
	m_bDlgInit = TRUE;
	
	char szValue[256];
	pWnd = GetDlgItem ( IDC_EDIT_HEIGHT_VALUE );
	_gcvt ( pView->GetMapEditMan()->m_fHeightValue_EDIT, 7, szValue );
	pWnd->SetWindowText ( szValue );

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	cClrGround.SubclassDlgItem ( IDC_BUTTON_GROUND, this );
	cClrUneven.SubclassDlgItem ( IDC_BUTTON_UNEVEN, this );
	cClrUp.SubclassDlgItem ( IDC_BUTTON_UP, this );
	cClrDown.SubclassDlgItem ( IDC_BUTTON_DOWN, this );
	cClrRelativeHeight.SubclassDlgItem ( IDC_BUTTON_RELATIVE_HEIGHT, this );
	cClrAbsoluteHeight.SubclassDlgItem ( IDC_BUTTON_ABSOLUTE_HEIGHT, this );

	if ( pView->GetMapEditMan()->m_bUneven_EDIT )
	{
		cClrGround.SetColour ( BLACK, LTGRAY );
		cClrUneven.SetColour ( BLACK, LBLUE );
	}
	else
	{
		cClrGround.SetColour ( BLACK, LBLUE );
		cClrUneven.SetColour ( BLACK, LTGRAY );
	}

	if ( pView->GetMapEditMan()->m_bUp_EDIT )
	{
		cClrUp.SetColour ( BLACK, LBLUE );
		cClrDown.SetColour ( BLACK, LTGRAY );
	}
	else
	{
		cClrUp.SetColour ( BLACK, LTGRAY );
		cClrDown.SetColour ( BLACK, LBLUE );
	}

	if ( pView->GetMapEditMan()->m_bAbsolute_EDIT )
	{
		cClrRelativeHeight.SetColour ( BLACK, LTGRAY );
		cClrAbsoluteHeight.SetColour ( BLACK, LBLUE );
	}
	else
	{
		cClrRelativeHeight.SetColour ( BLACK, LBLUE );
		cClrAbsoluteHeight.SetColour ( BLACK, LTGRAY );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CMapEditHeightPage::UpdateContrl ()
{
	if ( !m_bDlgInit )	return;

	OnInitDialog();		// �ٽ� �ʱ�ȭ�� ��Ų��.
}

void CMapEditHeightPage::OnBnClickedButtonApply()
{
	//// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	//CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	//m_sProperty = pView->GetMapEditMan()->GetProperty();

	//CWnd*		pWnd;
	//CString		strText;

	//pWnd = GetDlgItem ( IDC_EDIT_HEIGHT_VALUE );
	//pWnd->GetWindowText ( strText );
	//pView->GetMapEditMan()->m_fHeightValue_EDIT = (float)atof ( strText.GetString(256) );

	////pView->GetMapEditMan()->SetProperty ( m_sProperty );
}

void CMapEditHeightPage::OnBnClickedButtonGround()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetMapEditMan()->m_bUneven_EDIT = FALSE;

	cClrGround.SetColour ( BLACK, LBLUE );
	cClrUneven.SetColour ( BLACK, LTGRAY );
}

void CMapEditHeightPage::OnBnClickedButtonUneven()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetMapEditMan()->m_bUneven_EDIT = TRUE;

	cClrGround.SetColour ( BLACK, LTGRAY );
	cClrUneven.SetColour ( BLACK, LBLUE );
}

void CMapEditHeightPage::OnBnClickedButtonUp()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetMapEditMan()->m_bUp_EDIT = TRUE;

	cClrUp.SetColour ( BLACK, LBLUE );
	cClrDown.SetColour ( BLACK, LTGRAY );
}

void CMapEditHeightPage::OnBnClickedButtonDown()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetMapEditMan()->m_bUp_EDIT = FALSE;

	cClrUp.SetColour ( BLACK, LTGRAY );
	cClrDown.SetColour ( BLACK, LBLUE );
}

void CMapEditHeightPage::OnBnClickedButtonRelativeHeight()		// ��� ����
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetMapEditMan()->m_bAbsolute_EDIT = FALSE;

	cClrRelativeHeight.SetColour ( BLACK, LBLUE );//LBLUE, LBLUE );
	cClrAbsoluteHeight.SetColour ( BLACK, LTGRAY );//MAGENTA, LBLUE );
}

void CMapEditHeightPage::OnBnClickedButtonAbsoluteHeight()		// ���� ����
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->GetMapEditMan()->m_bAbsolute_EDIT = TRUE;

	cClrRelativeHeight.SetColour ( BLACK, LTGRAY );//LBLUE, LBLUE );
	cClrAbsoluteHeight.SetColour ( BLACK, LBLUE );//MAGENTA, LBLUE );
}

void CMapEditHeightPage::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CPropertyPage::OnPaint()��(��) ȣ������ ���ʽÿ�.
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CMapEditHeightPage::OnQueryDragIcon()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	return (HCURSOR) m_hIcon;
//	return CPropertyPage::OnQueryDragIcon();
}

void CMapEditHeightPage::OnEnChangeEditHeightValue()
{
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CPropertyPage::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	m_sProperty = pView->GetMapEditMan()->GetProperty();

	CWnd*		pWnd;
	CString		strText;

	pWnd = GetDlgItem ( IDC_EDIT_HEIGHT_VALUE );
	pWnd->GetWindowText ( strText );
	pView->GetMapEditMan()->m_fHeightValue_EDIT = (float)atof ( strText.GetString() );
}
