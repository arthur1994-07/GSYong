#pragma once
#include "afxcmn.h"


// CEffectNamePage ��ȭ �����Դϴ�.
class CsheetWithTabAnim;

class CEffectNamePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CEffectNamePage)

protected:
	CsheetWithTabAnim	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTabAnim *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void SetData();
	void ResetEffect();

public:
	CEffectNamePage();
	virtual ~CEffectNamePage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFF_NAMELIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_EffNameList;
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonResetEffect();
	afx_msg void OnBnClickedButtonPartsSave();
};
