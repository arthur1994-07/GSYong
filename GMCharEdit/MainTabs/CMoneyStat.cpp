// CMoneyStat.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include <process.h>
#include "../GMCharEdit.h"
#include "CMoneyStat.h"

#include "../Util/CGmCfg.h"

#include "../Database/ADO/ADOManager.h"
#include "../Database/ODBC/CGMOdbcManager.h"

#include "../MainFrm.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

static unsigned int WINAPI MoneyStatsticThread(void* p)
{
	if (p == NULL)
	{
		return 0;
	}
	else
	{
		CMoneyStat* pDlg = reinterpret_cast<CMoneyStat*> (p);
		pDlg->StatisticProc(p);
		return 0;
	}
}

// CMoneyStat

IMPLEMENT_DYNCREATE(CMoneyStat, CFormView)

CMoneyStat::CMoneyStat()
	: CFormView(CMoneyStat::IDD)
	, m_nChaActive(0)
	, m_nChaDeleted(0)
	, m_nChaTotal(0)
	, m_nUserInvenCount(0)
	, m_nClubCount(0)
	, m_llChaMoney(0)
	, m_llUserInvenMoney(0)
	, m_llClubMoney(0)
	, m_llTotalMoney(0)
	, m_llChaMoneyAvg(0)
	, m_llUserMoneyAvg(0)
	, m_llClubMoneyAvg(0)
	, m_bRunning(FALSE)
	, m_hThread(NULL)
{
	m_pDBMan = CGmCfg::GetInstance()->GetDBMan();//CADOManager::GetInstance();
}

CMoneyStat::~CMoneyStat()
{
}

void CMoneyStat::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, m_nChaActive);
	DDX_Text(pDX, IDC_EDIT31, m_nChaDeleted);
	DDX_Text(pDX, IDC_EDIT40, m_nChaTotal);
	DDX_Text(pDX, IDC_EDIT41, m_nUserInvenCount);
	DDX_Text(pDX, IDC_EDIT42, m_nClubCount);
	DDX_Text(pDX, IDC_EDIT4, m_llChaMoney);
	DDX_Text(pDX, IDC_EDIT5, m_llUserInvenMoney);
	DDX_Text(pDX, IDC_EDIT6, m_llClubMoney);
	DDX_Text(pDX, IDC_EDIT7, m_llTotalMoney);
	DDX_Text(pDX, IDC_EDIT8, m_llChaMoneyAvg);
	DDX_Text(pDX, IDC_EDIT9, m_llUserMoneyAvg);
	DDX_Text(pDX, IDC_EDIT10, m_llClubMoneyAvg);
}

BEGIN_MESSAGE_MAP(CMoneyStat, CFormView)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_MONEY_START, OnBnClickedBtnMoneyStart)
	ON_MESSAGE(WM_MONEYSTAT_COMPLETED, OnCompleted)
END_MESSAGE_MAP()

// CMoneyStat �����Դϴ�.

#ifdef _DEBUG
void CMoneyStat::AssertValid() const
{
	CFormView::AssertValid();
}

void CMoneyStat::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

// CMoneyStat �޽��� ó�����Դϴ�.

LRESULT CMoneyStat::OnCompleted(WPARAM wParam, LPARAM lParam)
{
	UpdateData(FALSE);
	return NULL;
}

void CMoneyStat::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	UpdateData(FALSE);
}

void CMoneyStat::OnClose()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (m_bRunning == TRUE) // �������̸�...
	{
		m_bRunning = FALSE; // �ߴܽ�Ű��
		Sleep( 1000 ); // 1 �ʴ��
	}
	else
	{
		CFormView::OnClose();
	}
}

void CMoneyStat::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

void CMoneyStat::OnBnClickedBtnMoneyStart()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_bRunning == TRUE) // �۾���
	{
		MessageBox(_T("Already running, First Stop Process"));
		return;
	}
	else // �۾�����
	{
		UpdateData(TRUE);
		DWORD dwThreadID = 121;
		/*
		m_hThread = ::CreateThread(NULL,
							 	   0,
							 	   (LPTHREAD_START_ROUTINE) MoneyStatsticThread,
								   this,
								   0,
								   &dwThreadID);
		*/
		m_hThread = (HANDLE) ::_beginthreadex(
									NULL,
							 		0,
							 		MoneyStatsticThread,
									this,
									0,
									(unsigned int*) &dwThreadID );
		if (m_hThread == NULL)
		{
			// ������ ������ �����Ͽ����ϴ�.
			MessageBox(_T("Thread Failed"));
		}
		else
		{
			///////////////////////////////////////////////////////////////////////////
			// GM Log
			CString strUserIP = m_pDBMan->GetLocalIP();
			CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
			int nGmNum = pFrame->m_nUserNum;
			std::strstream strGmCmd;
			strGmCmd << "Money Statistic";
			strGmCmd << '\0';
			m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
			strGmCmd.freeze( false );									// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
			///////////////////////////////////////////////////////////////////////////
		}
	}	
}

void CMoneyStat::StatisticProc(void* p)
{
	CMoneyStat* pDlg = reinterpret_cast<CMoneyStat*> (p);

	m_bRunning = TRUE;

	// Ȱ������ ĳ���� ����
	m_nChaActive = m_pDBMan->GetActiveChaNum();
	::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);
	
	// ������ ĳ���� ����
	m_nChaDeleted = m_pDBMan->GetDeletedChaNum();
	::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);
	
	// ��ü ĳ���� ����
	m_nChaTotal = m_nChaActive + m_nChaDeleted;
	::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);
	
	// ��ü ��Ŀ���� (����ڼ�)
	m_nUserInvenCount = m_pDBMan->GetUserInvenNum();
	::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);

	// ��ü Ŭ������
	m_nClubCount = m_pDBMan->GetClubNum();
	::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);

	// ��ü ĳ���� �κ��� �� ��
	m_llChaMoney = m_pDBMan->GetSumChaMoney();
	::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);
	
	// ��Ŀ�� �ִ� ���� ��
	m_llUserInvenMoney = m_pDBMan->GetSumUserInvenMoney();
	::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);

	// Ŭ�� â�� �ִ� ���� ��
	m_llClubMoney = m_pDBMan->GetSumClubInvenMoney();
	::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);

	// ��ü ������ ���� ��
	m_llTotalMoney = m_llChaMoney + m_llUserInvenMoney + m_llClubMoney;
	::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);

	// ĳ���ʹ� ��� �� ������
	if (m_nChaActive != 0 && m_nChaActive > 0)
	{
		m_llChaMoneyAvg = m_llTotalMoney / (LONGLONG) m_nChaActive;
		::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);
	}

	// ����ڴ� ��� ��
	if (m_nUserInvenCount != 0 && m_nUserInvenCount > 0)
	{
		m_llUserMoneyAvg = m_llTotalMoney / (LONGLONG) m_nUserInvenCount;
		::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);
	}

	// Ŭ���� ��� ��
	if (m_nClubCount != 0 && m_nClubCount > 0)
	{
		m_llClubMoneyAvg = m_llClubMoney / (LONGLONG) m_nClubCount;
		::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);
	}	

	m_bRunning = FALSE;

	::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);
}