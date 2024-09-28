#pragma once

#include "../../RanLogic/Skill/GLSkill.h"

// CCreatureSetDlg 대화 상자입니다.

class CCreatureSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CCreatureSetDlg)

public:
	CCreatureSetDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CCreatureSetDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_RECALL };

	SKILL::SSPECIAL_SKILL m_sSPECIAL_SKILL;
	int			          m_nSelectLevel;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonGateeffect();
};
