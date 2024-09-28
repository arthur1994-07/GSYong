#pragma once
#include "afxcmn.h"

#include "../../RanLogic/Character/GLCharData.h"
#include <vector>
#include "../resource.h"

// CDlgSkillAdd 대화 상자입니다.

class CDlgSkillAdd : public CDialog
{
	DECLARE_DYNAMIC(CDlgSkillAdd)

public:
	CDlgSkillAdd(bool bAllSkill, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgSkillAdd();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_SKILL_ADD };

protected:
    bool m_bAllSkill;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    void InitListHead();
    void SetData();
    CListCtrl m_ListSkillAdd;
    std::vector<SCHARSKILL> m_vSkill;
    afx_msg void OnBnClickedOk();

    int m_nSelected;
    std::vector<int> m_vSelected;

	afx_msg void OnNMDblclkListSkillAdd(NMHDR *pNMHDR, LRESULT *pResult);
};
