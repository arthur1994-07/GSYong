#pragma once
#include "afxcmn.h"


// CCheckDlg ��ȭ �����Դϴ�.

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
	CCheckDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	CCheckDlg(std::vector<DWORD> vecCheckItemList, GLItemMan *pItemMan1, GLItemMan *pItemMan2 );  
	virtual ~CCheckDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CHECK_DIALOG };

	std::vector<SIDLIST> m_vecIDList;
private:
	GLItemMan		  *m_pBaseItemMan;
	GLItemMan		  *m_pTargetItemMan;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
