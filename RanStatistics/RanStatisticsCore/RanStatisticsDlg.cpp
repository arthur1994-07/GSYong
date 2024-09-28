// RanStatisticsDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "RanStatistics.h"
#include "RanStatisticsDlg.h"
#include "ODBCManager.h"
#include "ADOManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
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



// CRanStatisticsDlg 대화 상자

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



// CRanStatisticsDlg 메시지 처리기

BOOL CRanStatisticsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	UpdateData(TRUE);
	

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.


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


	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CRanStatisticsDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CRanStatisticsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

