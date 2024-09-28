// RanSNSDlg.cpp : 구현 파일
//
#include "stdafx.h"

#include "RanSNS.h"
#include "RanSNSDlg.h"
#include ".\ransnsdlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRanSNSDlg 대화 상자


CRanSNSDlg::CRanSNSDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRanSNSDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pPS = new CPropSht(IDS_PROPSHEET);
}

CRanSNSDlg::~CRanSNSDlg()
{
	delete m_pPS;
	m_pPS = NULL;
}

void CRanSNSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRanSNSDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


// CRanSNSDlg 메시지 처리기

BOOL CRanSNSDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.


	

	CPropertyPage* pPage01 = (CPropertyPage*)&m_pPS->m_Page01;
	CPropertyPage* pPage02 = (CPropertyPage*)&m_pPS->m_Page02;

	
	
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
									rcDlg.Height(), SWP_NOZORDER | SWP_NOACTIVATE );

	m_pPS->GetWindowRect(&rcSheet);
	ScreenToClient(&rcSheet);
	
	//Resize of TabControl in PropertySheet
	m_pPS->GetTabControl()->SetWindowPos( NULL, rcSheet.left, rcSheet.top, 
								rcSheet.Width(), rcSheet.Height(), SWP_NOZORDER | SWP_NOACTIVATE );
	
	m_pPS->SetActivePage(pPage01);


	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CRanSNSDlg::OnPaint() 
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
HCURSOR CRanSNSDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CRanSNSDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialog::OnSysCommand(nID, lParam);
}
