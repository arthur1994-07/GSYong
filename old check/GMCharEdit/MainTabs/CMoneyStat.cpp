// CMoneyStat.cpp : 구현 파일입니다.
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

// CMoneyStat 진단입니다.

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

// CMoneyStat 메시지 처리기입니다.

LRESULT CMoneyStat::OnCompleted(WPARAM wParam, LPARAM lParam)
{
	UpdateData(FALSE);
	return NULL;
}

void CMoneyStat::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	UpdateData(FALSE);
}

void CMoneyStat::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (m_bRunning == TRUE) // 실행중이면...
	{
		m_bRunning = FALSE; // 중단시키고
		Sleep( 1000 ); // 1 초대기
	}
	else
	{
		CFormView::OnClose();
	}
}

void CMoneyStat::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CMoneyStat::OnBnClickedBtnMoneyStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_bRunning == TRUE) // 작업중
	{
		MessageBox(_T("Already running, First Stop Process"));
		return;
	}
	else // 작업시작
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
			// 쓰래드 생성에 실패하였습니다.
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
			strGmCmd.freeze( false );									// Note : std::strstream의 freeze. 안 하면 Leak 발생.
			///////////////////////////////////////////////////////////////////////////
		}
	}	
}

void CMoneyStat::StatisticProc(void* p)
{
	CMoneyStat* pDlg = reinterpret_cast<CMoneyStat*> (p);

	m_bRunning = TRUE;

	// 활동중인 캐릭터 갯수
	m_nChaActive = m_pDBMan->GetActiveChaNum();
	::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);
	
	// 삭제된 캐릭터 갯수
	m_nChaDeleted = m_pDBMan->GetDeletedChaNum();
	::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);
	
	// 전체 캐릭터 갯수
	m_nChaTotal = m_nChaActive + m_nChaDeleted;
	::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);
	
	// 전체 락커갯수 (사용자수)
	m_nUserInvenCount = m_pDBMan->GetUserInvenNum();
	::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);

	// 전체 클럽갯수
	m_nClubCount = m_pDBMan->GetClubNum();
	::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);

	// 전체 캐릭터 인벤의 돈 합
	m_llChaMoney = m_pDBMan->GetSumChaMoney();
	::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);
	
	// 락커에 있는 돈의 합
	m_llUserInvenMoney = m_pDBMan->GetSumUserInvenMoney();
	::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);

	// 클럽 창고에 있는 돈의 합
	m_llClubMoney = m_pDBMan->GetSumClubInvenMoney();
	::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);

	// 전체 서버의 돈의 합
	m_llTotalMoney = m_llChaMoney + m_llUserInvenMoney + m_llClubMoney;
	::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);

	// 캐릭터당 평균 돈 보유량
	if (m_nChaActive != 0 && m_nChaActive > 0)
	{
		m_llChaMoneyAvg = m_llTotalMoney / (LONGLONG) m_nChaActive;
		::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);
	}

	// 사용자당 평균 돈
	if (m_nUserInvenCount != 0 && m_nUserInvenCount > 0)
	{
		m_llUserMoneyAvg = m_llTotalMoney / (LONGLONG) m_nUserInvenCount;
		::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);
	}

	// 클럽당 평균 돈
	if (m_nClubCount != 0 && m_nClubCount > 0)
	{
		m_llClubMoneyAvg = m_llClubMoney / (LONGLONG) m_nClubCount;
		::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);
	}	

	m_bRunning = FALSE;

	::SendMessage(pDlg->m_hWnd, WM_MONEYSTAT_COMPLETED, 0, 0);
}