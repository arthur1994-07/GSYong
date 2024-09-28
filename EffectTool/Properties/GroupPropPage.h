#pragma once
#include "afxwin.h"
#include "../resource.h"

// CGroupPropPage ��ȭ �����Դϴ�.
struct EFF_PROPGROUP;
class CsheetWithTab;

class CGroupPropPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CGroupPropPage)

protected:
	CsheetWithTab*			m_pSheetTab;
	EFF_PROPGROUP*			m_pGroupProp;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab )	{ m_pSheetTab = pSheetTab; }

public:
	void SetProperty ( EFF_PROPGROUP* pGroupProp );

public:
	CGroupPropPage();
	virtual ~CGroupPropPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_GROUPPROP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonok();
	afx_msg void OnBnClickedCheckNeverdie();
	afx_msg void OnBnClickedRadioAverage();
	afx_msg void OnBnClickedRadioSlowfast();
	afx_msg void OnBnClickedRadioFastslow();
};
