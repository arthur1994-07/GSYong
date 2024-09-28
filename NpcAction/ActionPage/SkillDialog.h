#pragma once


#include "../../RanLogic/NpcTalk/NpcTalkCondition.h"

// CnSkillDialog 대화 상자입니다.


class CSkillDialog : public CDialog
{
	DECLARE_DYNAMIC(CSkillDialog)

public:
	CSkillDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSkillDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LEARNSKILLDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
