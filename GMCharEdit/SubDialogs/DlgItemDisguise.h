#pragma once
#include "afxwin.h"


// CDlgItemDisguise

class CDlgItemDisguise : public CDialog
{
	DECLARE_DYNAMIC(CDlgItemDisguise)

public:
	CDlgItemDisguise(CWnd* pParent = NULL);
	virtual ~CDlgItemDisguise();

	enum { IDD = IDD_DLG_ITEM_DISGUISE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedCheckValid();
	afx_msg void OnBnClickedOk();

public:
	CTime	m_cTime;

	CEdit	m_ctrlYear;
	CEdit	m_ctrlMonth;
	CEdit	m_ctrlDay;
	CEdit	m_ctrlHour;
	CEdit	m_ctrlMinute;

	UINT	m_uiYear;
	UINT	m_uiMonth;
	UINT	m_uiDay;
	UINT	m_uiHour;
	UINT	m_uiMinute;
	BOOL	m_bDisguise;
};