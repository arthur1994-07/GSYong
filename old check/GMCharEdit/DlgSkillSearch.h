#pragma once

#include <vector>
#include "../RanLogic/Character/GLCharData.h"
#include "resource.h"
#include "afxcmn.h"
#include "afxwin.h"

namespace gmce
{
    class IDBManager;
}

// DlgSkillSearch �� ���Դϴ�.

class DlgSkillSearch : public CFormView
{
	DECLARE_DYNCREATE(DlgSkillSearch)

protected:
	DlgSkillSearch();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~DlgSkillSearch();

public:
	enum { IDD = IDD_DLG_SKILL_SEARCH };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
    gmce::IDBManager* m_pDBMan;
    std::vector<SCHARSKILL> m_SelectedSkill;
    bool m_bSearchProgress;
    void SearchThreadProc();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedBtnSkillSearchSkillChoose();
    afx_msg void OnBnClickedBtnSkillSearchStart();
    CListCtrl m_ListSelectedSkill;
    CProgressCtrl m_ProgressSearch;
    CStatic m_ProgressText;
protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
    virtual void OnInitialUpdate();
};


