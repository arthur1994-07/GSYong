#pragma once

#include "../../RanLogic/Skill/GLSkill.h"

// CCreatureSetDlg ��ȭ �����Դϴ�.

class CCreatureSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CCreatureSetDlg)

public:
	CCreatureSetDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CCreatureSetDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_RECALL };

	SKILL::SSPECIAL_SKILL m_sSPECIAL_SKILL;
	int			          m_nSelectLevel;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonGateeffect();
};
