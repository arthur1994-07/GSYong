// RanStatisticsDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "RanStatistics.h"
#include "RanStatisticsDlg.h"
#include "ODBCManager.h"
#include "ADOManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� ������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ����

// ����
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()



////////////////////////////////////////////////////////////////////////////



// CRanStatisticsDlg ��ȭ ����

CRanStatisticsDlg::CRanStatisticsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRanStatisticsDlg::IDD, pParent)	
	, m_bExport(FALSE)	
{
	m_ctTo = CTime::GetCurrentTime();

	int nMonth = m_ctTo.GetMonth();
	int nYear = m_ctTo.GetYear();

	nYear = ( nMonth > 3 ? nYear : nYear - 1 );
	nMonth = ( nMonth > 3 ? nMonth - 3 : nMonth + 9 );

	m_ctFrom = CTime( nYear, nMonth, 1, 0, 0, 0 );

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pPS = new CMainPropertySheet(IDS_PROPERTYSHEET);
	m_pPS->SetMainDlg(this);
}


CRanStatisticsDlg::~CRanStatisticsDlg()
{
	delete m_pPS;
	m_pPS = NULL;
}


void CRanStatisticsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);	
	DDX_Check(pDX, IDC_CHECK_EXPORT, m_bExport);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_FROM, m_ctFrom);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_TO, m_ctTo);
}


BEGIN_MESSAGE_MAP(CRanStatisticsDlg, CDialog)
	ON_WM_SYSCOMMAND()	
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP	
END_MESSAGE_MAP()



// CRanStatisticsDlg �޽��� ó����

BOOL CRanStatisticsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	UpdateData(TRUE);
	

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ���� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	// �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.


	//Positioning TabControl and pages.	
	
	CPropertyPage* m_pPage01 = (CPropertyPage*)&m_pPS->m_Page01;
	CPropertyPage* m_pPage02 = (CPropertyPage*)&m_pPS->m_Page02; 
	CPropertyPage* m_pPage03 = (CPropertyPage*)&m_pPS->m_Page03;
	CPropertyPage* m_pPage04 = (CPropertyPage*)&m_pPS->m_Page04;
	
	
	//resize window to maximum screen size 
	/*CSize screenSize; 
	screenSize.cx = ::GetSystemMetrics(SM_CXSCREEN); 
	screenSize.cy = ::GetSystemMetrics(SM_CYSCREEN); 
	MoveWindow(0,0, screenSize.cx, screenSize.cy); */

	
	CRect rcDlg, rcSheet;
	GetWindowRect(&rcDlg);	
	
	if( !m_pPS->Create( this, WS_CHILD | WS_VISIBLE ) )
	{
		delete m_pPS;
		m_pPS = NULL;

		return FALSE;
	}
	
	m_pPS->ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	m_pPS->ModifyStyle( 0, WS_TABSTOP );

	//Positioning and Resize of PropertySheet
	m_pPS->SetWindowPos(NULL, rcDlg.left, rcDlg.top, rcDlg.Width(),
									rcDlg.Height() - 100, SWP_NOZORDER | SWP_NOACTIVATE );

	m_pPS->GetWindowRect(&rcSheet);
	ScreenToClient(&rcSheet);
	
	//Resize of TabControl in PropertySheet
	m_pPS->GetTabControl()->SetWindowPos( NULL, rcSheet.left, rcSheet.top, 
								rcSheet.Width(), rcSheet.Height(), SWP_NOZORDER | SWP_NOACTIVATE );
	
	m_pPS->SetActivePage(m_pPage01);


	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
}

void CRanStatisticsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸����� 
// �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
// �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CRanStatisticsDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�. 
HCURSOR CRanStatisticsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
