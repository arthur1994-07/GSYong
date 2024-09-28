#pragma once
#include "afxcmn.h"

#include "../../RanLogic/Character/GLCharData.h"
#include <vector>
#include "../resource.h"

// CDlgSkillAdd ��ȭ �����Դϴ�.

class CDlgSkillAdd : public CDialog
{
	DECLARE_DYNAMIC(CDlgSkillAdd)

public:
	CDlgSkillAdd(bool bAllSkill, CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgSkillAdd();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_SKILL_ADD };

protected:
    bool m_bAllSkill;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
