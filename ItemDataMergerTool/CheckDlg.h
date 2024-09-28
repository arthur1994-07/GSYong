#pragma once
#include "afxcmn.h"


// CCheckDlg 대화 상자입니다.

struct SIDLIST
{
	SNATIVEID sCheckID;
	SNATIVEID sApplyID;

	SIDLIST( SNATIVEID sInputCheckID, SNATIVEID sInputApplyID )
		: sCheckID(sInputCheckID)
		, sApplyID(sInputApplyID)
	{
	}
	SIDLIST( SNATIVEID sInputID )
		: sCheckID(sInputID)
		, sApplyID(sInputID)
	{
	}

};

class CCheckDlg : public CDialog
{
	DECLARE_DYNAMIC(CCheckDlg)

public:
	CCheckDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	CCheckDlg(std::vector<DWORD> vecCheckItemList, GLItemMan *pItemMan1, GLItemMan *pItemMan2 );  
	virtual ~CCheckDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CHECK_DIALOG };

	std::vector<SIDLIST> m_vecIDList;
private:
	GLItemMan		  *m_pBaseItemMan;
	GLItemMan		  *m_pTargetItemMan;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

public:
	CListCtrl m_list_Check;
	virtual BOOL OnInitDialog();		
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMDblclkCheckList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedIdchangeButton();
	void IDCheck();
	void UpdateIDList();
	afx_msg void OnBnClickedOk();
};
