#pragma once
#include "afxcmn.h"
#include <vector>
#include "afxwin.h"
struct G_SERVER_INFO_DLG;
class CServerManagerDlg;

// InspectionRewardPolicy 대화 상자입니다.
class InspectionRewardPolicy : public CDialog
{
	DECLARE_DYNAMIC(InspectionRewardPolicy)

public:
	InspectionRewardPolicy(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~InspectionRewardPolicy();
    void SetServerInfo( const std::vector<G_SERVER_INFO_DLG*>& vecInfo );


// 대화 상자 데이터입니다.
	enum { IDD = IDD_INSPECTION_REWARD_POLICY };
	CServerManagerDlg* m_pDlg;

protected:
    BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	DECLARE_MESSAGE_MAP()

private:
    void ServerGroupListInit();
    CListCtrl m_pServerGroupList;

private:
	void Reset();
    std::vector<G_SERVER_INFO_DLG*> m_vecServerList;
	int m_nSelectServerGroup;
	__time64_t m_tStart;
	__time64_t m_tEnd;
	int m_nReward;

public:
	afx_msg void OnDestroy();
    afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnBnClickedButton3();

	CEdit Edit_Interval;
	CEdit Edit_Reword;
	CEdit Edit_TimeView;
	CEdit Edit_RewordView;
	CButton Btn_Commit;
	CButton Btn_Send;
	

	afx_msg void OnBnClickedOk();
	afx_msg void OnEnSetfocusEdit3();
	afx_msg void OnEnSetfocusEdit2();
	afx_msg void OnBnClickedButton5();
};
