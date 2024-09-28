#pragma once

#include "afxwin.h"
#include "../resource.h"

// CMoveRotatePage 대화 상자입니다.
class CsheetWithTab;
struct MOVETARGET_PROPERTY;

// CMoveTargetPage 대화 상자입니다.

class CMoveTargetPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMoveTargetPage)

protected:
	CsheetWithTab*			m_pSheetTab;
	MOVETARGET_PROPERTY*	m_pProperty;

public:
	void SetProperty ( EFF_PROPERTY* pProp );
	void SetSheetTab ( CsheetWithTab *pSheetTab )	{ m_pSheetTab = pSheetTab; }

public:
	CMoveTargetPage();
	virtual ~CMoveTargetPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MOVETARGET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedRadioSTarget();
	afx_msg void OnBnClickedRadioSDirect();
	afx_msg void OnBnClickedButtonSDirect();
	afx_msg void OnBnClickedCheckSRandom();
	afx_msg void OnBnClickedCheckSGuided();
	afx_msg void OnBnClickedRadioECrash();
	afx_msg void OnBnClickedRadioETimeover();
	afx_msg void OnBnClickedRadioEDontmove();
	afx_msg void OnBnClickedCheckGroundCrash();
	afx_msg void OnBnClickedCheckAEff();
	afx_msg void OnBnClickedRadioACrash();
	afx_msg void OnBnClickedRadioAHigh();
	afx_msg void OnBnClickedRadioACrashs();
	afx_msg void OnBnClickedRadioATimeover();
	afx_msg void OnBnClickedRadioADontmove();
	afx_msg void OnBnClickedButtonSelect();
};
