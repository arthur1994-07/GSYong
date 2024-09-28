#pragma once

#define WM_MONEYSTAT_COMPLETED WM_USER + 199

namespace gmce
{
	class IDBManager;
}

class CMoneyStat : public CFormView
{
	DECLARE_DYNCREATE(CMoneyStat)

protected:
	CMoneyStat();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CMoneyStat();

public:
	enum { IDD = IDD_MONEYSTAT };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	BOOL m_bRunning;
	HANDLE m_hThread;
	void StatisticProc(void* p);

	gmce::IDBManager *m_pDBMan;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnCompleted(WPARAM wParam, LPARAM lParam);
	// 활동중인 캐릭터 갯수
	int m_nChaActive;
	// 삭제된 캐릭터 갯수
	int m_nChaDeleted;
	// 전체 캐릭터 갯수
	int m_nChaTotal;
	// 전체 락커갯수 (사용자수)
	int m_nUserInvenCount;
	// 전체 클럽갯수
	int m_nClubCount;
	// 전체 캐릭터 인벤의 돈 합
	LONGLONG m_llChaMoney;
	// 락커에 있는 돈의 합
	LONGLONG m_llUserInvenMoney;
	// 클럽 창고에 있는 돈의 합
	LONGLONG m_llClubMoney;
	// 전체 서버의 돈의 합
	LONGLONG m_llTotalMoney;
	// 캐릭터당 평균 돈 보유량
	LONGLONG m_llChaMoneyAvg;
	// 사용자당 평균 돈
	LONGLONG m_llUserMoneyAvg;
	// 클럽당 평균 돈
	LONGLONG m_llClubMoneyAvg;
	afx_msg void OnBnClickedBtnMoneyStart();
};
