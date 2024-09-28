#pragma once
#include "afxcmn.h"


// CEffectNamePage 대화 상자입니다.
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

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_NAMELIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_EffNameList;
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonResetEffect();
	afx_msg void OnBnClickedButtonPartsSave();
};
