#pragma once


#include "../../RanLogic/NpcTalk/NpcTalkCondition.h"

// CnSkillDialog ��ȭ �����Դϴ�.


class CSkillDialog : public CDialog
{
	DECLARE_DYNAMIC(CSkillDialog)

public:
	CSkillDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CSkillDialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_LEARNSKILLDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

private:
	SSkillCondition	m_Condition;
	BOOL			m_bConfirm;

public:
	SSkillCondition	GetSkillCondition ();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonFind();
};
