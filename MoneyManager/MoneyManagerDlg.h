// MoneyManagerDlg.h : 헤더 파일
//

#pragma once

#include <set>
#include "afxcmn.h"
#include "../RanLogic/Util/s_CCfg.h"

// CMoneyManagerDlg 대화 상자
class CMoneyManagerDlg : public CDialog
{
// 생성
public:
	CMoneyManagerDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_MONEYMANAGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

	BOOL LoadCfg();
	BOOL OpenDatabase();

// 구현
protected:
	HICON m_hIcon;
    std::set<int> m_setSTOUserNum;
    CCfg* m_pCfg;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL DestroyWindow();
	CListCtrl m_list_FieldInfo;
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedCheckMultiply();
	CProgressCtrl m_prog_Complete;
	afx_msg void OnBnClickedButtonMoneycheck();	
	afx_msg void OnBnClickedButtonReencrypt();
	afx_msg void OnBnClickedButtonNewencrypt();
};
