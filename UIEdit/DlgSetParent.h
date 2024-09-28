#pragma once

#include "Resource.h"
#include "afxwin.h"
#include "afxcmn.h"


class CDlgSetParent : public CDialog
{
public:
	enum { IDD = IDD_DIALOG_SET_PARENT };
	enum { LIST_NO = 0, LIST_NAME };

public:
	CDlgSetParent(CWnd* pParent = NULL);
	virtual ~CDlgSetParent();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	DECLARE_DYNAMIC(CDlgSetParent)
	DECLARE_MESSAGE_MAP()
public:
	int			m_nSelectedItem;
	CListCtrl	m_ParentList;
public:
	afx_msg void OnPaint();
	afx_msg void OnBnClickedOk();
	afx_msg void OnLvnItemchangedListParent(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListParent(NMHDR *pNMHDR, LRESULT *pResult);
};