// RanSNSDlg.cpp : ���� ����
//
#include "stdafx.h"

#include "RanSNS.h"
#include "RanSNSDlg.h"
#include ".\ransnsdlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRanSNSDlg ��ȭ ����


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


// CRanSNSDlg �޽��� ó����

BOOL CRanSNSDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	// �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.


	

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


	
	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸����� 
// �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
// �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CRanSNSDlg::OnPaint() 
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
HCURSOR CRanSNSDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CRanSNSDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CDialog::OnSysCommand(nID, lParam);
}
