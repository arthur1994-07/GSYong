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
	CMoneyStat();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnCompleted(WPARAM wParam, LPARAM lParam);
	// Ȱ������ ĳ���� ����
	int m_nChaActive;
	// ������ ĳ���� ����
	int m_nChaDeleted;
	// ��ü ĳ���� ����
	int m_nChaTotal;
	// ��ü ��Ŀ���� (����ڼ�)
	int m_nUserInvenCount;
	// ��ü Ŭ������
	int m_nClubCount;
	// ��ü ĳ���� �κ��� �� ��
	LONGLONG m_llChaMoney;
	// ��Ŀ�� �ִ� ���� ��
	LONGLONG m_llUserInvenMoney;
	// Ŭ�� â�� �ִ� ���� ��
	LONGLONG m_llClubMoney;
	// ��ü ������ ���� ��
	LONGLONG m_llTotalMoney;
	// ĳ���ʹ� ��� �� ������
	LONGLONG m_llChaMoneyAvg;
	// ����ڴ� ��� ��
	LONGLONG m_llUserMoneyAvg;
	// Ŭ���� ��� ��
	LONGLONG m_llClubMoneyAvg;
	afx_msg void OnBnClickedBtnMoneyStart();
};
